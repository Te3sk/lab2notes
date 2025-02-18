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
#include <semaphore.h>
#include <fcntl.h>
#include "lib/queue.h"
#include "lib/bib_ds.h"
#include "lib/pars.h"
#include "lib/log_func.h"

// TODO - ancora problemi col segnale di terminazione, in particolare con la join dei thread
// TODO - fix invalid type(): i client stampano molti invalid type per qualche motivo, credo che sia per problemi nel server sul segnale di terminazione (i worker smettono di lavorare prima di finire la coda)
// TODO - cambiare nomi file e pulire cordice
// TODO - testare tutto su macchina di alina

#define THIS_PATH "server.c/"
#define CONFIG_FILE "./config/bib.conf"
#define MAX_CLIENTS 40
#define MAX_NAME_LENGTH (10 * sizeof(char))
#define TERMINATION_SENTINEL -1
#define BIB_MUTEX_NAME "bib_conf_mutex"

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

// # global variables
//
volatile sig_atomic_t terminate = 0;
// thread worker number (user parameter)
int W;
// name of the server (user parameter)
char *name_bib;
// path of the data set (user parameter)
char *bib_path;
// file descriptor of the socket
int server_socket;
// path of the socket
char *socket_path;
// file descriptor of the log file
int log_file;
// array of thread ids
pthread_t *tid;
// shared data strucutre filled with all data set
BibData *bib;
// shared queue for the reqeuest
Queue *q;

void *worker();
void sendData(int clientFD, char type, char *data);
char readData(int clientFD, char **data);
void checkArgs(int argc, char *argv[]);
void signalHandler(int signum);
void writeServerInfo(const char *name, const char *socket_path);
void rmServerInfo(const char *name);
void freeMem();

int main(int argc, char *argv[])
{
    // checking user arguments (number and format)
    checkArgs(argc, argv);

    name_bib = argv[1];
    bib_path = argv[2];
    W = atoi(argv[3]);

    // termination handler registration
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);

    // read the record file
    bib = createBibData(bib_path);
    if (bib == NULL)
    {
        // if NULL the file on the path given doesn't exits
        printf("%s - ERROR: error in record file format or creation of bibData\n", name_bib);
        freeMem();
        exit(EXIT_FAILURE);
    }

    // create log file
    log_file = openLogFile(name_bib);
    if (log_file == -1)
    {
        // error handling
        perror(THIS_PATH "main - log file opening failed");
        exit(EXIT_FAILURE);
    }

    //  socket creation
    server_socket = socket(AF_UNIX, SOCK_STREAM, 0);
    if (server_socket == -1)
    {
        // error handling
        perror("socket creation failed");
        freeMem();
        exit(EXIT_FAILURE);
    }

    // socket address definition
    struct sockaddr_un server_address;
    server_address.sun_family = AF_UNIX;
    socket_path = (char *)malloc(strlen(name_bib) + strlen("socket/") + strlen("_sock") + 1);
    if (socket_path == NULL)
    {
        // error handling
        perror(THIS_PATH "main - socket_path allocation failed");
        freeMem();
        exit(EXIT_FAILURE);
    }
    sprintf(socket_path, "socket/%s_sock", name_bib);
    strcpy(server_address.sun_path, socket_path);

    // association of the socket to the address
    if (bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address)) == -1)
    {
        // error handling
        perror("bind failed");
        freeMem();
        exit(EXIT_FAILURE);
    }

    // Listen to the socket for incoming connections
    if (listen(server_socket, MAX_CLIENTS) == -1)
    {
        // error handling
        perror("listen failed");
        freeMem();
        exit(EXIT_FAILURE);
    }

    // write server info in the conf file
    writeServerInfo(name_bib, socket_path);

    // declaration and initialization of the share queue for the requests
    q = (Queue *)malloc(sizeof(Queue));
    if (q == NULL)
    {
        // error handling
        perror(THIS_PATH "main - q allocation failed");
        freeMem();
        exit(EXIT_FAILURE);
    }
    queue_init(q);

    // thread worker creation
    tid = (pthread_t *)malloc(sizeof(pthread_t) * W);
    if (tid == NULL)
    {
        // error handling
        perror(THIS_PATH "main - tid[] allocation failed");
        freeMem();
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < W; i++)
    {
        // declaration and initialization of the struct for the thread worker arguments
        pthread_create(&tid[i], NULL, worker, NULL);
    }

    Request *req = (Request *)malloc(sizeof(Request));

    // main cycle
    while (1)
    {
        // accept connection
        int client_fd = accept(server_socket, NULL, NULL);
        if (client_fd == -1)
        {
            // error handling
            perror("accept failed");
            freeMem();
            exit(EXIT_FAILURE);
        }
        // take request from the client on the socket
        char *data, type = readData(client_fd, &data);

        // check the format of the request and fill the struct
        req = requestFormatCheck(data, type, client_fd);
        if (req == NULL)
        {
            req = (Request *)malloc(sizeof(Request));
            if (req == NULL)
            {
                // error handling
                perror(THIS_PATH "main - req allocation failed");
                // freeMem();
                exit(EXIT_FAILURE);
            }
            req->senderFD = client_fd;
            // size = -1 is the sentinel for the error msg
            req->size = -1;
        }

        queue_push((void *)req, q);
    }

    free_request(req);
    return 0;
}

void *worker()
{

    while (!terminate)
    {
        // TODO - ora termina appena arriva al segnaele, controllare anche queue_is_empty nella guardia
        // take request from shared queue
        void *value = queue_pop(q);
        // check if the popped value is the termination sentinel
        if ((*((int *)value)) == TERMINATION_SENTINEL)
        {
            break;
        }

        // cast value to the right type
        Request *req = ((Request *)value);

        if (req == NULL)
        {
            continue;
        }

        // check the sentinel for the error msg
        if (req->size == -1)
        {
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
                // update log file
                req->loan ? logLoan(log_file, "", 0) : logQuery(log_file, "", 0);
                // free_request(req);
            }
            else
            {
                // initializing size and the string to send to the client
                size_t size = 0;
                // compute data size
                for (int i = 0; i < response->size; i++)
                {
                    // length of records
                    size += (strlen(bib->book[response->pos[i]])) * sizeof(char);
                }
                // add one '\n' for each record + one '\0' for the end
                size += (response->size + 1) * sizeof(char);
                char *data = (char *)malloc(size);
                if (data == NULL)
                {
                    // error handling
                    perror(THIS_PATH "worker - data allocation failed");
                    exit(EXIT_FAILURE);
                }

                int index = 0;

                // iter the records that matched with the request
                for (int i = 0; i < response->size; i++)
                {
                    // concat the strings
                    strcat(data, bib->book[response->pos[i]]);
                    index += strlen(bib->book[response->pos[i]]);

                    data[++index] = '\n';
                    data[++index] = '\0';
                }

                // end string sign
                data[index] = '\0';

                // remove strange char at the beginning
                int i = 0;
                // while ((data[i] < 65 || data[i] > 122))
                while ((data[i] < 48 || data[i] > 57) && (data[i] < 65 || data[i] > 90) && (data[i] < 97 || data[i] > 122))
                {
                    data++;
                    i++;
                }

                // send data to client
                sendData(req->senderFD, MSG_RECORD, data);

                // aggiorna file di log
                req->loan ? logLoan(log_file, data, response->size) : logQuery(log_file, data, response->size);
            }
        }
    }
    pthread_exit(NULL);
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

    // read data
    bytesread = read(clientFD, *data, length);
    if (bytesread == -1)
    {
        // error handling
        perror(THIS_PATH "readData - data reading failed");
        exit(EXIT_FAILURE);
    }

    // end string sign
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
    if (signum == SIGINT || signum == SIGTERM)
    {
        terminate = 1;
    }
    else
    {
        printf("Unknown signal : %d", signum);
        exit(EXIT_FAILURE);
    }

    // remove this server infos from conf file
    rmServerInfo(name_bib);
    int *temp = (int *)malloc(sizeof(int));
    *temp = TERMINATION_SENTINEL;
    for (int i = 0; i < W; i++)
    {
        queue_push((void *)temp, q);
    }

    for (int i = 0; i < W; i++)
    {
        pthread_join(tid[i], NULL);
    }
    free(temp);

    // close server socket
    close(server_socket);
    unlink(socket_path);

    // end log file writing
    close(log_file);

    // write new record_file
    if (updateRecordFile(name_bib, bib_path, bib) < 0)
    {
        // error handling
        printf("%sSignalHandler - error while updating record file\n", THIS_PATH);
    }

    // freeMem();
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
    sem_t *bib_mutex = sem_open(BIB_MUTEX_NAME, O_CREAT, S_IRWXU, 1);
    if (bib_mutex == SEM_FAILED)
    {
        perror("unable to obtain the semaphore");
        return;
    }

    if (sem_wait(bib_mutex) == -1)
    {
        perror("unable to wait on the semaphore");
        return;
    }

    // Open the file in append mod
    FILE *config_file = fopen(CONFIG_FILE, "a");
    if (config_file == NULL)
    {
        perror(THIS_PATH "writeServerInfo - Error opening config file");
        return;
    }

    // Write the server infos in the configuration file
    fprintf(config_file, "%s %s\n", name, socket_path);

    fclose(config_file);

    if (sem_post(bib_mutex) == -1)
    {
        perror("unable to post on the semaphore");
        return;
    }

    if (sem_close(bib_mutex) == -1)
    {
        perror("unable to close the semaphore");
        return;
    }
}

/*
### Description
    Remove `bib_server_name bib_server_path` from the conf file (when the server terminates and closes the socket).
    A temporary file is created, filled only with information from other servers and finally replaced with the original.
### Parameters
    - `const char *name` is the name (bib_server_name) of the bib server to remove
*/
void rmServerInfo(const char *name)
{
    sem_t *bib_mutex = sem_open(BIB_MUTEX_NAME, O_CREAT, S_IRWXU, 1);
    if (bib_mutex == SEM_FAILED)
    {
        perror("unable to obtain the semaphore");
        return;
    }

    if (sem_wait(bib_mutex) == -1)
    {
        perror("unable to wait on the semaphore");
        return;
    }

    // Open the file in read/write mod
    FILE *config_file = fopen(CONFIG_FILE, "r+");

    if (config_file == NULL)
    {
        // error handling
        perror(THIS_PATH "rmServerInfo - config_file allocation failed");
        // exit(EXIT_FAILURE);
    }

    // open a temp file
    char *temp_file_path = (char *)malloc(sizeof(char) * (strlen("config/") + strlen(name) + strlen("_temp.conf") + 1));
    sprintf(temp_file_path, "config/%s_temp.conf", name);
    FILE *temp_file = fopen(temp_file_path, "w");
    if (temp_file == NULL)
    {
        // error handling
        perror(THIS_PATH "rmServerInfo - temp_file allocation failed");
        // exit(EXIT_FAILURE);
    }

    // read each line
    char *temp_name = (char *)malloc(sizeof(char) * 1024);
    char *temp_path = (char *)malloc(sizeof(char) * 1024);

    while (fscanf(config_file, "%s %s", temp_name, temp_path) == 1)
    {
        // TODO - error handling
        if (strcmp(temp_name, name) != 0)
        {
            // if isn't the name of the current server, write it and the path in the temp file
            fprintf(temp_file, "%s %s\n", temp_name, temp_path);
        }
    }

    // close files
    fclose(config_file);
    fclose(temp_file);

    if(remove(CONFIG_FILE) != 0){
        // error handling
        perror(THIS_PATH"rmServerInfo - original file removing failed");
        exit(EXIT_FAILURE);
    }

    if(rename(temp_file_path, CONFIG_FILE) != 0){
        // error handling
        perror(THIS_PATH"rmServerInfo - new conf file renaming failed");
        exit(EXIT_FAILURE);
    }

    
    if (sem_post(bib_mutex) == -1)
    {
        perror("unable to post on the semaphore");
        return;
    }

    if (sem_close(bib_mutex) == -1)
    {
        perror("unable to close the semaphore");
        return;
    }

    if(sem_unlink(BIB_MUTEX_NAME)==-1 && errno!=ENOENT){
        perror("unable to unlink the semaphore");
        return;
    }
}

void freeMem()
{
    if (name_bib != NULL)
    {
        free(name_bib);
    }
    if (bib_path != NULL)
    {
        free(bib_path);
    }
    if (socket_path != NULL)
    {
        free(socket_path);
    }
    if (q != NULL)
    {
        queue_destroy(q);
        free(q);
    }
    if (tid != NULL)
    {
        free(tid);
    }
    if (bib != NULL)
    {
        freeBib(bib);
    }
}
