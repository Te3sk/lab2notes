// Implementare una chat dove il server inoltra i messaggi
// dei clienti a tutti gli altri clienti connessi in quel
// momento. I clienti si connettono e mandano messaggi
// multipli presi dall'utente da tastiera

// Variante 1: implementare il server single thread

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <errno.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>

#define IP_ADDRESS "192.168.111.47"
#define PORT 2222
#define MAX_CLIENTS 100
#define BUFFER_SIZE (1024 * sizeof(char))

int main()
{
    // * socket creation
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    // error handling
    if (server_fd == -1)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // * build ds for socket fd
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);
    server_address.sin_addr.s_addr = inet_addr(IP_ADDRESS);

    // * bind socket
    // error handling
    if (bind(server_fd, (struct sockaddr *)&server_address, sizeof(server_address)) == -1)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // * socket listen
    // error handling
    if (listen(server_fd, MAX_CLIENTS) == -1)
    {
        perror("listen failed");
        exit(EXIT_FAILURE);
    }

    // * build fd set
    fd_set allFDs, readfds;
    FD_ZERO(&allFDs);
    FD_SET(server_fd, &allFDs);
    int fdMax = server_fd;

    // * main cycle
    while (1)
    {
        // * fd set backup
        readfds = allFDs;

        // * select - wait for activiy
        // error handling
        if (select(fdMax + 1, &readfds, NULL, NULL, NULL) == -1)
        {
            perror("select failed");
            exit(EXIT_FAILURE);
        }

        // * cycle on FDs
        for (int i = 0; i <= fdMax; i++)
        {
            if (FD_ISSET(i, &readfds))
            {
                // * server case
                if (i == server_fd)
                {
                    // * accept new client
                    int client_fd = accept(server_fd, NULL, NULL);
                    // error handling
                    if (client_fd == -1)
                    {
                        perror("accept failed");
                        exit(EXIT_FAILURE);
                    }

                    // * add client to set and update fdMax
                    FD_SET(client_fd, &allFDs);
                    if (client_fd > fdMax)
                    {
                        fdMax = client_fd;
                    }
                }
                else
                {
                    // * client case
                    char *buf = (char *)malloc(BUFFER_SIZE * sizeof(char));
                    // * read from client
                    int bytesread = read(i, buf, BUFFER_SIZE);
                    if (bytesread <= 0)
                    {
                        if (bytesread == 0)
                        {
                            printf("\tClient disconnected (fd: %d)\n", i);
                        }
                        else if (bytesread == -1)
                        {
                            // error handling
                            perror("read failed");
                            exit(EXIT_FAILURE);
                        }
                        // * remove client from the set and update fdMax
                        close(i);
                        FD_CLR(i, &allFDs);
                        if (i == fdMax)
                        {
                            do
                            {
                                fdMax--;
                            } while (!FD_ISSET(fdMax, &allFDs));
                        }
                    }
                    else
                    {
                        printf("\t(sender fd: %d) Read - %s\n", i, buf);

                        // * insert end-char in the string
                        buf[bytesread] = '\0';

                        // * send msg to the other client
                        for (int j = 0; j <= fdMax; j++)
                        {
                            if (FD_ISSET(j, &allFDs) && j != i && j != server_fd)
                            {
                                // doesn't send msg to server and to sender client
                                // error handling
                                if (write(j, buf, bytesread) == -1)
                                {
                                    perror("write failed");
                                    exit(EXIT_FAILURE);
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    close(server_fd);

    return 0;
}
