#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include "my_queue.h"

extern int QUEUE_SIZE;

Queue* queue_create()   //empty queue,Initialization
{
	Queue *new_queue=(Queue *)malloc(QUEUE_SIZE * sizeof(Queue));
    new_queue->base=(DATATYPE *)malloc(QUEUE_SIZE * sizeof(DATATYPE));
	new_queue->front =0;
	new_queue->rear = 0;
    new_queue->full=0;
    return new_queue;
}

void queue_free(Queue **queue)  //delete the queue
{
     if(*queue==NULL)
     {
		 printf("Queue doesn't exists,destroy failed!\n");
     }
     else
     {
        free(*queue);
        *queue=NULL;
        printf("Destroy queue success!\n");
      }
}

int queue_size(Queue *queue)
{
     if(queue->base==NULL)
     {
		 return -1;
	 }
     else if(queue->full==1&&queue->front==queue->rear)
     {
		 printf("The queue is full\n");
		 return QUEUE_SIZE;
	 }
     else
     {
//	     printf("The queue is not full, return current size--\n");
         return (queue->rear + QUEUE_SIZE - queue->front) % QUEUE_SIZE;
     }
}

DATATYPE* queue_top(Queue *queue)//return the top ele of the queue
{
	 DATATYPE *p=(DATATYPE*)malloc(sizeof(DATATYPE));
     if(queue->base==NULL)  
     {
		 *p=-1;
		 return p;
     }
     else if(queue->full==0&&queue->front==queue->rear)
	 {
		 *p=0;
		 return p;
	 }
     else
     {
	    *p = queue->base[queue->front];
//	    printf("Diaplay first element success!\n");
	    return p;
	 }
}

void queue_enqueue(Queue *queue,DATATYPE element)//insert pas Q new ele
{
     if(queue->base==NULL)	
     {
		printf("Queue doesn't exists, please create a queue first:\n");
     }
     else if(queue->full==1&&queue->front==queue->rear)
	 {
		printf("Failed, queue is full:\n");  
	 }
     else
     {
	    queue->base[queue->rear] = element;
	    queue->rear = (queue->rear + 1) % QUEUE_SIZE;
        if(queue->front==queue->rear)
        {
			queue->full=1;
	    }
        else;
  //      printf("Enter queue success!\n");
	 }
}

void queue_dequeue(Queue *queue)//delete Q's top element use p return the value
{
     if(queue->base==NULL)	
     {  
		 printf("Queue doesn't exists, please create a queue first:\n");	
	 }
     else if(queue->full==0&&queue->front==queue->rear)
	 {
		 printf("Failed, queue is empty:\n");
	 }
     else
     {
	    queue->front = (queue->front + 1) % QUEUE_SIZE;
        if(queue->front==queue->rear)
        {
			queue->full=0;
	    }
        else;
 //       printf("The top element out of queue success!\n");
	 }
}

