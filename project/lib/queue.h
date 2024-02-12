#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <signal.h>

// Define a node
typedef struct n
{
    void *data;
    struct n *next;
} Node;

// Define a queue
typedef struct
{
    // Head and tail of the queue
    Node *head;
    Node *tail;
    // Lock for the queue
    pthread_mutex_t mutex;
    // Condition variable for the queue
    pthread_cond_t cond;

} Queue;

// Initialize the queue
void init(Queue *q);

// Destroy the queue
void destroy(Queue *q);

// Push an element to the queue
void push(void *data, Queue *q);

// Pop an element from the queue
void *pop(Queue *q);