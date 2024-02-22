#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <string.h>
#include <pthread.h>
#include <stdbool.h>
#include <string.h>
#include <arpa/inet.h>
#include "lib/queue.h"
#include "lib/bib_ds.h"
#include "lib/pars.h"
#include "lib/log_func.h"

#define SOCKET_PATH "./socket/temp_sock"
#define THIS_PATH "server.c/"
#define CONFIG_FILE "./config/bib.conf"
#define MAX_CLIENTS 10                      // temp, it must be 40
#define MAX_LENGTH 500                      // TODO - understand how many bytes give
#define MAX_NAME_LENGTH (10 * sizeof(char)) // TODO - understrand how many

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

// initalization of termination flag
int termination_flag = 0;
// TODO - temp, W have to be insert by user in program launch
// #define W 4

typedef struct WorkerArgs
{
    Queue *q;
    BibData *bib;
    FILE *log_file;
    int *term_flag;
} WorkerArgs;

void *worker(void *arg);
void sendData(int clientFD, char type, char *data);
char readData(int clientFD, char **data);
void checkArgs(int argc, char *argv[]);
void signalHandler(int signum);

// TODO - quando il client riceve i record risultanti sono sempre preceduti da un qualche carattere a cazzo di cane

int main(int argc, char *argv[])
{
    checkArgs(argc, argv);

    char *name_bib = argv[1];
    char *bib_path = argv[2];
    int W = atoi(argv[3]);

    // @ temp test
    printf("%s\n%s\n%d\n", name_bib, bib_path, W);

    // termination handler registration
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);

    // read the record file
    BibData *bib = createBibData(bib_path);
    // if NULL the file on the path given doesn't exits
    if (bib == NULL)
    {
        printf("ERROR: the given path does not correspond to any existing file\n");
        exit(EXIT_FAILURE);
    }

    // create log file
    FILE *log_file = openLogFile(name_bib);

    // * socket creation
    int server_socket = socket(AF_UNIX, SOCK_STREAM, 0);
    if (server_socket == -1)
    {
        // error handling
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    // * socket address definition
    struct sockaddr_un server_address;
    server_address.sun_family = AF_UNIX;
    char *socket_path = (char*)malloc(strlen(name_bib) + strlen("socket/"));
    strcpy(socket_path, "socket/");
    strcat(socket_path, name_bib);
    strcpy(server_address.sun_path, SOCKET_PATH);

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

    Queue *q = (Queue *)malloc(sizeof(Queue));
    queue_init(q);

    // thread creation
    pthread_t tid[W];
    for (int i = 0; i < W; i++)
    {
        WorkerArgs *args = (WorkerArgs *)malloc(sizeof(WorkerArgs));
        args->q = q;
        args->bib = bib;
        args->log_file = log_file;
        args->term_flag = &termination_flag;
        pthread_create(&tid[i], NULL, worker, (void *)args);
    }

    // TODO - poi dovrà essere gestito con il segnale di terminazione
    // * main cycle
    while ((!termination_flag))
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
            req->senderFD = client_fd;
            req->size = -1;
        }

        queue_push((void *)req, q);
    }

    for (int i = 0; i < W; i++)
    {
        pthread_join(tid[i], NULL);
    }

    // TODO - registra nuovo file_record
    if(updateRecordFile(bib_path, bib) == NULL){
        // error handling
        perror(THIS_PATH"main - updateRecordFile failed");
        exit(EXIT_FAILURE);
    }

    close(server_socket);
    unlink(SOCKET_PATH);

    return 0;
}

void *worker(void *arg)
{
    // take data from arg data structure
    Queue *queue = ((WorkerArgs *)arg)->q;
    BibData *bib = ((WorkerArgs *)arg)->bib;
    FILE *log_file = ((WorkerArgs *)arg)->log_file;
    int *term_flag = ((WorkerArgs *)arg)->term_flag;

    while ((!(*term_flag)))
    {
        // take request from shared data queue
        Request *req = ((Request *)queue_pop(queue));

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
            }
            else
            {
                int size = 0;
                char *data = (char *)malloc(sizeof(char));
                for (int i = 0; i < response->size; i++)
                {
                    // reallocation of data for the string to append
                    data = realloc(data, strlen(bib->book[response->pos[i]]) + size + 1);
                    size += strlen(bib->book[response->pos[i]] + 1);
                    // concat the strings
                    strcat(data, bib->book[response->pos[i]]);
                    data[size] = '\n';
                }

                // fix the format
                int i = size;
                while (data[i] == '\n' || data[i] == '\0')
                {
                    if (data[i] == '\n')
                    {
                        size--;
                    }
                    i--;
                }
                data[++size] = '\0';
                i = 0;
                while (data[i] < 65 || data[i] > 122)
                {
                    data++;
                    i++;
                }

                // send data to client
                sendData(req->senderFD, MSG_RECORD, data);

                // TODO - update log file
                // @ temp test
                printf("\n|%s|\n", data); // 65 to 122
                // aggiorna file di log
                req->loan ? logLoan(log_file, data, response->size >= 0 ? true : false) : logQuery(log_file, data, response->size);
            }
        }
    }

    exit(1);
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

// TODO - desc
void signalHandler(int signum)
{
    termination_flag = 1;
    // @ temp test
    printf("Termination signal received\n");
}

// TODO - desc
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

// TODO - desc
void writeServerInfo(const char *name, const char *socket_path) {
    FILE *config_file = fopen(CONFIG_FILE, "a"); // Open the file in append mod
    if (config_file == NULL) {
        // @ temp test
        perror(THIS_PATH "writeServerInfo - Error opening config file");
        // If cannot open the file, try to create it
        // // config_file = fopen(CONFIG_FILE, "w"); // Apre il file in modalità scrittura
        // // if (config_file == NULL) {
        // //     perror("Error opening/creating config file");
        // //     exit(EXIT_FAILURE);
        // // }
    }

    // Write the server infos in the configuration file
    fprintf(config_file, "%s %s\n", name, socket_path);

    fclose(config_file);
}
