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
#define W 4

typedef struct WorkerArgs
{
    Queue *q;
    BibData *bib;
} WorkerArgs;

void *worker(void *arg);
void sendData(int clientFD, char type, char *data);
char readData(int clientFD, char **data);

int main()
{
    BibData *bib = createBibData("bibData/bib1.txt");

    // // * socket creation
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

    // @ temp test
    printf("Il server è pronto e in ascolto (fd: %d)\n", server_socket);

    Queue *q = (Queue *)malloc(sizeof(Queue));
    queue_init(q);
    // @ temp test
    printf("queue initalized\n");

    // TODO - thread creation
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

        queue_push((void *)req, q);

        // @ temp test
        printf("Nuova connessione accettata(client fd: %d)\n", client_fd);

        // TODO - put request in the queue
    }

    // TODO - poi dovrà essere gestito con il segnale di terminazione
    close(server_socket);
    unlink(SOCKET_PATH);

    // lib testing (bib_ds.c and pars.c)
    /*Request *req = (Request *)malloc(sizeof(Request));

    char temp[] = "author:Luccio, Fabrizio;title:Manuale di architettura pisana;p";
    char temp[] = "author:Luccio, Fabrizio;title:Mathematical and Algorithmic Foundations of the Internet;p";
    char temp[] = "author:Luccio, Fabrizio;title:Mathematical and Algorithmic Foundations of the Internet;editor:Morgan Kaufmann";
    char temp[] = "author:Luccio, Fabrizio;title:Mathematical and Algorithmic Foundations of the Internet;editor:CRC Press, Taylor and Francis Group;p";
    char temp[] = "author:Kernighan, Brian W.;title:Programmazione nella Pratica;p";


    // @ temp test
    printf("FROM CLIENT: %s\n\n", temp);

    req = requestParser(temp);


    if (bib == NULL)
    {
        // error handling
        exit(EXIT_FAILURE);
    }
    Response *paolo = searchRecord(bib, req);

    if (paolo != NULL)
    {
        // @ temp test
        printf("SERVER: #record trovati %d\n\n", paolo->size);
        for (int i = 0; i < paolo->size; i++)
        {
            // printf("%s\n\n", paolo->records[i]);
            if (paolo->loan) {
            // @ temp test
            printf("%s\n\n", bib->book[paolo->pos[i]]);

            } else {
                // @ temp test
                printf("prestito non disponibile\n\n");
            }
        }
    }
    else
    {
        printf("Record not found\n\n");
    }*/

    return 0;
}

void *worker(void *arg)
{
    Queue *queue = ((WorkerArgs *)arg)->q;
    BibData *bib = ((WorkerArgs *)arg)->bib;
    // @ temp test
    printf("\tenter in worker func\n");

    Request *req = ((Request *)queue_pop(queue));
    // @ temp test
    // for (int i = 0; i < req->size; i++) {
    //     printf("\t\t|%s|\t:\t|%s|\n", req->field_codes[i], req->field_values[i]);
    // }
    printf("loan%srequest\n", req->loan ? " " : " not ");

    Response *response = searchRecord(bib, req);

    if (response == NULL) {
        // @ temp test
        printf("nessun record trovato\n");
        sendData(req->senderFD, MSG_NO, "");
    } else {
        char type = MSG_RECORD;
        // @ temp test
        printf("ottenuto/i risultato/i\n");
        int size = 0;
        char *data = (char*)malloc(sizeof(char));
        for (int i = 0; i < response->size; i++) {
            data = realloc(data, strlen(bib->book[response->pos[i]]) + size + 1);
            size += strlen(bib->book[response->pos[i]] + 1);
            strcat(data, bib->book[response->pos[i]]);
            data[size] = '\n';
        }

        int i = size;
        while(data[i] == '\n' || data[i] == '\0') {
            if (data[i] == '\n') {
                size--;
            }
            i--;
        }
        data[++size] = '\0';

        sendData(req->senderFD, MSG_RECORD, data);
    }

    


    exit(1);
}

// TODO - desc
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

// TODO - desc
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