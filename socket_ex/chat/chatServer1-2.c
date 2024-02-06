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

typedef struct
{
    int fd;
    bool send;
} Client;

int main()
{
    // Socket creation
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1)
    {
        perror("socket creation");
        exit(EXIT_FAILURE);
    }

    // TODO - temp test
    printf("socket creata\n");

    // building data structure for socket file descriptors
    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = inet_addr(IP_ADDRESS);

    // socket bind
    if (bind(server_fd, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1)
    {
        perror("bind ok");
        exit(EXIT_FAILURE);
    }

    // TODO - temp test
    printf("bind fatto\n");

    // socket listen
    if (listen(server_fd, MAX_CLIENTS) == -1)
    {
        perror("listen ok");
        exit(EXIT_FAILURE);
    }

    // TODO - temp test
    printf("listen ok\n");

    // building file descriptor set
    fd_set allFDs, readfds;
    FD_ZERO(&allFDs);
    FD_SET(server_fd, &allFDs);
    int fdMax = server_fd;

    // TODO - temp test
    printf("set ok\n");

    // initializing client data structure
    Client clients[MAX_CLIENTS];
    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        clients[i].fd = -1;
        clients[i].send = false;
    }

    // main cycle of program
    while (1)
    {
        // TODO - temp test
        printf("\n\tstart while\n");
        // file descriptor set copy
        readfds = allFDs;

        // waiting for an event in the set of descriptors
        if (select(fdMax + 1, &readfds, NULL, NULL, NULL) == -1)
        {
            perror("select");
            exit(EXIT_FAILURE);
        }

        // TODO - temp test
        printf("\tselect ok\n");

        // cycle on file descriptors
        for (int i = 0; i <= fdMax; i++)
        {
            // TODO - temp test
            printf("\tfor ite (%d of %d)", i, fdMax);
            // check if fd is active
            if (FD_ISSET(i, &readfds))
            {
                // server case
                if (i == server_fd)
                {
                    // TODO - temp test
                    printf("\n\t\tserver case\n");
                    // accept new client
                    int client_fd = accept(server_fd, NULL, NULL);
                    if (client_fd == -1)
                    {
                        perror("accept failed");
                        exit(EXIT_FAILURE);
                    }

                    // TODO - temp test
                    printf("\t\taccept ok - clientFD : %d\n", client_fd);

                    // simil handshake - send msg to client with the server identifier for it
                    char msg[BUFFER_SIZE];
                    sprintf(msg, "%d", client_fd);
                    // TODO - temp test
                    printf("msg:\t%s\0", msg);
                    write(client_fd, msg, sizeof(char)*strlen(msg));

                    // add new client to file descriptor set
                    FD_SET(client_fd, &allFDs);
                    if (client_fd > fdMax)
                    {
                        // TODO - temp test
                        printf("\t\t\tfd max update from %d to %d\n", fdMax, client_fd);
                        fdMax = client_fd;
                    }
                }
                else
                { // client case
                    // TODO - temp test
                    printf("\n\t\tclient case\n");
                    char buf[BUFFER_SIZE];
                    int bytesRead = read(i, buf, BUFFER_SIZE);
                    if (bytesRead <= 0)
                    {
                        if (bytesRead == 0)
                        {
                            printf("client close the connection\n");
                        }
                        else if (bytesRead == -1)
                        {
                            perror("read failed");
                        }
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
                        // TODO - temp test
                        printf("\tletta stringa di lunghezza:\t%d\n", strlen(buf));
                        // TODO - temp test
                        // TODO - temp test
                        printf("\tscritta stringa di lunghezza:\t%d\n", strlen(buf));
                        for (int k = 0; k < strlen(buf); k++)
                        {
                            printf("\t%c", buf[k]);
                        }
                        printf("\n");
                        for (int k = 0; k < strlen(buf); k++)
                        {
                            printf("\t%d", k + 1);
                        }
                        printf("\n");

                        buf[bytesRead <= (BUFFER_SIZE - 1) ? bytesRead : BUFFER_SIZE - 1] = '\0';
                        char *bufcpy = strdup(buf);
                        // separate client id from msg
                        char *token = strtok(buf, "_");
                        int cid = atoi(token);
                        token = strtok(NULL, "_");
                        char *msg = token;
                        // ! problem with token pointer in this free
                        // free(token);

                        printf("client n %d sended: %s\n\tbuf = %s\n\tbufcpy = %s", cid, msg, buf, bufcpy);
                        // TODO - send it to other clients
                        for (int j = 0; j <= fdMax; j++)
                        {
                            // TODO - temp test
                            printf("\t\tfd:%d\n", j);
                            if (!FD_ISSET(j, &allFDs))
                            {
                                printf("\t\t-\tnon e' un fd\n");
                                continue;
                            }
                            else if (j == server_fd)
                            {
                                printf("\t\t-\te' il server\n");
                                continue;
                            }
                            else if (j == i)
                            {
                                printf("\t\t-\te' il client che ha inviato\n");
                                continue;
                            }
                            else
                            // * if (FD_ISSET(j, &allFDs) && j != server_fd)
                            {
                                // TODO - temp test
                                printf("\tsend ' %s ' to fd n: %d\n", bufcpy, j);
                                if (write(j, bufcpy, (bytesRead + strlen("0_"))) == -1)
                                {
                                    perror("write failed");
                                    exit(EXIT_FAILURE);
                                }
                            }
                        }
                    }
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
                // TODO - temp test
                printf("fd in the set:\n");
                for (int j = 0; j <= fdMax; j++)
                {
                    if (FD_ISSET(j, &allFDs))
                    {
                        // TODO - temp test
                        if (j == server_fd)
                        {
                            printf("\t- server: %d\n", j);
                        }
                        else
                        {
                            printf("\t- client: %d\n", j);
                        }
                    }
                }
            }
        }
    }
    close(server_fd);

    return 0;
}