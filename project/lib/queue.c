#include "queue.h"

/*
### Description
    Initialization of a queue
### Parameters
    - `Queue *q` is a pointer to a Queue data structure (defined in lib/queue.h)
*/
void queue_init(Queue *q)
{
    // initialize the queue
    q->head = q->tail = NULL;
    // initialize mutex and condition variable
    pthread_mutex_init(&(q->mutex), NULL);
    pthread_cond_init(&(q->notEmpty), NULL);
}

/*
### Description
    Destroys a queue
### Parameters
    - `Queue *q` is a pointer to a Queue data structure (defined in lib/queue.h)
*/
void queue_destroy(Queue *q)
{
    pthread_mutex_lock(&(q->mutex));

    // empty the queue
    Node *current = q->head;
    while (current != NULL)
    {
        Node *temp = current;
        current = current->next;
        // free data if it's allocated
        free(temp->data);
        // free node
        free(temp);
    }
    // destroy the mutex and condition variable
    pthread_mutex_destroy(&(q->mutex));
    pthread_cond_destroy(&(q->notEmpty));
}

/*
### Description
    Push (insert) value of `data` in the queue
### Parameters
    - `void *data` is a pointer to the value to insert
    - `Queue *q` is the queue in which the func insert the value
*/
void queue_push(void *data, Queue *q)
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
    pthread_cond_signal(&q->notEmpty);
    // unlock the mutex
    pthread_mutex_unlock(&q->mutex);
}

/*
### Description
    Remove the first item from the queue and give it to caller
### Parameters
    - `Queue *q` is the queue from which the func remove the item
    - `bool cond` is an optional condition. When it's true, the function stop to wait for the condition variable. If don't need it set as `NULL`
### Return value
    Return `data`, the first one item in the queue (head), the type depends on which data have been insert
    If it stops for the cond, return -1
*/
void *queue_pop(Queue *q)
{
    // lock the mutex
    pthread_mutex_lock(&q->mutex);
    // check if the queue is empty
    while (q->head == NULL)
    {
        // pthread_cond_wait(&(q->notEmpty), &(q->mutex));
        pthread_cond_wait(&(q->notEmpty), &(q->mutex));
    }
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