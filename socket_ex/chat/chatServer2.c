// Implementare una chat dove il server inoltra i messaggi
// dei clienti a tutti gli altri clienti connessi in quel
// momento. I clienti si connettono e mandano messaggi
// multipli presi dall'utente da tastiera

// Variante 2: implementare il server multi thread dove
// all'arrivo di ogni cliente viene creato un thread
// worker nuovo che gestisce solo quel cliente

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <pthread.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>

#define IP_ADDRESS "192.168.111.47" // pax
// #define IP_ADDRESS "192.168.1.243" // home
#define PORT 2222
#define MAX_CLIENTS 100
#define BUFFER_SIZE (1024 * sizeof(char))

typedef struct WorkerArgs
{
    int clientFD;
    int server_fd;
    fd_set *allFDs;
    int *fdMax;
    pthread_mutex_t *lock;
} WorkerArgs;

void *worker(void *);

int main()
{
    // * Creazione del socket
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);

    // * build data structure for socket fd
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr(IP_ADDRESS);

    // * bind socket
    // error handling
    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
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
    fd_set allFDs, readFDs;
    FD_ZERO(&allFDs);
    FD_SET(server_fd, &allFDs);
    int fdMax = server_fd;

    pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

    // * accept loop
    while (1)
    {
        int clientFD = accept(server_fd, NULL, NULL);
        // error handling
        if (clientFD == -1)
        {
            perror("accept failed");
            exit(EXIT_FAILURE);
        }

        FD_SET(clientFD, &allFDs);
        if (clientFD > fdMax)
        {
            fdMax = clientFD;
        }

        pthread_t tid;
        struct WorkerArgs *args = (struct WorkerArgs *)malloc(sizeof(WorkerArgs));
        args->clientFD = clientFD;
        args->fdMax = &fdMax;
        args->allFDs = &allFDs;
        args->lock = &lock;
        args->server_fd = server_fd;

        // error handling
        if (pthread_create(&tid, NULL, worker, (void *)args) != 0)
        {
            perror("pthread_create failed");
            exit(EXIT_FAILURE);
        }
    }

    return 0;
}

void *worker(void *arg)
{
    int clientFD = ((WorkerArgs *)arg)->clientFD;
    int *fdMax = ((WorkerArgs *)arg)->fdMax;
    fd_set *allFDs = ((WorkerArgs *)arg)->allFDs;
    pthread_mutex_t *lock = ((WorkerArgs *)arg)->lock;
    int server_fd = ((WorkerArgs *)arg)->server_fd;

    char buf[BUFFER_SIZE];
    int bytesread;

    // * main cycle
    while (1)
    {
        bytesread = read(clientFD, buf, BUFFER_SIZE);
        if (bytesread <= 0)
        {
            pthread_mutex_lock(lock);
            // error handling
            if (bytesread == -1)
            {
                perror("read failed");
            }
            else if (bytesread == 0)
            {
                printf("client disconnect\n");
            }

            close(clientFD);
            FD_CLR(clientFD, allFDs);
            if (clientFD == *fdMax)
            {
                do
                {
                    (*fdMax)--;
                } while (!FD_ISSET(*fdMax, allFDs));
            }
            pthread_mutex_unlock(lock);
            break;
        }
        else
        {
            buf[bytesread] = '\0';

            pthread_mutex_lock(lock);
            // * send msg to other clients

            for (int i = 0; i <= *fdMax; i++)
            {
                if (FD_ISSET(i, allFDs) && i != server_fd && i != clientFD)
                {
                    // error handling
                    if (write(i, buf, bytesread) == -1)
                    {
                        perror("write failed");
                        exit(EXIT_FAILURE);
                    }
                }
            }
            pthread_mutex_unlock(lock);
        }
    }

    return NULL;
}