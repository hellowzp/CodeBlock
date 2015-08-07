#ifndef MYQUEUE_
#define MYQUEUE_

//typedef struct queue Queue;

/* 
**  Make some changes to the defined type of DATATYPE so that queue can accept varries of data type
*/
extern int QUEUE_SIZE;

typedef int DATATYPE;

typedef struct queue										
{
 DATATYPE *base;
 DATATYPE front;
 DATATYPE rear;
 DATATYPE full;
}Queue;
/*
**  Creates and initializes the queue and prepares it for usage
**  Return a pointer to the newly created queue
*/
Queue* queue_create();

/*  
**  Add an element of type defined DATATYPE to the queue
*/
void queue_enqueue(Queue* queue, DATATYPE element);

/*
**  Delete the queue from memory; set queue to NULL
**  The queue can no longer be used unless QueueCreate is called again
*/
void queue_free(Queue** queue);


/*
**  Return the number of elements in the queue
*/
int queue_size(Queue* queue);

/*
**  Return a pointer to the top element in the queue
*/
DATATYPE* queue_top(Queue* queue);

/*
**  Remove the top element from the queue
*/
void queue_dequeue(Queue* queue);


#endif //MYQUEUE_
