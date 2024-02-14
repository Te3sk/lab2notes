#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <string.h>
#include <pthread.h>
#include "lib/queue.h"
#include "lib/bib_ds.h"

#define SOCKET_PATH "./socket/temp_sock"
#define MAX_CLIENTS 10 // temp, it must be 40
// @ temp test
#define W 10

void *worker(void *arg);

int main()
{
    BibData *bib = createBibData("bibData/bibFake.txt");
    
    if(bib == NULL){
        // error handling
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
    if(bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address)) == -1){
        // error handling
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // * Listen to the socket for incoming connections
    if(listen(server_socket, MAX_CLIENTS) == -1){
        // error handling
        perror("listen failed");
        exit(EXIT_FAILURE);
    }

    // @ temp test
    printf("Il server è pronto e in ascolto (fd: %d)\n", server_socket);

    // TODO - thread creation


    // * main cycle
    while(1) {
        int client_fd = accept(server_socket, NULL, NULL);
        if(client_fd == -1){
            // error handling
            perror("accept failed");
            exit(EXIT_FAILURE);
        }

        // @ temp test
        printf("Nuova connessione accettata(client fd: %d)\n", client_fd);

        // TODO - put request in the queue
    }

    // TODO - poi dovrà essere gestito con il segnale di terminazione
    close(server_socket);
    unlink(SOCKET_PATH);

    return 0;
}

void *worker(void *arg) {

}