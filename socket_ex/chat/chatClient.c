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
// pool di N thread worker che ricevano dei clienti da
// gestire (un thread gestisce più clienti in concomitanza)

// TODO
//      l'unico problema rimasto è che dopo che si scrive
//      qualcosa su std input bisogna battere invio due
//      volte altrimenti non va avanti nel ciclo

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <sys/select.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>

#define PORT 2222
#define IP_ADDRESS "192.168.111.47" // pax
// #define IP_ADDRESS "192.168.1.243" // home
#define BUFFER_SIZE (1024) * sizeof(char)

void clear_input();
char *read_input();

int main()
{
    // * socket creation
    int sock_fd = socket(AF_INET, SOCK_STREAM, 0);

    // * build socket address
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr(IP_ADDRESS);

    // * try to connect to the server
    int conn = 0;
    while ((conn = connect(sock_fd, (struct sockaddr *)&server_addr, sizeof(server_addr))) == -1 && errno == ENOENT)
    {
        sleep(1);
    }
    if (conn == -1)
    {
        perror("Connection failed");
        exit(EXIT_FAILURE);
    }

    // * declarate file descriptor set
    fd_set allFDs;

    printf("Inserire messaggio (exit per uscire):\n\t");

    // * main cycle
    while (1)
    {
        // * initializing fd set with socket fd and stdin fd
        FD_ZERO(&allFDs);
        FD_SET(sock_fd, &allFDs);
        FD_SET(STDIN_FILENO, &allFDs);

        // * waiting for activity in stdin or socket
        if ((select(sock_fd + 1, &allFDs, NULL, NULL, NULL) == -1))
        {
            perror("Select failed");
            exit(EXIT_FAILURE);
        }

        char *buf = (char *)malloc(BUFFER_SIZE * sizeof(char));

        if (FD_ISSET(STDIN_FILENO, &allFDs))
        {
            // * std input case

            buf = read_input();

            // TODO - temp test
            printf("send \'%s\' to the server\n", buf);

            // error handling
            if (write(sock_fd, buf, strlen(buf)) == -1)
            {
                perror("write failed");
                exit(EXIT_FAILURE);
            }

            // clear_input();

            printf("Inserire messaggio (exit per uscire):\n\t");
        }
        if (FD_ISSET(sock_fd, &allFDs))
        {
            // * socket case

            int bytesread = read(sock_fd, buf, BUFFER_SIZE);

            // * check if server disconnect
            if (bytesread <= 0)
            {
                // error handling
                if (bytesread == -1)
                {
                    perror("read failed");
                    close(sock_fd);
                    exit(EXIT_FAILURE);
                }
                else if (bytesread == 0)
                {
                    printf("Server disconnected\n");
                    close(sock_fd);
                    exit(EXIT_SUCCESS);
                }
            }

            buf[bytesread] = '\0';
            printf("Server: %s\n", buf);
        }
    }

    close(sock_fd);

    return 0;
}

void clear_input()
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF)
    {
    }
    clearerr(stdin);
}

char *read_input()
{
    char *buf = (char *)malloc(BUFFER_SIZE * sizeof(char));
    // error handling
    if (fgets(buf, BUFFER_SIZE, stdin) == NULL)
    {
        perror("fgets failed");
        exit(EXIT_FAILURE);
    }

    // * check if user want to exit
    if (strcmp(buf, "exit\n") == 0)
    {
        printf("confermi di voler uscire (y/n)\n");
        // error handling
        if (fgets(buf, BUFFER_SIZE, stdin) == NULL)
        {
            perror("fgets failed");
            exit(EXIT_FAILURE);
        }
        if (strcmp(buf, "y\n") == 0)
        {
            printf("exit...\n");
            exit(EXIT_SUCCESS);
        }
    }

    clear_input();

    if (buf[strlen(buf) - 1] == '\n')
    {
        // printf("ultimo char = \\n,\t cambiato con \\0\n");
        buf[strlen(buf) - 1] = '\0';
    } else {
        // printf("ultmimo carattere = %c,\taggiunto \\0\n", buf[strlen(buf) - 1]);
        buf[strlen(buf)] = '\0';
    }

    return buf;
}