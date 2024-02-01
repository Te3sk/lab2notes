// Implementare una chat dove il server inoltra i messaggi
// dei clienti a tutti gli altri clienti connessi in quel
// momento. I clienti si connettono e mandano messaggi
// multipli presi dall'utente da tastiera

// Variante 1: implementare il server single thread

// TODO - problemi nell'uso della socket condivisa
// ! il client invia il messaggio ma il server non lo stampa e non lo reinvia,
// ! quando viene chiuso il server il client riceve il messaggio all'infinito, ma solo il client che l'ha inviato

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <sys/select.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>

#define BUFFER_SIZE 100
#define MAX_CLIENTS 10
#define PORT 2222
#define IP_ADDRESS "192.168.111.47"

int main()
{
    int server_fd, client_fd, bytesread;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addrlen = sizeof(client_addr);
    // TODO - BUFFER_SIZE variable to initialize
    char buffer[BUFFER_SIZE];

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    // AF_INET to run both client and server on the same machine
    server_addr.sin_family = AF_INET;
    // TODO - PORT and IP_ADDRESS variables to initialize
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr(IP_ADDRESS);

    // Bind the socket with the server address
    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    // Listen to the socket, with #MAX_CLIENT in the waiting queue
    // TODO - MAX_CLIENTS variable to initialize
    if (listen(server_fd, MAX_CLIENTS) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(server_fd, &readfds);
    int max_fd = server_fd;

    // Open the server, waiting for a client to connect
    while (1)
    {

        // Add the client socket to the set
        FD_SET(client_fd, &readfds);
        if (client_fd > max_fd)
        {
            max_fd = client_fd;
        }

        for (int i = 0; i <= max_fd; i++)
        {
            if (FD_ISSET(i, &readfds))
            {
                if (i == server_fd) // caso server
                {
                    // Take connection
                    if ((client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &addrlen)) < 0)
                    {
                        perror("accept");
                        exit(EXIT_FAILURE);
                    }

                    printf("Client connesso\n");
                    FD_SET(client_fd, &readfds);
                    max_fd = (client_fd > max_fd) ? client_fd : max_fd;
                }
                else // caso client
                {
                    char buff[BUFFER_SIZE];
                    int bytesread = read(i, buff, BUFFER_SIZE);
                    if (bytesread <= 0) {
                        if (bytesread == 0) {
                            printf("Client disconnesso\n");
                            close(i);
                            if (i == max_fd) {
                                do{
                                    max_fd--;
                                } while(!FD_ISSET(max_fd, &readfds));
                            }
                            FD_CLR(i, &readfds);
                        } else {
                            perror("read");
                            exit(EXIT_FAILURE);
                        }
                    } else {
                        printf("Il cliente ha inviato: %s\n", buff);
                        // TODO - qui il server deve mandare il messaggio di quel client a tutti gli altri client
                        for (int j = 0; j <= max_fd; j++) {
                            if (FD_ISSET(j, &readfds)) {
                                if (j != server_fd && j != i) {
                                    write(j, buff, bytesread);
                                }
                            }
                        }
                    }
                }
            }
        }
        // Data reading

        // Data writing
    }

    return 0;
}

// char *reader(int client_socket)
// {
//     int bytesread;
//     char *buffer = (char *)malloc(sizeof(char) * BUFFER_SIZE);

//     if ((bytesread = read(client_socket, buffer, BUFFER_SIZE)) < 0)
//     {
//         perror("read");
//         exit(EXIT_FAILURE);
//     }

//     return buffer;
// }

// int main()
// {
//     int server_fd, client_fd, bytesread;
//     ssize_t valread;
//     struct sockaddr_in server_addr, client_addr;
//     socklen_t client_addr_len = sizeof(client_addr);
//     char buffer[BUFFER_SIZE] = {0};

//     // Creazione socket
//     if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
//     {
//         perror("creazione socket");
//         exit(EXIT_FAILURE);
//     }
//     server_addr.sin_family = AF_INET;
//     server_addr.sin_port = htons(PORT);
//     server_addr.sin_addr.s_addr = inet_addr(IP_ADDRESS);

//     // Bind socket
//     if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
//     {
//         perror("bind");
//         exit(EXIT_FAILURE);
//     }

//     // Listen socket
//     if (listen(server_fd, MAX_CLIENTS) < 0)
//     {
//         perror("listen");
//         exit(EXIT_FAILURE);
//     }

//     // riceve la connessione
//     client_fd = accept(server_fd, NULL, NULL);

//     if (client_fd  < 0)
//     {
//         perror("accept");
//         exit(EXIT_FAILURE);
//     }
//     printf("cliente connesso\n");

//     // Legge i dati
//     char *temp = reader(client_fd);
//     printf("Il cliente ha inviato:\t%s\n", temp);

//     // Invia i dati
//     write(client_fd, buffer, bytesread);

//     // Apro il server, ascolta le connessioni
//     // while (1)
//     // {
//     // }

//     close(client_fd);
//     close(server_fd);

//     return 0;
// }