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

#define SOCKET_PATH "./socket/temp_sock"
#define THIS_PATH "server.c/"
#define MAX_CLIENTS 10 // temp, it must be 40
#define MAX_LENGTH 500 // TODO - understand how many bytes give

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

// TODO - temp, W have to be insert by user in program launch
// #define W 4

typedef struct WorkerArgs
{
    Queue *q;
    BibData *bib;
} WorkerArgs;

void *worker(void *arg);
void sendData(int clientFD, char type, char *data);
char readData(int clientFD, char **data);

int main(int argc, char *argv[])
{
    // check the input arguments
    if (argc < 4) {
        printf("ERROR: misssing arguments\n%s", USAGE);
        exit(EXIT_FAILURE);
    } else if (argc > 4) {
        printf("ERROR: too many arguments\n%s", USAGE);
        exit(EXIT_FAILURE);
    }
    
    char *name_bib = argv[1];
    char *bib_path = argv[2];
    int W = atoi(argv[3]);

    // @ temp test
    printf("%s\n%s\n%d\n", name_bib, bib_path, W);

    // read the record file
    BibData *bib = createBibData(bib_path);
    // if NULL the file on the path given doesn't exits
    if(bib == NULL) {
        printf("ERROR: the given path does not correspond to any existing file\n");
        exit(EXIT_FAILURE);
    }

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

    Queue *q = (Queue *)malloc(sizeof(Queue));
    queue_init(q);

    // thread creation
    for (int i = 0; i < W; i++)
    {
        pthread_t tid;
        WorkerArgs *args = (WorkerArgs *)malloc(sizeof(WorkerArgs));
        args->q = q;
        args->bib = bib;
        pthread_create(&tid, NULL, worker, (void *)args);
    }

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
        if (req == NULL) {
            req = (Request *)malloc(sizeof(Request));
            req->senderFD = client_fd;
            req->size = -1;
        }

        queue_push((void *)req, q);
    }

    // TODO - poi dovrà essere gestito con il segnale di terminazione
    close(server_socket);
    unlink(SOCKET_PATH);

    return 0;
}

void *worker(void *arg)
{
    Queue *queue = ((WorkerArgs *)arg)->q;
    BibData *bib = ((WorkerArgs *)arg)->bib;

    Request *req = ((Request *)queue_pop(queue));

    if (req->size == -1)
    {
        sendData(req->senderFD, MSG_ERROR, "");
    }
    else
    {

        Response *response = searchRecord(bib, req);

        if (response == NULL)
        {
            sendData(req->senderFD, MSG_NO, "");
        }
        else
        {
            char type = MSG_RECORD;
            int size = 0;
            char *data = (char *)malloc(sizeof(char));
            for (int i = 0; i < response->size; i++)
            {
                data = realloc(data, strlen(bib->book[response->pos[i]]) + size + 1);
                size += strlen(bib->book[response->pos[i]] + 1);
                strcat(data, bib->book[response->pos[i]]);
                data[size] = '\n';
            }

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

            sendData(req->senderFD, MSG_RECORD, data);
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
    char type, *l = (char *)malloc(10 * sizeof(char));
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