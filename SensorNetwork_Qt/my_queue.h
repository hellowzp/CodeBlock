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
Queue* QueueCreate();

/*  
**  Add an element of type defined DATATYPE to the queue
*/
void Enqueue(Queue* queue, DATATYPE element);

/*
**  Delete the queue from memory; set queue to NULL
**  The queue can no longer be used unless QueueCreate is called again
*/
void QueueDestroy(Queue** queue);


/*
**  Return the number of elements in the queue
*/
int QueueSize(Queue* queue);

/*
**  Return a pointer to the top element in the queue
*/
DATATYPE* QueueTop(Queue* queue);

/*
**  Remove the top element from the queue
*/
void Dequeue(Queue* queue);


#endif //MYQUEUE_
