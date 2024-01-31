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

#define N 100
#define PORT 2222

int main()
{
    // Creare un socket TCP
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);

    // Inserire indirizzo IP e porta
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr("192.168.111.47");

    fd_set readfds; // insieme di file descriptor

    // Connessione al server
    int conn = 0;
    while ((conn = connect(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr))) == -1 && errno == ENOENT)
    {
        sleep(1);
    }

    if (conn == -1)
    {
        perror("Connection error");
        exit(EXIT_FAILURE);
    }
    else
    {
        printf("Connessione riuscita\n");
    }

    char buff[N];

    // while(1)
    while (1)
    {
        // Inserire messaggio
        printf("Inserire messaggio ('exit' per uscire):\n");
        // scanf("%s", buff);


        // Inizializzare l'insieme di file descriptor
        FD_ZERO(&readfds);
        FD_SET(server_fd, &readfds);
        FD_SET(STDIN_FILENO, &readfds);

        // attendere la presenza di dati da parte del server
        if (select(server_fd + 1, &readfds, NULL, NULL, NULL) < 0)
        {
            perror("select");
            exit(EXIT_FAILURE);
        }

        // Ricevere dati dallo server
        if (FD_ISSET(server_fd, &readfds))
        {
            read(server_fd, buff, N);
            printf("Server ha inviato:\t%s\n", buff);
        }

        // Ricevere dati dall'utente
        if (FD_ISSET(STDIN_FILENO, &readfds))
        {
            scanf("%s", buff);
            // Controllare se l'utente vuole uscire
            if (strcmp(buff, "exit") == 0)
            {
                printf("Sicuro di voler uscire? (y/n)\n");
                scanf("%s", buff);
                if (strcmp(buff, "y") == 0)
                {
                    break;
                }
            }
            write(server_fd, buff, strlen(buff));
            printf("\tMessaggio inviato\n");
        }
    }

    // chiudere la connessione
    close(server_fd);

    return 0;
}