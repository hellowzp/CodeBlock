#include <stdio.h>
#include "myqueue.h"
#include <stdlib.h>

#define DEBUG_PRINTF /* define here a debug macro */

/* The caller of the queue is responsible for implementing the functions below */
extern void element_print(element_t element);
extern void element_copy(element_t *dest_element, element_t src_element);
extern void element_free(element_t *element);

/*
 * The real definition of 'struct queue'
 */
struct queue {
  element_t *arr; // dynamic array containing data elements
  int current_size; // Counts number of elements in the queue
  int front, rear;
  // Remark for later: extra fields need to be added here to make it a thread-safe queue as needed for the assigment 
};

queue_t* queue_create()
{
  // implementation goes here
  	queue_t *new_queue=(queue_t *)malloc(QUEUE_SIZE * sizeof(queue_t));
    new_queue->arr=(element_t *)malloc(QUEUE_SIZE * sizeof(element_t));
	new_queue->front =0;
	new_queue->rear = 0;
    new_queue->current_size=0;
    return new_queue;
}

void queue_free(queue_t** queue)
{
  // implementation goes here
    if(*queue==NULL)
    {
		 printf("\nQueue doesn't exists,free failed!\n");
    }
    else
    {
        free(*queue);
        *queue=NULL;
        printf("\nFree success!\n");
    }
}

void queue_enqueue(queue_t* queue, element_t element)
{
  // implementation goes here
    if(queue->arr==NULL)	
    {
		printf("\nQueue doesn't exists, please create a queue first:\n");
    }
    else if(queue->current_size!=0&&queue->front==queue->rear)
	{
		printf("\nFailed, queue is full:\n");  
	}
    else
    {
	    queue->arr[queue->rear] = element;
	    queue->rear = (queue->rear + 1) % QUEUE_SIZE;
	    queue->current_size=queue->current_size + 1;
        if(queue->front==queue->rear)
        {
			queue->current_size=QUEUE_SIZE;
	    }
        else;
        printf("\nEnter queue success!\n");
	}
}

int queue_size(queue_t* queue)
{
  // implementation goes here
    if(queue->arr==NULL)
    {
		 return -1;
	}
    else if(queue->current_size!=0&&queue->front==queue->rear)
    {
		 printf("\nThe queue is full, return the max size--");
		 return QUEUE_SIZE;
	}
    else
    {
	     printf("\nThe queue is not full, return current size--");
         return (queue->current_size);//////
    }
}

element_t* queue_top(queue_t* queue){
  // implementation goes here
	element_t *p=(element_t*)malloc(sizeof(element_t));
    if(queue->arr==NULL)  
    {
		 *p=-1;
		 return p;
    }
    else if(queue->current_size==0&&queue->front==queue->rear)
	{
		 *p=0;
		 return p;
	}
    else
    {
	    *p = queue->arr[queue->front];
	    printf("\nDiaplay first element success!\n");
	    return p;
	}
}

void queue_dequeue(queue_t* queue)
{
  // implementation goes here
    if(queue->arr==NULL)	
    {  
		 printf("\nQueue doesn't exists, please create a queue first:\n");	
	}
    else if(queue->current_size==0&&queue->front==queue->rear)
	{
		 printf("\nFailed, queue is empty:\n");
	}
    else
    {
	    queue->front = (queue->front + 1) % QUEUE_SIZE;
	    queue->current_size=queue->current_size-1;////
        if(queue->front==queue->rear)
        {
			queue->current_size=0;
	    }
        else;
        printf("\nThe top element out of queue success!\n");
	}
}

void queue_print(queue_t *queue)
{
  // implementation goes here
  if(queue->arr==NULL)	
  {  
		 printf("\nQueue doesn't exists!\n");	
  }
  else if(queue->current_size==0&&queue->front==queue->rear)
  {
		 printf("\nPrint nothing! Queue is emputy!\n");
  }
  else
  {
         int i;
         printf("\nPrint queue success!");
         for(i=queue->front;i!=queue->rear;i =(i+1)%QUEUE_SIZE)
             printf("%d,",queue->arr[i]);
  }
}

