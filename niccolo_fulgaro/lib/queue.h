#ifndef QUEUE_H
#define QUEUE_H

#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <signal.h>
#include <stdbool.h>
#include <stdatomic.h>
#include <time.h>
#include <errno.h>

#include "pars.h"
// #include "bib_ds.h"

/*
### Field
- `void *data`: *generic C pointer*, it can be cast to all pointer types: by caller: func(&variable), by function: type1 var = (type1 *)var
- `struct n *next`: point to the next item in te queue
*/
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
    pthread_cond_t notEmpty;
} Queue;

// Initialize the queue
void queue_init(Queue *q);

// Destroy the queue
void queue_destroy(Queue *q);

// Push an element to the queue
void queue_push(void *data, Queue *q);

// Pop an element from the queue
void *queue_pop(Queue *q);

// TODO - desc
int queue_is_empty(Queue *q);

#endif