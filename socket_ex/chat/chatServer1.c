// Implementare una chat dove il server inoltra i messaggi
// dei clienti a tutti gli altri clienti connessi in quel
// momento. I clienti si connettono e mandano messaggi
// multipli presi dall'utente da tastiera

// Variante 1: implementare il server single thread

// TODO - problemi nell'uso della socket condivisa
// ! il server blocca il ciclo rispetto al primo client
// ! che si connette: riceve la connessione e i messaggi
// ! degli altri ma li stampa solo dopo un'istruzione proveniente dal primo client

// TODO - aggiungere segnale di terminazione per il client

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

// TODO - tempo testing functions
void tempSetSize (fd_set *set, int max_fd) {
    int count = 0;
    for (int i = 0; i < max_fd; i++) {
        if (FD_ISSET(i, set)) {
            count++;
        }
    }
    printf("Number of clients: %d\n", count);
    return;
}

void tempFunc()
{
    printf("entrato in tempfunc\n");
}

int main()
{
    int server_fd, client_fd, bytesread;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addrlen = sizeof(client_addr);
    char buffer[BUFFER_SIZE];

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    // TODO - temp
    printf("\tserver_fd = %d\n", server_fd);

    // AF_INET to run both client and server on the same machine
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr(IP_ADDRESS);

    // Bind the socket with the server address
    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    // TODO - temp
    printf("\t(binding)server_fd = %d\n", server_fd);

    // Listen to the socket, with #MAX_CLIENT in the waiting queue
    if (listen(server_fd, MAX_CLIENTS) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    // TODO - temp
    printf("\t(listening)server_fd = %d\n", server_fd);

    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(server_fd, &readfds);
    int max_fd = server_fd;

    // TODO - temp
    printf("\t(fd set creation)server_fd = %d\n", server_fd);

    // Open the server, waiting for a client to connect
    while (1)
    {
        // TODO - temp test
        printf("\t\tinizio while\n");
        // TODO - temp test
        tempSetSize(&readfds, max_fd);

        if (select(max_fd + 1, &readfds, NULL, NULL, NULL) == -1)
        {
            perror("select(tempfds)");
            exit(EXIT_FAILURE);
        }

        for (int i = 0; i <= max_fd; i++)
        {
            // TODO - temp test
            printf("\t\tinizio for (%d of %d)\n", i, max_fd);
            if (FD_ISSET(i, &readfds)) // chek if file descriptor "i" is in the set "readfds"
            {
                if (i == server_fd) // caso server
                {
                    // TODO - temp test
                    printf("\n\tentrato nel caso server (fd: %d)\t-\t", i);
                    // Take connection
                    if ((client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &addrlen)) < 0)
                    {
                        perror("accept");
                        exit(EXIT_FAILURE);
                    }

                    // TODO - temp test
                    printf("Client connesso, client_fd = %d, max_fd = %d\n", client_fd, max_fd);
                    FD_SET(client_fd, &readfds);
                    if (client_fd > max_fd)
                    {
                        printf("\t\tcambiato max_fd  %d -> a -> %d\n", max_fd, client_fd);
                        max_fd = client_fd;
                    }
                    max_fd = (client_fd > max_fd) ? client_fd : max_fd;
                }
                else // caso client
                {
                    // TODO - temp test
                    printf("\n\tentrato nel caso client (fd: %d)\t-\t", i);
                    char buff[BUFFER_SIZE];
                    int bytesread = read(i, buff, BUFFER_SIZE);
                    if (bytesread <= 0)
                    {
                        if (bytesread == 0)
                        {
                            printf("Client disconnesso\n");
                            close(i);
                            if (i == max_fd)
                            {
                                do
                                {
                                    max_fd--;
                                } while (!FD_ISSET(max_fd, &readfds));
                            }
                            FD_CLR(i, &readfds);
                        }
                        else
                        {
                            perror("read");
                            exit(EXIT_FAILURE);
                        }
                    }
                    else
                    {
                        printf("Il cliente ha inviato: %s\n", buff);
                        tempFunc();
                        // TODO - qui il server deve mandare il messaggio di quel client a tutti gli altri client
                        for (int j = 0; j <= max_fd; j++)
                        {
                            if (FD_ISSET(j, &readfds))
                            {
                                if (j != server_fd && j != i)
                                {
                                    write(j, buff, bytesread);
                                }
                            }
                        }
                        // TODO - temp test
                        printf("uscito dal for di scrittura\n");
                    }
                }
            }
        }
    }

    return 0;
}
