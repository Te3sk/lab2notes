#include "queue.h"

void init(Queue *q)
{
    // initialize the queue
    q->head = q->tail = NULL;
    // initialize mutex and condition variable
    pthread_mutex_init(&q->mutex, NULL);
    pthread_cond_init(&q->cond, NULL);
}

void destroy(Queue *q)
{
    // destroy the mutex and condition variable
    pthread_mutex_destroy(&q->mutex);
    pthread_cond_destroy(&q->cond);
    // empty the queue
    while (q->head)
    {
        Node *temp = q->head;
        q->head = q->head->next;
    }
    q->tail = NULL;
}

void push(void *data, Queue *q)
{
    // create a new node
    Node *n = (Node *)malloc(sizeof(Node));
    n->data = data;
    n->next = NULL;

    // lock the mutex
    pthread_mutex_lock(&q->mutex);
    // check if the queue is empty
    if (q->tail == NULL)
    {
        // if empty, assign the new node to head
        q->head = n;
    }
    else
    {
        // if not empty, assign the new node to the tail's next
        q->tail->next = n;
    }
    // assign the new node to the tail
    q->tail = n;

    // signal the condition variable
    pthread_cond_signal(&q->cond);
    // unlock the mutex
    pthread_mutex_unlock(&q->mutex);
}

void *pop(Queue *q)
{

    // lock the mutex
    pthread_mutex_lock(&q->mutex);
    // check if the queue is empty
    while (q->head == NULL)
        pthread_cond_wait(&q->cond, &q->mutex);

    // store the head node
    Node *n = q->head;

    // assign the head's next to the head
    q->head = q->head->next;
    // check if the queue is empty
    if (q->head == NULL)
        q->tail = NULL;
    // unlock the mutex
    pthread_mutex_unlock(&q->mutex);

    // store the data
    void *data = n->data;
    // free the node
    free(n);
    // return the data
    return data;
}