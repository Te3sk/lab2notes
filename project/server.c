#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <string.h>
#include "lib/queue.h"
#include "lib/bib_ds.h"

#define SOCKET_PATH "./socket/temp_sock"
#define MAX_CLIENTS 10 // temp, it must be 40

int main()
{
    BibData *temp = createBibData("bibData/bib1.txt");

    int* temp_index = searchRecord(temp, "Fulgaro", 'a');

    if(temp_index == NULL){
        // error handling
        perror("search Record failed");
        exit(EXIT_FAILURE);
    }

    int i = 0;
    while(temp_index[i] != -1) {
        printf("un risultato in pos %d\n", temp_index[i]);
        i++;
    }

    // // // * socket creation
    // // int server_socket = socket(AF_UNIX, SOCK_STREAM, 0);
    // // if (server_socket == -1)
    // // {
    // //     // error handling
    // //     perror("socket creation failed");
    // //     exit(EXIT_FAILURE);
    // // }

    // // // * socket address definition
    // // struct sockaddr_un server_address;
    // // server_address.sun_family = AF_UNIX;
    // // strcpy(server_address.sun_path, SOCKET_PATH);

    // // // * association of the socket to the address
    // // if(bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address)) == -1){
    // //     // error handling
    // //     perror("bind failed");
    // //     exit(EXIT_FAILURE);
    // // }

    // // // * Listen to the socket for incoming connections
    // // if(listen(server_socket, MAX_CLIENTS) == -1){
    // //     // error handling
    // //     perror("listen failed");
    // //     exit(EXIT_FAILURE);
    // // }

    // // // TODO - temp test
    // // printf("Il server è pronto e in ascolto (fd: %d)\n", server_socket);

    // // // * main cycle
    // // while(1) {
    // //     int client_fd = accept(server_socket, NULL, NULL);
    // //     if(client_fd == -1){
    // //         // error handling
    // //         perror("accept failed");
    // //         exit(EXIT_FAILURE);
    // //     }

    // //     // TODO - temp test
    // //     printf("Nuova connessione accettata(client fd: %d)\n", client_fd);
    // // }

    // // // TODO - poi dovrà essere gestito con il segnale di terminazione
    // // close(server_socket);
    // // unlink(SOCKET_PATH);

    return 0;
}