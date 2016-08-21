#include "sbuffer.h"
#include "util.h"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>

#ifndef INITIAL_QUEUE_SIZE
    #define INITIAL_QUEUE_SIZE 10
#endif

/* The caller of the buffer is responsible for implementing the functions below */
extern void sbuffer_data_print(sbuffer_data_t* element);
extern void sbuffer_data_copy(sbuffer_data_t** dest_element, sbuffer_data_t* src_element);
extern void sbuffer_data_free(sbuffer_data_t** element);

static inline sbuffer_data_t* get_element_at_index(const sbuffer_t* buffer, unsigned int index);
static inline int get_size(sbuffer_t* queu);
static inline void free_elements(sbuffer_t* buffer);

struct sbuffer {
  sbuffer_data_t * elements;   // dynamic array containing data elements
  unsigned int front, rear; // the front and rear element indices
  unsigned int capacity;    // the current total dynamic capacity
  pthread_rwlock_t rwlock;
};

/**   Implementation Notes
 * Queue front and rear are init to QUEUE_SIZE,
 * which is invalid for index so as to indicate an empty buffer.
 *
 * Inside every function, the appropiate read/write lock should be locked
 * to ensure comsistency. More importantly and error-pronely, if a function
 * return in the middle, remember to unlock first !!
 * And if one function calls another function, consider carefully whether
 * deadlock will happen. ==> use unlocked inner functions such as get_size().
 */
int sbuffer_init(sbuffer_t ** sbuffer) {
    assert(sbuffer != NULL);
    sbuffer_t * buffer = malloc(sizeof(sbuffer_t));
    if (buffer) {
        buffer->elements = malloc( INITIAL_QUEUE_SIZE * sizeof(sbuffer_data_t));
        buffer->front = INITIAL_QUEUE_SIZE;
        buffer->rear = INITIAL_QUEUE_SIZE;
        buffer->capacity = INITIAL_QUEUE_SIZE;
        buffer->rwlock = (pthread_rwlock_t)PTHREAD_RWLOCK_INITIALIZER;
        *sbuffer = buffer;
        return SBUFFER_SUCCESS;
    } else {
        ERROR_PRINT("SBUFFER init error");
        return SBUFFER_FAILURE;
    }
}

/**
 * Note that the special case when rear < front for a circular buffer
 */
int sbuffer_free(sbuffer_t ** buffer) {
    assert(buffer!=NULL && *buffer!=NULL);
    pthread_rwlock_wrlock( &(*buffer)->rwlock );

    free_elements(*buffer);
    pthread_rwlock_unlock( &(*buffer)->rwlock );
    pthread_rwlock_destroy( &(*buffer)->rwlock );
    free(*buffer);
    *buffer = NULL;
    DEBUG_PRINT("\n%s\n","Queue free succeed!");
    return SBUFFER_SUCCESS;
}

/**
 * Note that the special case when rear < front for a circular buffer
 */
int sbuffer_insert(sbuffer_t* buffer, sbuffer_data_t* element) {
    assert(buffer!=NULL && element!=NULL);
    pthread_rwlock_wrlock( &buffer->rwlock );

    int size = get_size(buffer);
    if(size==0){
        buffer->front = buffer->rear = 0;
        sbuffer_data_copy(&buffer->elements, element);
    } else if(size==buffer->capacity) {

        unsigned int newCapacity = buffer->capacity * 2;
        /**
         * don't use realloc, if elements already shifted (front!=0),
         * memory error will happen ==> further test how realloc copy
         * previously allocated memory (like the size copied)
         */
        sbuffer_data_t* newBlock = calloc(newCapacity, sizeof(sbuffer_t));
        if(newBlock != NULL) {
            sbuffer_data_t* frontElement = get_element_at_index(buffer, buffer->front);
            // should do piece-wise copy if front>rear based on FIFO
            if( buffer->front > buffer->rear ){
                int frontBlockSize = (buffer->capacity - buffer->front) * sizeof(sbuffer_t);
                int rearBlockSize = (buffer->rear + 1) * sizeof(sbuffer_t);
                memcpy(newBlock, frontElement, frontBlockSize);
                memcpy(newBlock + frontBlockSize, buffer->elements, rearBlockSize);
            } else {
                memcpy(newBlock, frontElement, buffer->capacity * sizeof(sbuffer_t));
            }
            free_elements(buffer); // free old elements memory
            buffer->front = 0;
            buffer->rear = buffer->capacity;
            buffer->elements = newBlock;
            buffer->capacity = newCapacity;
            sbuffer_data_t* newAddr = get_element_at_index(buffer, buffer->rear);
            sbuffer_data_copy( &newAddr, element);
        } else {
            fprintf( stderr, "\n%s!!\n\n","Queue reallocate memory failed");
        }
    } else {  // just copy the element to the buffer array
        buffer->rear ++;
        if(buffer->rear==buffer->capacity)
            buffer->rear = 0;
        sbuffer_data_t* rearElement = get_element_at_index(buffer, buffer->rear);
        sbuffer_data_copy( &rearElement, element);
    }
    pthread_rwlock_unlock( &buffer->rwlock );
    return SBUFFER_SUCCESS;
}

int sbuffer_remove(sbuffer_t * buffer, sbuffer_data_t * data) {
    assert( buffer != NULL && data != NULL);
    return SBUFFER_SUCCESS;
}

void sbuffer_print(sbuffer_t* buffer) {
    assert(buffer!=NULL);
    pthread_rwlock_rdlock( &buffer->rwlock );
    int size = get_size(buffer);
    printf("\n*****Print Queue*****\nQueue size: %d %d %d\n",
           size, buffer->front, buffer->rear);
    if(size==0) {
        printf("%s","empty buffer...");
    } else if ( buffer->front <= buffer->rear ){
        unsigned int i = 0;
        sbuffer_data_t* e;
        for( i=buffer->front; i<=buffer->rear; i++ ){
            e = get_element_at_index( buffer, i);
            sbuffer_data_print(e);
        }
    } else {
        unsigned int i = 0;
        sbuffer_data_t* e;
        for( i=buffer->front; i<buffer->capacity; i++ ){
            e = get_element_at_index( buffer, i);
            sbuffer_data_print(e);
        }
        for( i=0; i<=buffer->capacity; i++ ){
            e = get_element_at_index( buffer, i);
            sbuffer_data_print(e);
        }
    }

    printf("\n%s\n\n", "*****Print Finished*****");
    pthread_rwlock_unlock( &buffer->rwlock );
}

static inline sbuffer_data_t* get_element_at_index(
        const sbuffer_t* buffer, unsigned int index){
    return buffer->elements + sizeof(sbuffer_data_t) * index;
}

static inline int get_size(sbuffer_t* buffer) {
    if(buffer->front==buffer->capacity)
        return 0;
    int size = buffer->rear - buffer->front + 1;
    return size<=0 ? buffer->capacity + size : size;
}

static inline void free_elements(sbuffer_t* buffer) {
    DEBUG_PRINT("free buffer elements memory\n");
    sbuffer_data_t* element;
    // piece-wise free if front > rear
    if( buffer->front > buffer->rear ) {
        int i = 0;
        for( i=0; i <= buffer->rear ; i++ ){
            element = get_element_at_index(buffer,i);
            sbuffer_data_free(&element);
        }
        for( i=buffer->front; i < buffer->capacity; i++ ){
            element = get_element_at_index(buffer,i);
            sbuffer_data_free(&element);
        }
    } else if( buffer->front < buffer->capacity ){  // not empty
        int i = 0;
        for( i = buffer->front; i <= buffer->rear; i++ ){
            element = get_element_at_index(buffer,i);
            sbuffer_data_free(&element);
        }
    }

    free(buffer->elements);
}
