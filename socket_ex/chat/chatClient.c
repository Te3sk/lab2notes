// Implementare una chat dove il server inoltra i messaggi
// dei clienti a tutti gli altri clienti connessi in quel
// momento. I clienti si connettono e mandano messaggi
// multipli presi dall'utente da tastiera

// Variante 1: implementare il server single thread

// Variante 2: implementare il server multi thread dove
// all'arrivo di ogni cliente viene creato un thread
// worker nuovo che gestisce solo quel cliente

// Variante 3: implementare un server multi thread con un
// pool di N thread worker che ricevono dei clienti da
// pool di N thread worker che ricevano dei clienti da
// pool di N thread worker che ricevano dei clienti da
// gestire (un thread gestisce più clienti in concomitanza)

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <sys/select.h>

#define BUFFER_SIZE 100
#define IP_ADDRESS "192.168.111.47"
#define PORT 2222

int main()
{
    // Socket creating - AF_INET -> IPv4
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);

    // Server address initialization
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr(IP_ADDRESS);

    // Connect to the server
    if (connect(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("connect");
        exit(EXIT_FAILURE);
    }
    else
    {
        printf("Connected to the server\n");
    }

    // fd set initialization
    fd_set readfds;
    FD_ZERO(&readfds);

    printf("Inserire messaggio (exit per uscire):\n");
    while (1)
    {
        // Add the server socket to the set
        FD_SET(server_fd, &readfds);
        // Add the user input to the set
        FD_SET(STDIN_FILENO, &readfds);

        // Wait for activity on any of the sockets in the set
        select(server_fd + 1, &readfds, NULL, NULL, NULL);
        // Data reading
        char buff[BUFFER_SIZE];

        if (FD_ISSET(STDIN_FILENO, &readfds) != 0) // Case: user wrote on std input
        {
            // Read user input
            if (fgets(buff, BUFFER_SIZE, stdin) == NULL)
            {
                perror("Errore nella lettura dell'input");
                exit(EXIT_FAILURE);
            }
            // Check if user want to exit
            if (strcmp(buff, "exit\n") == 0)
            {
                printf("Confermi di voler uscire?(y/n)\n");
                if (fgets(buff, BUFFER_SIZE, stdin) == NULL)
                {
                    perror("Errore nella lettura dell'input");
                    exit(EXIT_FAILURE);
                }
                if (buff[0] == 'y')
                {
                    break;
                }
            }
            else
            {
                // Sending message
                write(server_fd, buff, strlen(buff));
                printf("Inserire messaggio (exit per uscire):\n");
            }
        }
        else if (FD_ISSET(server_fd, &readfds) != 0) // Case: server send message on socket
        {
            printf("messaggio dal server\n");
            // Recive answere
            int bytesread = read(server_fd, buff, BUFFER_SIZE);
            if (bytesread < 0) {
                perror("read");
                exit(EXIT_FAILURE);
            }
            printf("Server ha inviato:\t%s\n", buff);
            printf("Inserire messaggio (exit per uscire):\n");
        }
    }
    // Closing socket
    close(server_fd);

    return 0;
}