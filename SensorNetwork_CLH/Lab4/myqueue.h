#ifndef MYQUEUE_
#define MYQUEUE_

typedef struct queue queue_t;

/*
 ** The default queue size is 5
 */

#ifndef QUEUE_SIZE
    #define QUEUE_SIZE 5
#endif

/*
 **  Make some changes here to define the type of element that will be stored in the queue
 */

typedef int element_t;

/*
 **  Creates (memory allocation!) and initializes the queue and prepares it for usage
 **  Return a pointer to the newly created queue
 **  Returns NULL if queue creation failed
 */
queue_t* queue_create();

/*  
 **  Add an element to the queue
 **  Does nothing if queue is full
 */
void queue_enqueue(queue_t* queue, element_t element);

/*
 **  Delete the queue from memory; set queue to NULL
 **  The queue can no longer be used unless queue_create is called again
 */
void queue_free(queue_t** queue);

/*
 **  Return the number of elements in the queue
 */
int queue_size(queue_t* queue);

/*
 **  Return a pointer to the top element in the queue
 **  Returns NULL if queue is empty
 */
element_t* queue_top(queue_t* queue);

/*
 **  Remove the top element from the queue
 **  Does nothing if queue is empty
 */
void queue_dequeue(queue_t* queue);

/*
 **  Print all elements in the queue, starting from the front element
 */
void queue_print(queue_t *queue);

#endif //MYQUEUE_


