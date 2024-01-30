#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define QUEUE_SIZE 100

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
    pthread_mutex_init(&q->lock, NULL);
    pthread_cond_init(&q->not_empty, NULL);
    pthread_cond_init(&q->not_full, NULL);
}

void queue_destroy(queue_t *q) {
    free(q->data);
    pthread_mutex_destroy(&q->lock);
    pthread_cond_destroy(&q->not_empty);
    pthread_cond_destroy(&q->not_full);
}

void queue_push(queue_t *q, int value) {
    pthread_mutex_lock(&q->lock);
    while (q->size == QUEUE_SIZE) {
        pthread_cond_wait(&q->not_full, &q->lock);
    }
    q->data[q->tail] = value;
    q->tail = (q->tail + 1) % QUEUE_SIZE;
    q->size++;
    pthread_cond_signal(&q->not_empty);
    pthread_mutex_unlock(&q->lock);
}

int queue_pop(queue_t *q) {
    pthread_mutex_lock(&q->lock);
    while (q->size == 0) {
        pthread_cond_wait(&q->not_empty, &q->lock);
    }
    int value = q->data[q->head];
    q->head = (q->head + 1) % QUEUE_SIZE;
    q->size--;
    pthread_cond_signal(&q->not_full);
    pthread_mutex_unlock(&q->lock);
    return value;
}

void *worker(void *arg) {
    queue_t *q = arg;
    while (1) {
        int value = queue_pop(q);
        printf("Worker %lu got value %d\n", pthread_self(), value);
    }
    return NULL;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s N\n", argv[0]);
        return 1;
    }

    int n = atoi(argv[1]);
    queue_t q;
    queue_init(&q);

    pthread_t threads[n];
    for (int i = 0; i < n; i++) {
        pthread_create(&threads[i], NULL, worker, &q);
    }

    for (int i = 0; i < 1000; i++) {
        queue_push(&q, i);
    }

    for (int i = 0; i < n; i++) {
        pthread_join(threads[i], NULL);
    }

    queue_destroy(&q);
    return 0;
}