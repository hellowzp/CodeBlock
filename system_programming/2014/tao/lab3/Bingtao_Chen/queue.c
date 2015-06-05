#include<stdio.h>
#include"queue.h"
#include<assert.h>
#include <stdlib.h>

#define INIT_STACK_SIZE	10


void CheckAssert( void );
struct Queue{
	Element *data;
	int front;
        int rear;
	int maxSize;
	};
typedef struct Queue member;
member queue;

				
/*------------------------------------------------------------------------------
		cheak error			      
------------------------------------------------------------------------------*/

ErrCode Init( void ) 
//----------------------------------------------------------------------------//
{

	queue.data = (Element *) malloc ( sizeof(Element) * INIT_STACK_SIZE );
	if (queue.data == NULL)
		return ERR_MEM;
	else
	queue.front = -1;	/* indicates empty stack */
        queue.rear = -1;
        queue.maxSize = INIT_STACK_SIZE;
	return ERR_NONE;
}

ErrCode Enqueue( Element element ) 
{
  ErrCode overflow;
  CheckAssert();
  if(queue.front == (queue.rear+1)%queue.maxSize)
{
  printf("The queue is overflowed!");
  overflow = ERR_FULL;
}
  
 else{
    if(queue.front == -1 && queue.rear == -1)
{
    queue.front = 0;
    queue.rear = 0;
    queue.data[queue.rear] = element;
    printf("Rear is %d, Front is %d\n",queue.rear,queue.front);
    overflow = ERR_NONE;
}
  else 
   {queue.rear = (queue.rear+1)%queue.maxSize;
    queue.data[queue.rear] = element; 
    printf("Rear is %d, Front is %d\n",queue.rear,queue.front);
    overflow = ERR_NONE;
    }
}
return overflow;  
}

ErrCode Dequeue(void) 
{  
   ErrCode underflow;
   CheckAssert();
   if(queue.front==-1)
   underflow = ERR_EMPTY;
   else
   {
   underflow = ERR_NONE;
   int a = queue.data[queue.front];
   if(queue.front==queue.rear)
{
   queue.front = -1;
   queue.rear  = -1;;
}
   else
{
   queue.front = (queue.front+1)%queue.maxSize;
   printf("The deleted element from the queue is %d\n",a);
   printf("Rear is %d, Front is %d\n",queue.rear,queue.front);
}
}
   return underflow;
}

int QueueTop()
{   
    CheckAssert();
    int top;
    if(queue.front==-1)
{   
    top = 0;
    printf("The queue is empty!\n");
    
}
    else
{
    top =  queue.data[queue.front];
}    
    return top;
}

int QueueSize() 
{   
    CheckAssert();
    int size;
    if(queue.front==-1)
    printf("The size of the queue is zero!\n");
    else{
          if(queue.front==queue.rear)
          size=1;
          else if(queue.rear > queue.front)
          size = queue.rear - queue.front +1;
          else
          size = queue.maxSize-queue.front+queue.rear+1;
         }
    return size;
}

void QueueDestroy(void)
{
     free(queue.data);
}

void CheckAssert(void)
{
  assert(queue.rear <= queue.maxSize -1);
}
