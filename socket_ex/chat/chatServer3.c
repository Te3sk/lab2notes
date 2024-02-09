// Implementare una chat dove il server inoltra i messaggi
// dei clienti a tutti gli altri clienti connessi in quel
// momento. I clienti si connettono e mandano messaggi
// multipli presi dall'utente da tastiera

// Variante 3: implementare un server multi thread con un
// pool di N thread worker che ricevono dei clienti da
// gestire (un thread gestisce più clienti in concomitanza)

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/select.h>
#include <arpa/inet.h>

#define QUEUE_SIZE 10
#define MAX_CLIENTS 10
#define PORT 2222
#define IP_ADDRESS "192.168.111.47" // pax
// #define IP_ADDRESS "192.168.1.243" // home
#define BUFFER_SIZE (1024 * sizeof(char))

typedef struct
{
    int *data;
    int head;
    int tail;
    int size;
    pthread_mutex_t lock;
    pthread_cond_t not_empty;
    pthread_cond_t not_full;
} queue_t;

typedef struct WorkerArgs
{
    queue_t *queue;
    int server_fd;
    int *fdMax;
    fd_set *allFDs;

} WorkerArgs;

void queue_init(queue_t *);
void queue_destroy(queue_t *);
void queue_push(queue_t *, int);
int queue_pop(queue_t *);
void *worker(void *);

// TODO - testing func
void temp_fd_pres(fd_set, int, int);

int main(int argc, char *argv[])
{
    // * check input arguments
    if (argc < 2)
    {
        printf("Errore: inserire numero di thread worker\n");
        exit(EXIT_FAILURE);
    }
    else if (argc > 2)
    {
        printf("Errore: inseriti troppi argomenti\n");
        exit(EXIT_FAILURE);
    }

    // * get number of thread worker
    int num_threads = atoi(argv[1]);

    // * build the queue
    queue_t queue;
    queue_init(&queue);


    // * create the socket
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);

    // * build data structure for server fd
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr(IP_ADDRESS);


    // * bind the socket to the address
    // error handling
    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }


    // error handling
    if (listen(server_fd, MAX_CLIENTS))
    {
        perror("listen failed");
        exit(EXIT_FAILURE);
    }

    // * build the set of fd
    fd_set allFDs, readFDs;
    FD_ZERO(&allFDs);
    FD_SET(server_fd, &allFDs);
    int fdMax = server_fd;

    // *
    struct WorkerArgs *args = (struct WorkerArgs *)malloc(sizeof(struct WorkerArgs));
    args->server_fd = server_fd;
    args->fdMax = &fdMax;
    args->allFDs = &allFDs;
    args->queue = &queue;

    // * create the threads
    pthread_t tid[num_threads];
    for (int i = 0; i < num_threads; i++)
    {
        pthread_create(&tid[i], NULL, worker, (void *)args);
    }

    // * main loop
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
        queue_push(&queue, clientFD);
    }

    return 0;
}

void temp_fd_pres(fd_set allFDs, int fdMax, int server_fd)
{
    printf("\tnumber of present fd: %d\n", fdMax);
    for (int i = 0; i <= fdMax; i++)
    {
        if (FD_ISSET(i, &allFDs))
        {
            printf("\t\t- %d", i);
            if (i == server_fd)
            {
                printf(" server\n");
            }
            else
            {
                printf(" client\n");
            }
        }
    }
}

void *worker(void *arg)
{
    int serverFD = ((WorkerArgs *)arg)->server_fd;
    int *fdMax = ((WorkerArgs *)arg)->fdMax;
    fd_set *allFDs = ((WorkerArgs *)arg)->allFDs;
    queue_t *queue = ((WorkerArgs *)arg)->queue;
    int clientFD = queue_pop(queue);
    pthread_mutex_t lock = queue->lock;
    pthread_cond_t not_empty = queue->not_empty;
    pthread_cond_t not_full = queue->not_full;


    char buffer[BUFFER_SIZE];

    while (1)
    {
        int bytesread = read(clientFD, buffer, BUFFER_SIZE);
        if (bytesread < 0)
        {
            pthread_mutex_lock(&lock);
            // error handling
            if (bytesread == -1)
            {
                perror("read failed");
                exit(EXIT_FAILURE);
            }
            else if (bytesread == 0)
            {
                printf("Client disconnected\n");
            }

            close(clientFD);
            FD_CLR(clientFD, allFDs);
            if (clientFD == *fdMax)
            {
                do
                {
                    (*fdMax)--;
                } while (!FD_ISSET((*fdMax), allFDs));
            }
            pthread_mutex_unlock(&lock);
            break;
        }
        else
        {
            buffer[bytesread] = '\0';
            pthread_mutex_lock(&lock);

            // * send msg to other clients
            for (int i = 0; i < (*fdMax) + 1; i++)
            {
                if (FD_ISSET(i, allFDs) && i != clientFD && i != serverFD)
                {
                    // error handling
                    if (write(i, buffer, bytesread) == -1)
                    {
                        perror("write failed");
                        exit(EXIT_FAILURE);
                    }
                }
            }
            pthread_mutex_unlock(&lock);
        }
    }
    return NULL;
}

void queue_init(queue_t *q)
{
    q->data = malloc(QUEUE_SIZE * sizeof(int));
    q->head = 0;
    q->tail = 0;
    q->size = 0;
    pthread_mutex_init(&(q->lock), NULL);
    pthread_cond_init(&(q->not_empty), NULL);
    pthread_cond_init(&(q->not_full), NULL);
}

void queue_destroy(queue_t *q)
{
    free(q->data);
    pthread_mutex_destroy(&(q->lock));
    pthread_cond_destroy(&(q->not_empty));
    pthread_cond_destroy(&(q->not_full));
}

void queue_push(queue_t *q, int fd)
{
    pthread_mutex_lock(&(q->lock));
    while (q->size == QUEUE_SIZE)
    {
        pthread_cond_wait(&(q->not_full), &(q->lock));
    }
    q->data[q->tail] = fd;
    q->tail = (q->tail + 1) % QUEUE_SIZE;
    q->size++;
    pthread_cond_signal(&(q->not_empty));
    pthread_mutex_unlock(&(q->lock));
}

int queue_pop(queue_t *q)
{
    pthread_mutex_lock(&(q->lock));
    while (q->size == 0)
    {
        pthread_cond_wait(&(q->not_empty), &(q->lock));
    }
    int value = q->data[q->head];
    q->head = (q->head + 1) % QUEUE_SIZE;
    q->size--;
    pthread_cond_signal(&(q->not_full));
    pthread_mutex_unlock(&(q->lock));
    return value;
}