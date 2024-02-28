#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <string.h>
#include <pthread.h>
#include <stdbool.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdatomic.h>
#include "lib/queue.h"
#include "lib/bib_ds.h"
#include "lib/pars.h"
#include "lib/log_func.h"

#define SOCKET_PATH "./socket/temp_sock"
#define THIS_PATH "server.c/"
#define CONFIG_FILE "./config/bib.conf"
#define MAX_CLIENTS 10 // temp, it must be 40
#define MAX_NAME_LENGTH (10 * sizeof(char))
#define TERMINATION_SENTINEL -1

// TODO - per ora non lancia con 'bibserver' ma con './bibserver'
#define USAGE "Run with:\n\n\t$ bibserver name_bib file_record W\n\n\'name_bib\' is the name of the library, \'file_record\' is the path of the file containing the records, \'W\' is the number of workers.\n"

// Messaggio per richiedere i record che contengono alcune parole specifiche in alcuni campi
#define MSG_QUERY 'Q'
// Messaggio per richiedere il prestito di tutti i record che contengono alcune parole specifiche in alcuni campi
#define MSG_LOAN 'L'
// Messaggio di invio record. Il campo buffer contiene il record completo come stringa correttamente terminata da '\0'.
#define MSG_RECORD 'R'
// Messaggio di risposta negativa. Con questo messaggio il server segnala che non ci sono record che verificano la query
#define MSG_NO 'N'
// MSG ERROR Messaggio di errore. Questo tipo di messaggio viene spedito quando si è verificato un errore nel processare la richiesta del client.
#define MSG_ERROR 'E'

// global variables
_Atomic bool termination_flag = false;
pthread_t *tid;
int W;
char *bib_path;
BibData *bib;
int server_socket;
char *name_bib;
Queue *q;

typedef struct WorkerArgs
{
    Queue *q;
    BibData *bib;
    FILE *log_file;
    pthread_t tid;
    // bool *term_flag;
} WorkerArgs;

void *worker(void *arg);
void sendData(int clientFD, char type, char *data);
char readData(int clientFD, char **data);
void checkArgs(int argc, char *argv[]);
void signalHandler(int signum);
void writeServerInfo(const char *name, const char *socket_path);
void rmServerInfo(const char *name);

// TODO - quando il client riceve i record risultanti sono sempre preceduti da un qualche carattere a cazzo di cane

int main(int argc, char *argv[])
{
    checkArgs(argc, argv);

    name_bib = argv[1];
    bib_path = argv[2];
    W = atoi(argv[3]);

    // termination handler registration
    // TODO - segnale di terminazione (quando fatto levare parametro tid da queue_pop)
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);

    // read the record file
    bib = createBibData(bib_path);
    // if NULL the file on the path given doesn't exits
    if (bib == NULL)
    {
        printf("ERROR: the given path does not correspond to any existing file\n");
        exit(EXIT_FAILURE);
    }

    // create log file
    FILE *log_file = openLogFile(name_bib);

    // * socket creation
    server_socket = socket(AF_UNIX, SOCK_STREAM, 0);
    if (server_socket == -1)
    {
        // error handling
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    // * socket address definition
    struct sockaddr_un server_address;
    server_address.sun_family = AF_UNIX;
    char *socket_path = (char *)malloc(strlen(name_bib) + strlen("socket/") + strlen("_sock"));
    if (socket_path == NULL)
    {
        // error handling
        perror(THIS_PATH "main - socket_path allocation failed");
        exit(EXIT_FAILURE);
    }
    strcpy(socket_path, "socket/");
    strcat(socket_path, name_bib);
    strcat(socket_path, "_sock");
    strcpy(server_address.sun_path, socket_path);

    // * association of the socket to the address
    if (bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address)) == -1)
    {
        // error handling
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // * Listen to the socket for incoming connections
    if (listen(server_socket, MAX_CLIENTS) == -1)
    {
        // error handling
        perror("listen failed");
        exit(EXIT_FAILURE);
    }

    // write server info in the conf file
    writeServerInfo(name_bib, socket_path);

    q = (Queue *)malloc(sizeof(Queue));
    if (q == NULL)
    {
        // error handling
        perror(THIS_PATH "main - q allocation failed");
        exit(EXIT_FAILURE);
    }
    queue_init(q);

    // thread creation
    tid = (pthread_t *)malloc(sizeof(pthread_t) * W);
    if (tid == NULL)
    {
        // error handling
        perror(THIS_PATH "main - tid[] allocation failed");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < W; i++)
    {
        WorkerArgs *args = (WorkerArgs *)malloc(sizeof(WorkerArgs));
        if (args == NULL)
        {
            // error handling
            perror(THIS_PATH "main - args failed");
            exit(EXIT_FAILURE);
        }
        args->q = q;
        args->bib = bib;
        args->log_file = log_file;
        args->tid = i;
        // args->term_flag = &termination_flag;
        pthread_create(&tid[i], NULL, worker, (void *)args);
    }

    // TODO - poi la guardia del while dovrà essere gestita con il segnale di terminazione
    // * main cycle
    while (1)
    {
        int client_fd = accept(server_socket, NULL, NULL);
        if (client_fd == -1)
        {
            // error handling
            perror("accept failed");
            exit(EXIT_FAILURE);
        }
        char *data, type = readData(client_fd, &data);

        Request *req = requestFormatCheck(data, type, client_fd);
        if (req == NULL)
        {
            req = (Request *)malloc(sizeof(Request));
            if (req == NULL)
            {
                // error handling
                perror(THIS_PATH "main - req allocation failed");
                exit(EXIT_FAILURE);
            }
            req->senderFD = client_fd;
            req->size = -1;
        }

        queue_push((void *)req, q);
    }

    return 0;
}

void *worker(void *arg)
{
    // TODO - il server taglia gli ultimi caratteri dei msg che manda al client
    // take data from arg data structure
    Queue *queue = ((WorkerArgs *)arg)->q;
    BibData *bib = ((WorkerArgs *)arg)->bib;
    FILE *log_file = ((WorkerArgs *)arg)->log_file;
    pthread_t stid = ((WorkerArgs *)arg)->tid;

    while (!atomic_load(&termination_flag))
    {
        if (atomic_load(&termination_flag))
        {
            return NULL;
        }
        // take request from shared data queue
        void *data = queue_pop(q, NULL, NULL);
        int *int_ptr = (int *)data;
        int result = *int_ptr;

        if((*((int *)data)) == TERMINATION_SENTINEL) {
            // @ temp test
            printf("TROVATO termination sentinel : %d\n", TERMINATION_SENTINEL);
            return NULL;
        }

        Request *req = ((Request *)queue_pop(queue, &termination_flag, stid));
        if (atomic_load(&termination_flag))
        {
            return NULL;
        }
        if (req == NULL)
        {
            return NULL;
        }
        if (req->size == -1)
        {
            // request error
            sendData(req->senderFD, MSG_ERROR, "");
        }
        else
        {
            // search in the shared data structure
            Response *response = searchRecord(bib, req);
            if (response == NULL)
            {
                // nothing find
                sendData(req->senderFD, MSG_NO, "");
                // TODO - update log file
                req->loan ? logLoan(log_file, "", 0) : logQuery(log_file, "", 0);
            }
            else
            {
                // // // @ temp test
                // // printf("WORKER\n");
                // // for (int i = 0; i < response->size; i++)
                // // {
                // //     // @ temp test
                // //     printf("\t%s\n", bib->book[response->pos[i]]);
                // // }
                int size = 0;
                char *data = (char *)malloc(sizeof(char));
                if (data == NULL)
                {
                    // error handling
                    perror(THIS_PATH "worker - data allocation failed");
                    exit(EXIT_FAILURE);
                }
                for (int i = 0; i < response->size; i++)
                {
                    // reallocation of data for the string to append
                    data = realloc(data, strlen(bib->book[response->pos[i]]) + size + 1);
                    size += strlen(bib->book[response->pos[i]] + 1);
                    while (bib->book[response->pos[i]][0] < 65 || bib->book[response->pos[i]][0] > 122)
                    {
                        bib->book[response->pos[i]]++;
                    }
                    // concat the strings
                    strcat(data, bib->book[response->pos[i]]);
                    data[size] = '\n';
                }

                // fix the format
                int i = size;
                while (data[i] == '\n' || data[i] == '\0')
                {
                    // if (data[i] == '\n')
                    // {
                    //     size--;
                    // }
                    i--;
                }
                data[++size] = '\0';
                i = 0;
                while (data[i] < 65 || data[i] > 122)
                {
                    data++;
                    i++;
                }

                // @ temp test
                printf("WORKER - |%s|\n", data);

                // send data to client
                sendData(req->senderFD, MSG_RECORD, data);

                // aggiorna file di log
                req->loan ? logLoan(log_file, data, response->size >= 0 ? true : false) : logQuery(log_file, data, response->size);
            }
        }
        if (atomic_load(&termination_flag))
        {
            return NULL;
        }
    }
    return NULL;
}

/*
### Description
    Send the data to the server with the right comunication protocol: type -> length -> data
### Parameters
    - `int clientFD` is the file descriptor
    - `char type` is the char with the type of msg for the client: MSG_RECORD, MSG_NO, MSG_ERROR
    - `char *data` is the msg for the client.
        on search success contain the records finded
        on fail or error is empty
*/
void sendData(int clientFD, char type, char *data)
{
    // send type
    if (send(clientFD, &type, sizeof(char), 0) == -1)
    {
        // error handling
        perror(THIS_PATH "sendData - type sending failed");
        exit(EXIT_FAILURE);
    }
    // send length
    send_int(strlen(data), clientFD);
    // send data
    if (send(clientFD, data, strlen(data), 0) == -1)
    {
        // error handling
        perror(THIS_PATH "sendData - data sending failed");
        exit(EXIT_FAILURE);
    }
}

/*
### Description
    Read data (request) from server whith the right comunication protocol: type -> length -> data
### Parameters
    - `int clientFD` is the file descriptor
    - `char **data` is the pointer where save the request string from the client
### Return value
    On success return the char with the type
    On fail print an error msg and exit
*/
char readData(int clientFD, char **data)
{
    char type;
    // read type
    int length, bytesread = read(clientFD, &type, sizeof(char));
    if (bytesread == -1)
    {
        // error handling
        perror(THIS_PATH "readData - type reading failed");
        exit(EXIT_FAILURE);
    }
    // read length
    length = receive_int(clientFD);
    *data = (char *)malloc((length + 1) * sizeof(char));
    if (data == NULL)
    {
        // error handling
        perror(THIS_PATH "readData - data failed");
        exit(EXIT_FAILURE);
    }
    bytesread = read(clientFD, *data, length);
    if (bytesread == -1)
    {
        // error handling
        perror(THIS_PATH "readData - data reading failed");
        exit(EXIT_FAILURE);
    }
    (*data)[bytesread] = '\0';
    return type;
}

/*
### Description
    Handler of SIGINT/SIGTERM signal, it does all the things todo before kill the program
### Parameters
    - `int signum` is the identifier of the signal
*/
void signalHandler(int signum)
{
    termination_flag = true;
    // @ temp test
    printf("\nTermination signal received: %d\n", signum);
    // remove this server infos from conf file
    // @ temp test
    printf("\trimozione info da bib.conf\n");
    rmServerInfo(name_bib);

    // TODO - attendere fine di tutti i worker, new approach: push a termination parameter on shared request queue
    // @ temp test
    printf("\tinserimento termination sentinel nella coda\n");
    for (int i = 0; i < W; i++) {
        int *temp = (int *)malloc(sizeof(int));
        *temp = TERMINATION_SENTINEL;
        queue_push(q, temp);
    }
    // needed: tid[i]
    // @ temp test
    // printf("\tattesa join dei thread\n");
    // for (int i = 0; i < W; i++)
    // {
    //     pthread_join(tid[i], NULL);
    //     // @ temp test
    //     printf("terminato thread %d\n", i);
    // }
    // TODO - termianre scrittura log file
    // // in teoria lo fanno i worker, quindi è compreso nel punto sopra
    // // TODO - registrare nuovo record file
    // // needed: file record fd, bibData
    // // @ temp test
    // printf("\tupdating record file\n");
    // if (updateRecordFile(bib_path, bib) < 0) {
    //     // error handling
    //     printf("%ssignalHandler - error updating record file\n", THIS_PATH);
    // }
    // TODO - eliminare la socket del server
    // // needed: server socket fd
    // // @ temp test
    // printf("\tchiusura socket\n");
    // close(server_socket);
    // unlink(SOCKET_PATH);
    // TODO - registra nuovo file_record

    exit(EXIT_SUCCESS);
}

/*
### Description
    Checks the user argument, they must be 4 (3 arg + 1 exe_name), the strings lengths must be minus than a certain maximum and the last arg must be a number
### Parameters
    - `int argc` is the number of args
    - `char *argv[]` is an array of strings, each string is an arg
*/
void checkArgs(int argc, char *argv[])
{
    // check the input arguments
    if (argc < 4)
    {
        printf("ERROR: misssing arguments\n%s", USAGE);
        exit(EXIT_FAILURE);
    }
    else if (argc > 4)
    {
        printf("ERROR: too many arguments\n%s", USAGE);
        exit(EXIT_FAILURE);
    }

    // check name_bib
    if (strlen(argv[1]) > MAX_NAME_LENGTH)
    {
        printf("ERROR: bib name given is too long, the maximum is 10 characters\n");
        exit(EXIT_FAILURE);
    }

    // check W
    if (atoi(argv[3]) < 1 || atoi(argv[3]) > 5)
    {
        printf("ERROR: W must be between 1 and 5\n");
        exit(EXIT_FAILURE);
    }
}

/*
### Description
    Write `bib_server_name bib_server_path` in the conf file (when the server create the socket)
### Parameters
    - `const char *name` is the name of the server bib (bib_server_name)
    - `const char *socket_path` is the path of the server socket (bib_server_path)
*/
void writeServerInfo(const char *name, const char *socket_path)
{
    // TODO - controllare se il nome è già presente, in caso restituire "Errore: biblioteca già presente nel file di configurazione"
    FILE *config_file = fopen(CONFIG_FILE, "a"); // Open the file in append mod
    if (config_file == NULL)
    {
        // @ temp test
        perror(THIS_PATH "writeServerInfo - Error opening config file");
    }

    // Write the server infos in the configuration file
    fprintf(config_file, "%s %s\n", name, socket_path);

    fclose(config_file);
}

/*
### Description
    Remove `bib_server_name bib_server_path` from the conf file (when the server terminates and closes the socket)
### Parameters
    - `const char *name` is the name (bib_server_name) of the bib server to remove
*/
void rmServerInfo(const char *name)
{
    // TODO - se termina prima che il client faccia richieste ok, altrimenti da segmentation fault (a volte)
    FILE *config_file = fopen(CONFIG_FILE, "rw");
    if (config_file == NULL)
    {
        // error handling
        perror(THIS_PATH "rmServerInfo - config_file allocation failed");
        exit(EXIT_FAILURE);
    }

    FILE *temp_file = fopen("temp.conf", "w");
    if (temp_file == NULL)
    {
        // error handling
        perror(THIS_PATH "rmServerInfo - temp_file allocation failed");
        exit(EXIT_FAILURE);
    }

    char *temp_name = (char *)malloc(sizeof(char) * 100);
    if (temp_name == NULL)
    {
        // error handling
        perror(THIS_PATH "rmServerInfo - temp_name allocation failed");
        exit(EXIT_FAILURE);
    }
    char *temp_path = (char *)malloc(sizeof(char) * 100);
    if (temp_path == NULL)
    {
        // error handling
        perror(THIS_PATH "rmServerInfo - temp_path allocation failed");
        exit(EXIT_FAILURE);
    }
    while (fscanf(config_file, "%s %s", temp_name, temp_path) == 1)
    {
        if (strcmp(temp_name, name) == 0)
        {
            // @ temp test
            printf("RMsERVERiNFO - removed\n");
        }
        else
        {
            fprintf(temp_file, "%s %s\n", temp_name, temp_path);
        }
    }
    // @ temp test
    printf("RMSERVERINFO - while done, closing files fds\n");
    fclose(config_file);
    fclose(temp_file);
    // @ temp test
    printf("RMSERVERINFO - files closed\n");
    if (remove(CONFIG_FILE) != 0)
    {
        perror(THIS_PATH "rmServerInfo - Errore nella rimozione del file originale");
        exit(EXIT_FAILURE);
    }
    // @ temp test
    printf("RMSERVERINFO - old config file removed\n");
    if (rename("temp.conf", "config/bib.conf") != 0)
    {
        perror(THIS_PATH "rmServerInfo - Errore nel rinominare il file temporaneo");
        exit(EXIT_FAILURE);
    }
}