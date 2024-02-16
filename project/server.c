#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <string.h>
#include <pthread.h>
#include <stdbool.h>
#include <string.h>
#include "lib/queue.h"
#include "lib/bib_ds.h"

#define SOCKET_PATH "./socket/temp_sock"
#define THIS_PATH "server.c/"
#define MAX_CLIENTS 10 // temp, it must be 40
#define MAX_LENGTH 500 // TODO - understand how many bytes give
// TODO - temp, W have to be insert by user in program launch
#define W 10

typedef struct WorkerArgs
{
    Queue *q;
    BibData *bib;
} WorkerArgs;

void *worker(void *arg);

int main()
{
    Request *req = (Request *)malloc(sizeof(Request));

    // char temp[] = "author:Luccio, Fabrizio;title:Manuale di architettura pisana;p";
    // char temp[] = "author:Luccio, Fabrizio;title:Mathematical and Algorithmic Foundations of the Internet;p";
    char temp[] = "author:Luccio, Fabrizio;";

    // @ temp test
    printf("FROM CLIENT: %s\n", temp);

    req = requestParser(temp);

    BibData *bib = createBibData("bibData/bib1.txt");

    if (bib == NULL)
    {
        // error handling
        exit(EXIT_FAILURE);
    }
    // @ temp test
    printf("chiamato searchrecord\n");
    Response *paolo = searchRecord(bib, req);

    // @ temp test
    printf("SERVER: #record trovati %d\n", paolo->size);

    if (paolo != NULL)
    {
        for (int i = 0; i < paolo->size; i++)
        {
            printf("%s\n\n", paolo->records[i]);
        }
    }
    else
    {
        printf("Record not found\n");
    }

    // // * socket creation
    // int server_socket = socket(AF_UNIX, SOCK_STREAM, 0);
    // if (server_socket == -1)
    // {
    //     // error handling
    //     perror("socket creation failed");
    //     exit(EXIT_FAILURE);
    // }

    // // * socket address definition
    // struct sockaddr_un server_address;
    // server_address.sun_family = AF_UNIX;
    // strcpy(server_address.sun_path, SOCKET_PATH);

    // // * association of the socket to the address
    // if(bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address)) == -1){
    //     // error handling
    //     perror("bind failed");
    //     exit(EXIT_FAILURE);
    // }

    // // * Listen to the socket for incoming connections
    // if(listen(server_socket, MAX_CLIENTS) == -1){
    //     // error handling
    //     perror("listen failed");
    //     exit(EXIT_FAILURE);
    // }

    // // @ temp test
    // printf("Il server è pronto e in ascolto (fd: %d)\n", server_socket);

    // // TODO - queue init
    // Queue *q;
    // init(q);
    // // @ temp test
    // printf("queue initalized\n");

    // // TODO - thread creation

    // // * main cycle
    // while(1) {
    //     int client_fd = accept(server_socket, NULL, NULL);
    //     if(client_fd == -1){
    //         // error handling
    //         perror("accept failed");
    //         exit(EXIT_FAILURE);
    //     }

    //     // @ temp test
    //     printf("Nuova connessione accettata(client fd: %d)\n", client_fd);

    //     // TODO - put request in the queue
    // }

    // // TODO - poi dovrà essere gestito con il segnale di terminazione
    // close(server_socket);
    // unlink(SOCKET_PATH);

    return 0;
}

void *worker(void *arg)
{
    Queue *queue = ((WorkerArgs *)arg)->q;
    BibData *bib = ((WorkerArgs *)arg)->bib;
    // @ temp test
    printf("\tenter in worker func\n");

    // TODO - error for this
    // int clientFD = pop(queue);
    // @ temp test
    // printf("\tclient fd = %d\n", clientFD);

    char *buffer = (char *)malloc(MAX_LENGTH * sizeof(char));
    // int bytesread = read(clientFD, buffer, MAX_LENGTH);
    // if(bytesread == -1){
    //     // error handling
    //     perror(THIS_PATH "worker - read failed");
    //     exit(EXIT_FAILURE);
    // }

    // @ temp test
    // printf("\tclient send: %s\n", buffer);

    // TODO - waiting for lib/bib_ds.c/requestFormatCheck dev
    // if(requestFormatCheck(buffer) == comparig value){
    //     // error handling
    //     perror("worker - wrong request format");
    //     exit(EXIT_FAILURE);
    // }
}
