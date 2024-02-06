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
#define IP_ADDRESS "192.168.111.47"
#define BUFFER_SIZE (1024) * sizeof(char)

char *check_msg(int, char *);

void clear_input();

int main()
{
    // socket creation
    int sock_fd = socket(AF_INET, SOCK_STREAM, 0);

    // TODO - temp test
    printf("socket id: %d\n", sock_fd);

    // build socket address
    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = inet_addr(IP_ADDRESS);

    // try to connect to the server
    int conn = 0;
    while ((conn = connect(sock_fd, (struct sockaddr *)&serverAddr, sizeof(serverAddr))) == -1 && errno == ENOENT)
    {
        sleep(1);
    }
    // if connection failed
    if (conn == -1)
    {
        perror("Connection failed");
        exit(EXIT_FAILURE);
    }

    // simil handshake for client identifier
    char *temp = (char *)malloc(sizeof(char) * BUFFER_SIZE);
    read(sock_fd, temp, sizeof(1024));
    int myID = atoi(temp);
    free(temp);
    // TODO - temp test
    printf("connessione riuscita\t|\tClient ID: %d\n", myID);

    // TODO - temp test
    printf("socket: %d\t\t|\tstdin_fileno: %d\n", (sock_fd), (STDIN_FILENO));

    // build fd set
    fd_set allFDs;

    printf("Inserire messaggio ('exit' per uscire):\n\t");

    while (1)
    {
        // initailizing fd_set with socket fd and std input fd
        FD_ZERO(&allFDs);
        FD_SET(sock_fd, &allFDs);
        FD_SET(STDIN_FILENO, &allFDs);

        // waiting for activity in one between socket and std input
        if ((select(sock_fd + 1, &allFDs, NULL, NULL, NULL)) == -1)
        {
            perror("select()");
            exit(EXIT_FAILURE);
        }

        // TODO - temp test
        printf("\tselect ok\n");

        char buf[BUFFER_SIZE];

        if (FD_ISSET(STDIN_FILENO, &allFDs))
        {
            // std input case
            // TODO - temp test
            printf("\tSTD INPUT CASE\n");

            if (fgets(buf, BUFFER_SIZE, stdin) == NULL)
            {
                perror("fgets()");
                exit(EXIT_FAILURE);
            }
            // TODO - temp test
            printf("\t\tfgets ok\n");

            if (strcmp(buf, "exit\n") == 0)
            {
                printf("confermi di voler uscire? (y/n)");
                if (fgets(buf, BUFFER_SIZE, stdin) == NULL)
                {
                    perror("fgets()");
                    exit(EXIT_FAILURE);
                }
                if (strcmp(buf, "y\n") == 0)
                {
                    break;
                }
                // TODO - temp test
                printf("\t\tok, continuo\n");
            }

            // build final string (to send) with own identifier
            char fin_msg[BUFFER_SIZE];
            sprintf(fin_msg, "%d_%s\0", myID, buf);
            // TODO - temp test
            printf("fin_msg:\t%s\n", fin_msg);

            // send to server
            if (write(sock_fd, fin_msg, strlen(fin_msg)) == -1)
            {
                perror("write()");
                exit(EXIT_FAILURE);
            }

            clear_input();

            check_msg(myID, fin_msg);
            printf("\n\t\twrite ok\n");

            printf("Inserire messaggio ('exit' per uscire):\n\t");
        }
        else if (FD_ISSET(sock_fd, &allFDs))
        {
            // socket case
            // TODO - temp test
            printf("\tSOCKET CASE\n");
            printf("\tmessaggio dal server\n");

            // read server msg
            int bytesread = read(sock_fd, buf, BUFFER_SIZE);
            if (bytesread == 0)
            {
                printf("\t\tserver disconesso\n");
            }
            else if (bytesread == -1)
            {
                perror("read()");
                exit(EXIT_FAILURE);
            }

            char *temp = check_msg(myID, buf);
            if (temp == NULL)
            {
                printf("-----------\n\tsono io\n-----------\n");
            }
            else
            {
                printf("-----------\n\nmsg =\t%s\n-----------\n", temp);
            }

            printf("server ha inviato:\t%s\n", buf);
            printf("Inserire messaggio ('exit' per uscire):\n\t");
        }
    }
    // close socket
    close(sock_fd);

    return 0;
}

char *check_msg(int myID, char *buf)
{
    // riceve una stringa della forma 'x_stringa', dove x è un intero e stringa è il messaggio
    char *fin_msg;

    printf("\tCHECK_MSG():\n");

    if (!(buf[1] == '_'))
    {
        printf("MALE");
        return NULL;
    }

    // TODO - temp test
    printf("\tletta stringa di lunghezza:\t%d\n", strlen(buf));
    for (int k = 0; k < strlen(buf); k++)
    {
        printf("\t%c", buf[k]);
    }
    for (int k = 0; k < strlen(buf); k++)
    {
        printf("\t%d", k + 1);
    }

    printf("\n");

    char *token = strtok(buf, "_");
    int senderID = atoi(token);
    // TODO - temp test
    printf("prima parte:\t%d\n", senderID);
    token = strtok(NULL, "_");
    char *msg;
    strcpy(msg, token);
    // TODO - temp test
    printf("seconda parte:\t%s\n", msg);

    if (senderID == myID)
    {
        return NULL;
    }
    else
    {
        return msg;
    }
}

void clear_input()
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF)
    {
    }
}