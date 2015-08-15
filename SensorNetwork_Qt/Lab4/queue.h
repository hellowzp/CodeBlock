#ifndef MYQUEUE_
#define MYQUEUE_

typedef void* element_ptr_t;

typedef struct queue queue_t;
typedef struct queue* Queue;

/*
 ** The default queue size is 5
 */
#ifndef QUEUE_SIZE
    #define QUEUE_SIZE 5
#endif

/*
 **  Creates (memory allocation!) and initializes the queue and prepares it for usage
 **  Return a pointer to the newly created queue
 **  Returns NULL if queue creation failed
 */
Queue queue_create();

/*  
 **  Add an element to the queue
 **  Does nothing if queue is full
 */
void queue_enqueue(Queue queue, element_ptr_t element);

/*
 **  Delete the queue from memory; set queue to NULL
 **  The queue can no longer be used unless queue_create is called again
 */
void queue_free(Queue* queue);

/*
 **  Return the number of elements in the queue
 */
int queue_size(Queue queue);

/*
 **  Return a pointer to the top element in the queue
 **  Returns NULL if queue is empty
 */
element_ptr_t queue_top(Queue queue);

/*
 **  Remove the top element from the queue
 **  Does nothing if queue is empty
 */
void queue_dequeue(Queue queue);

/*
 **  Print all elements in the queue, starting from the front element
 */
void queue_print(Queue queue);

#endif //MYQUEUE_


