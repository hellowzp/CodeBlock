#include "queue.h"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <pthread.h>

#ifdef DEBUG
#define DEBUG_PRINTF(msg) do { printf(msg); } while(0);
#else
#define DEBUG_PRINTF(msg)
#endif

/* The caller of the queue is responsible for implementing the functions below */
extern void element_print(element_ptr_t element);
extern void element_copy(element_ptr_t* dest_element, element_ptr_t src_element);
extern void element_free(element_ptr_t* element);

extern const unsigned int QUEUE_ELEMENT_SIZE;

/*
 * The real definition of 'struct queue'
 */
struct queue {
  element_ptr_t elements; // dynamic array containing data elements
  unsigned int front, rear; // the two elements indices
  unsigned int capacity;  //the current total dynamic capacity
  pthread_mutex_t mutex;
};

/**
 * front and rear are init to QUEUE_SIZE,
 * which is invalid for index, so it means an empty queue
 */
Queue queue_create()
{
    Queue queue = (Queue)malloc(sizeof(struct queue));
    queue->elements=(element_ptr_t)malloc(QUEUE_SIZE * sizeof(QUEUE_ELEMENT_SIZE));
    queue->front = QUEUE_SIZE;
    queue->rear = QUEUE_SIZE;
    queue->capacity = QUEUE_SIZE;
    return queue;
}

/**
 * Note that the special case when rear < front for a circular queue
 */
void queue_free(Queue* queue)
{
    assert(queue!=NULL && *queue!=NULL);
    if( (*queue)->front > (*queue)->rear ) {
        element_ptr_t elements = (*queue)->elements;
        int i = 0;
        for( i=0; i< (*queue)->rear; i++ ){
            element_ptr_t current_element = elements + i * QUEUE_ELEMENT_SIZE;
            element_free(&current_element);
        }

        elements += (*queue)->front * QUEUE_ELEMENT_SIZE;
        for( i=0; i< (*queue)->capacity - (*queue)->front; i++ ){
            element_ptr_t current_element = elements + i * QUEUE_ELEMENT_SIZE;
            element_free(&current_element);
        }
    } else {
        element_ptr_t elements = (*queue)->elements + (*queue)->rear * QUEUE_ELEMENT_SIZE;
        int i = 0;
        for( i=0; i< (*queue)->rear - (*queue)->front; i++ ){
            element_ptr_t current_element = elements + i * QUEUE_ELEMENT_SIZE;
            element_free(&current_element);
        }
    }

    free( (*queue)->elements);
    free(*queue);
    *queue = NULL;
    DEBUG_PRINTF('"\n%s\n","Queue free succedd!"');
}

/**
 * Note that the special case when rear < front for a circular queue
 */
void queue_enqueue(Queue queue, element_ptr_t element)
{
    assert(queue!=NULL && element!=NULL);

    int size = queue_size(queue);
    if(size==0){
        queue->front = queue->rear = 0;
        element_copy(&queue->elements, element);
    } else if(size==queue->capacity) {
        queue->capacity += QUEUE_SIZE;
        element_ptr_t newBlock = realloc( queue->elements,
                queue->capacity * QUEUE_ELEMENT_SIZE);
        if(newBlock != NULL) {
            queue->elements = newBlock;
            // should adjust front and rear here based on FIFO
            queue->front = 0;
            queue->rear = queue->capacity - QUEUE_SIZE;
            element_ptr_t newAddr = queue->elements + queue->rear * QUEUE_ELEMENT_SIZE;
            element_copy( &newAddr, element);
        } else {
            printf("\n%s!!\n\n","Queue reallocate memory failed");
        }
    } else {
        if(queue->front <= queue->rear) {
            queue->rear ++;
            element_ptr_t newAddr = queue->elements + queue->rear * QUEUE_ELEMENT_SIZE;
            element_copy( &newAddr, element);
        }
    }

    printf("Queue size: %d %d %d\n", queue_size(queue),
           queue->front, queue->rear);
}

/**
 * Note that the special case when rear < front for a circular queue
 */
int queue_size(Queue queue)
{
    assert(queue!=NULL);
    if(queue->front==queue->capacity) return 0;
    int size = queue->rear - queue->front;
    return size<0 ? queue->capacity + size +1 : size+1;
}

element_ptr_t queue_top(Queue queue){
    assert(!queue);
    if(queue_size(queue)==0) return NULL;
    return queue->elements + queue->rear * QUEUE_ELEMENT_SIZE;
}

void queue_dequeue(Queue queue)
{

}

void queue_print(Queue queue)
{

}

