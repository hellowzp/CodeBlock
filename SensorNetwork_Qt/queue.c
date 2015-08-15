#include "queue.h"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>

#ifdef DEBUG
#define DEBUG_PRINTF(...) do { fprintf( stderr, __VA_ARGS__); } while(0)
#else
#define DEBUG_PRINTF(...)
#endif

/* The caller of the queue is responsible for implementing the functions below */
extern void queue_element_print(element_ptr_t element);
extern void queue_element_copy(element_ptr_t* dest_element, element_ptr_t src_element);
extern void queue_element_free(element_ptr_t* element);

extern const unsigned int QUEUE_ELEMENT_SIZE;

/**
 * get the valid element of the circular queue at index,
 * the index may be out of bound of the queue memory.
 * The size parameter is the maximum number of elements
 * the queue can contain.
 */
static element_ptr_t get_valid_element(const element_ptr_t start_element_addr,
                                       unsigned int index, unsigned int size);

/*
 * The real definition of 'struct queue'
 */
struct queue {
  element_ptr_t elements; // dynamic array containing data elements
  unsigned int front, rear; // the two elements indices
  unsigned int capacity;  //the current total dynamic capacity
  pthread_rwlock_t rwlock;
};

/**
 * front and rear are init to QUEUE_SIZE,
 * which is invalid for index, so it means an empty queue
 */
queue_ptr_t queue_create()
{
    queue_ptr_t queue = (queue_ptr_t)malloc(sizeof(queue_t));
    queue->elements=(element_ptr_t)malloc(QUEUE_SIZE * sizeof(QUEUE_ELEMENT_SIZE));
    queue->front = QUEUE_SIZE;
    queue->rear = QUEUE_SIZE;
    queue->capacity = QUEUE_SIZE;
    queue->rwlock = (pthread_rwlock_t)PTHREAD_RWLOCK_INITIALIZER;
    return queue;
}

/**
 * Note that the special case when rear < front for a circular queue
 */
void queue_free(queue_ptr_t* queue)
{
    pthread_rwlock_wrlock( &(*queue)->rwlock );
    assert(queue!=NULL && *queue!=NULL);

    // piece-wise free if front > rear
    if( (*queue)->front > (*queue)->rear ) {
        element_ptr_t elements = (*queue)->elements;
        int i = 0;
        for( i=0; i< (*queue)->rear; i++ ){
            element_ptr_t current_element = elements + i * QUEUE_ELEMENT_SIZE;
            queue_element_free(&current_element);
        }

        elements += (*queue)->front * QUEUE_ELEMENT_SIZE;
        for( i=0; i< (*queue)->capacity - (*queue)->front; i++ ){
            element_ptr_t current_element = elements + i * QUEUE_ELEMENT_SIZE;
            queue_element_free(&current_element);
        }
    } else {
        element_ptr_t elements = (*queue)->elements + (*queue)->rear * QUEUE_ELEMENT_SIZE;
        int i = 0;
        for( i=0; i< (*queue)->rear - (*queue)->front; i++ ){
            element_ptr_t current_element = elements + i * QUEUE_ELEMENT_SIZE;
            queue_element_free(&current_element);
        }
    }

    free( (*queue)->elements );
    pthread_rwlock_unlock( &(*queue)->rwlock );
    pthread_rwlock_destroy( &(*queue)->rwlock );
    free(*queue);
    *queue = NULL;
    DEBUG_PRINTF("\n%s\n","Queue free succeed!");
}

/**
 * Note that the special case when rear < front for a circular queue
 */
void queue_enqueue(queue_ptr_t queue, element_ptr_t element)
{
    pthread_rwlock_wrlock( &queue->rwlock );
    assert(queue!=NULL && element!=NULL);

    int size = queue_size(queue);
    if(size==0){
        queue->front = queue->rear = 0;
        queue_element_copy(&queue->elements, element);
    } else if(size==queue->capacity) {

        unsigned int newCapacity = queue->capacity * 2;
        /**
         * don't use realloc, if elements already shifted (front!=0),
         * memory error will happen ==> further test how realloc copy
         * previously allocated memory (like the size copied)
         */
//      queue->capacity += QUEUE_SIZE;
//      element_ptr_t newBlock = realloc(
//                queue->elements + queue->front * QUEUE_ELEMENT_SIZE,
//                queue->capacity * QUEUE_ELEMENT_SIZE);
        element_ptr_t newBlock = calloc(QUEUE_ELEMENT_SIZE, newCapacity);
        if(newBlock != NULL) {
            element_ptr_t frontElement = queue->elements + queue->front * QUEUE_ELEMENT_SIZE;
            // should do piece-wise copy if front>rear based on FIFO
            if( queue->front > queue->rear ){
                int frontBlockSize = (queue->capacity - queue->front) * QUEUE_ELEMENT_SIZE;
                int rearBlockSize = (queue->rear + 1) * QUEUE_ELEMENT_SIZE;
                memcpy(newBlock, frontElement, frontBlockSize);
                memcpy(newBlock + frontBlockSize, queue->elements, rearBlockSize);
            } else {
                memcpy(newBlock, frontElement, queue->capacity * QUEUE_ELEMENT_SIZE);
            }
            queue->front = 0;
            queue->rear = queue->capacity;
            queue->elements = newBlock;
            queue->capacity = newCapacity;
            element_ptr_t newAddr = newBlock + queue->rear * QUEUE_ELEMENT_SIZE;
            queue_element_copy( &newAddr, element);
        } else {
            fprintf( stderr, "\n%s!!\n\n","Queue reallocate memory failed");
        }
    } else {  // just copy the element to the queue array
        queue->rear ++;
        if(queue->rear==queue->capacity)
            queue->rear = 0;
        element_ptr_t rear_element = queue->elements + queue->rear * QUEUE_ELEMENT_SIZE;
        queue_element_copy( &rear_element, element);
    }
    pthread_rwlock_unlock( &queue->rwlock );
}

/**
 * Note that the special case when rear < front for a circular queue
 */
int queue_size(queue_ptr_t queue)
{
    pthread_rwlock_rdlock( &queue->rwlock );
    assert(queue!=NULL);
    if(queue->front==queue->capacity) return 0;
    int size = queue->rear - queue->front;
    size = size<0 ? queue->capacity + size +1 : size+1;
    pthread_rwlock_unlock( &queue->rwlock );
    return size;
}

element_ptr_t queue_top(queue_ptr_t queue){
    pthread_rwlock_rdlock( &queue->rwlock );
    assert(queue!=NULL);
    if(queue_size(queue)==0) return NULL;
    element_ptr_t e = queue->elements + queue->front * QUEUE_ELEMENT_SIZE;
    pthread_rwlock_unlock( &queue->rwlock );
    return e;
}

void queue_dequeue(queue_ptr_t queue)
{
    assert(queue!=NULL);
    int size = queue_size(queue);
    if(size==0) {
        printf("%s\n", "Invalid peration: dequeue an empty queue");
        return;
    }

    element_ptr_t top = queue_top(queue);
    queue_element_free(&top);
    pthread_rwlock_wrlock( &queue->rwlock );
    if(size==1) { // only one element, make it to the initial empty state
        queue->front = queue->rear = queue->capacity;
    } else {
        queue->front ++;
        if(queue->front==queue->capacity)
            queue->front = 0;
    }   
    pthread_rwlock_unlock( &queue->rwlock );
}

void queue_print(queue_ptr_t queue)
{
    pthread_rwlock_rdlock( &queue->rwlock );
    assert(queue!=NULL);
    printf("\n*****Print Queue*****\nQueue size: %d %d %d\n",
           queue_size(queue), queue->front, queue->rear);
    unsigned int rear = queue->rear;
    if( queue->front > rear ){
        rear += queue->capacity;
    }
    unsigned int i;
    for( i=queue->front; i<=rear; i++ ){
        element_ptr_t e = get_valid_element( queue->elements, i, queue->capacity);
        queue_element_print(e);
    }
    printf("\n%s\n\n", "*****Print Finished*****");
    pthread_rwlock_unlock( &queue->rwlock );
}

static element_ptr_t get_valid_element(const element_ptr_t start_element_addr,
                                       unsigned int index, unsigned int size)
{
    return start_element_addr + QUEUE_ELEMENT_SIZE * (index%size);
}

