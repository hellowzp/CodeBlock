#include "queue.h"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>

#ifdef DEBUG
#define DEBUG_PRINTF(...) do { fprintf( stdout, __VA_ARGS__); } while(0)
#else
#define DEBUG_PRINTF(...)
#endif

/* The caller of the queue is responsible for implementing the functions below */
extern void queue_element_print(element_ptr_t element);
extern void queue_element_copy(element_ptr_t* dest_element, element_ptr_t src_element);
extern void queue_element_free(element_ptr_t* element);

extern const unsigned int QUEUE_ELEMENT_SIZE;

static inline element_ptr_t get_element_at_index(const queue_ptr_t queue, unsigned int index);
static inline int get_size(queue_ptr_t queu);
static inline void free_elements(queue_ptr_t queue);

struct queue {
  element_ptr_t elements;   // dynamic array containing data elements
  unsigned int front, rear; // the two elements indices
  unsigned int capacity;    //the current total dynamic capacity
  pthread_rwlock_t rwlock;
};

/**   Implementation Notes
 * Queue front and rear are init to QUEUE_SIZE,
 * which is invalid for index so as to indicate an empty queue.
 *
 * Inside every function, the appropiate read/write lock should be locked
 * to ensure comsistency. More importantly and error-pronely, if a function
 * return in the middle, remember to unlock first !!
 * And if one function calls another function, consider carefully whether
 * deadlock will happen. ==> use unlocked inner functions such as get_size().
 */
queue_ptr_t queue_create()
{
    queue_ptr_t queue = malloc(sizeof(queue_t));
    queue->elements = malloc( INITIAL_QUEUE_SIZE * sizeof(QUEUE_ELEMENT_SIZE));
    queue->front = INITIAL_QUEUE_SIZE;
    queue->rear = INITIAL_QUEUE_SIZE;
    queue->capacity = INITIAL_QUEUE_SIZE;
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

    free_elements(*queue);
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

    int size = get_size(queue);
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
        element_ptr_t newBlock = calloc(newCapacity, QUEUE_ELEMENT_SIZE);
        if(newBlock != NULL) {
            element_ptr_t frontElement = get_element_at_index(queue, queue->front);
            // should do piece-wise copy if front>rear based on FIFO
            if( queue->front > queue->rear ){
                int frontBlockSize = (queue->capacity - queue->front) * QUEUE_ELEMENT_SIZE;
                int rearBlockSize = (queue->rear + 1) * QUEUE_ELEMENT_SIZE;
                memcpy(newBlock, frontElement, frontBlockSize);
                memcpy(newBlock + frontBlockSize, queue->elements, rearBlockSize);
            } else {
                memcpy(newBlock, frontElement, queue->capacity * QUEUE_ELEMENT_SIZE);
            }
            free_elements(queue); // free old elements memory
            queue->front = 0;
            queue->rear = queue->capacity;
            queue->elements = newBlock;
            queue->capacity = newCapacity;
            element_ptr_t newAddr = get_element_at_index(queue, queue->rear);
            queue_element_copy( &newAddr, element);
        } else {
            fprintf( stderr, "\n%s!!\n\n","Queue reallocate memory failed");
        }
    } else {  // just copy the element to the queue array
        queue->rear ++;
        if(queue->rear==queue->capacity)
            queue->rear = 0;
        element_ptr_t rearElement = get_element_at_index(queue, queue->rear);
        queue_element_copy( &rearElement, element);
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
    int size = get_size(queue);
    pthread_rwlock_unlock( &queue->rwlock );
    return size;
}

element_ptr_t queue_top(queue_ptr_t queue)
{
    pthread_rwlock_rdlock( &queue->rwlock );
    assert(queue!=NULL);
    element_ptr_t e = NULL;
    if( get_size(queue) > 0)
        e = get_element_at_index(queue, queue->front);
    pthread_rwlock_unlock( &queue->rwlock );
    return e;
}

void queue_dequeue(queue_ptr_t queue)
{
    // must be locked at the first beginning to ensure consistency
    pthread_rwlock_wrlock( &queue->rwlock );
    assert(queue!=NULL);
    int size = get_size(queue);
    if(size==0) {
        printf("%s\n", "Invalid peration: dequeue an empty queue");
        // be sure to unlock before return
        pthread_rwlock_unlock( &queue->rwlock );
        return;
    }

    element_ptr_t top = get_element_at_index(queue, queue->front);
    queue_element_free(&top);
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
    int size = get_size(queue);
    printf("\n*****Print Queue*****\nQueue size: %d %d %d\n",
           size, queue->front, queue->rear);
    if(size==0) {
        printf("%s","empty queue...");
    } else if ( queue->front <= queue->rear ){
        unsigned int i = 0;
        element_ptr_t e;
        for( i=queue->front; i<=queue->rear; i++ ){
            e = get_element_at_index( queue, i);
            queue_element_print(e);
        }
    } else {
        unsigned int i = 0;
        element_ptr_t e;
        for( i=queue->front; i<queue->capacity; i++ ){
            e = get_element_at_index( queue, i);
            queue_element_print(e);
        }
        for( i=0; i<=queue->capacity; i++ ){
            e = get_element_at_index( queue, i);
            queue_element_print(e);
        }
    }

    printf("\n%s\n\n", "*****Print Finished*****");
    pthread_rwlock_unlock( &queue->rwlock );
}

static inline element_ptr_t get_element_at_index(
        const queue_ptr_t queue, unsigned int index)
{
    return queue->elements + QUEUE_ELEMENT_SIZE * index;
}

static inline int get_size(queue_ptr_t queue) {
    if(queue->front==queue->capacity)
        return 0;
    int size = queue->rear - queue->front + 1;
    return size<=0 ? queue->capacity + size : size;
}

static inline void free_elements(queue_ptr_t queue) {
    DEBUG_PRINTF("free queue elements memory\n");
    element_ptr_t element;
    // piece-wise free if front > rear
    if( queue->front > queue->rear ) {
        int i = 0;
        for( i=0; i <= queue->rear ; i++ ){
            element = get_element_at_index(queue,i);
            queue_element_free(&element);
        }
        for( i=queue->front; i < queue->capacity; i++ ){
            element = get_element_at_index(queue,i);
            queue_element_free(&element);
        }
    } else if( queue->front < queue->capacity ){  // not empty
        int i = 0;
        for( i = queue->front; i <= queue->rear; i++ ){
            element = get_element_at_index(queue,i);
            queue_element_free(&element);
        }
    }

    free(queue->elements);
}

