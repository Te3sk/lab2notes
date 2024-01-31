// Variante 3: implementare un server multithread con un pool di N thread worker
// (N è un parametro da linea di comando). Gli N thread ricevono i clienti da gestire
// tramite una coda condivisa

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

#define BUFFER_SIZE 100
#define QUEUE_SIZE 10

typedef struct {
    int *data;
    int head;
    int tail;
    int size;
    pthread_mutex_t lock;
    pthread_cond_t not_empty;
    pthread_cond_t not_full;
} queue_t;

void queue_init(queue_t *q) {
    q->data = malloc(sizeof(int) * QUEUE_SIZE);
    q->head = 0;
    q->tail = 0;
    q->size = 0;
    pthread_mutex_init(&(q->lock), NULL);
    pthread_cond_init(&(q->not_empty), NULL);
    pthread_cond_init(&(q->not_full), NULL);
}

void queue_destroy(queue_t *q) {
    free(q->data);
    pthread_mutex_destroy(&(q->lock));
    pthread_cond_destroy(&(q->not_empty));
    pthread_cond_destroy(&(q->not_full));
}

void queue_push(queue_t *q, int fd) {
    pthread_mutex_lock(&(q->lock));
    while ((q->size == QUEUE_SIZE)) {
        pthread_cond_wait(&(q->not_full), &(q->lock));
    }
    q->data[q->tail] = fd;
    q->tail = (q->tail + 1) % QUEUE_SIZE;
    q->size++;
    pthread_cond_signal(&(q->not_empty));
    pthread_mutex_unlock(&(q->lock));
}

int queue_pop(queue_t *q) {
    pthread_mutex_lock(&(q->lock));
    while ((q->size == QUEUE_SIZE)) {
        pthread_cond_wait(&(q->not_empty), &(q->lock));
    }
    int value = q->data[q->head];
    q->head = (q->head + 1) % QUEUE_SIZE;
    q->size--;
    pthread_cond_signal(&(q->not_full));
    pthread_mutex_unlock(&(q->lock));
    return value;
}

void* worker(void* arg) {

    queue_t q = *(queue_t *) arg;
    while (1) {
        int clientFD = queue_pop(&q);
        char buf[BUFFER_SIZE];
        read(clientFD, buf, BUFFER_SIZE);
        printf("Il cliente ha inviato %s\n", buf);
        write(clientFD, buf, strlen(buf) + 1);
        close(clientFD);
    }
    // int fd = *(int*) arg;
    // read(fd, buf, BUFFER_SIZE);
    // write(fd, buf, strlen(buf) + 1);
    // printf("Il cliente ha inviato %s\n", buf);
    // close(fd);

    return NULL;
}

int main(int argc, char* argv[]) {
    // Check if the number of arguments is valid
    if (argc < 2) {
        printf("Errore: inserire numero di thread worker\n");
        exit(EXIT_FAILURE);
    } else if (argc > 2) {
        printf("Errore: inseriti troppi parametri\n");
        exit(EXIT_FAILURE);
    }

    // Get the number of threads from the command line argument
    int N = atoi(argv[1]);
    printf("numero di thread worker: %d\n", N);

    // Create a queue to store the clients
    queue_t q;
    queue_init(&q);

    // Create a socket for the server
    int server = socket(AF_INET, SOCK_STREAM, 0);

    // Create the server address
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(2222);
    server_addr.sin_addr.s_addr = inet_addr("192.168.111.47");

    // Bind the socket to the address
    bind(server, (struct sockaddr *)&server_addr, sizeof(server_addr));
    listen(server, 10);

    // Create a set of all the sockets
    fd_set allFDs, readFDs;
    FD_ZERO(&allFDs);
    FD_SET(server, &allFDs);
    int fdMax = server;

    // Create the threads
    pthread_t thread[N];
    for (int i = 0; i < N; i++) {
        pthread_create(&thread[i], NULL, worker, &q);
    }

    // Accept clients and add them to the queue
    while(1) {
        int clientFD = accept(server, NULL, NULL);
        if (clientFD == -1) {
            perror("Errore nella accept");
            continue;
        }
        printf("Nuovo cliente connesso\n");
        queue_push(&q, clientFD);
    }

    // Join the threads
    for (int i = 0; i < N; i++) {
        pthread_join(thread[i], NULL);
    }

    // Destroy the queue
    queue_destroy(&q);
    close(server);    

    return 0;
}