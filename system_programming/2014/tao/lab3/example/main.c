#include<stdio.h>
#include"queue.h"
#include <assert.h>
#include <stdlib.h>

void ErrHandler( ErrCode err )  
//----------------------------------------------------------------------------//
{
	switch( err ) {
		case ERR_NONE:
			break;
		case  ERR_EMPTY:
			printf( "\nCan't execute this operation while the queue is empty.\n" );
			break;
		case ERR_FULL:
			printf( "\nCan't execute this operation while the queue is full.\n" );
			break;
		case ERR_MEM:
			printf( "\nMemory problem occured while executing this operation on the queue.\n" );
			break;
		case ERR_INIT:
			printf( "\nqueue initialization problem.\n" );
			break;
		case ERR_UNDEFINED:
			printf( "\nUndefined problem occured while executing this operation on the queue.\n" );
			break;
		default: // should never come here
			assert( 1==0 );
	}
}

int main(void)
{
        Element value;	
	    ErrCode err;
        int choice;
	//initialize the queue before using it
	Init();

        do {
		printf( "\nGive a value to push on the queue (negative value to quit): " );
		scanf( "%ld", &value );
		if ( value >= 0 ) {
			err = Enqueue( value );
			ErrHandler( err );
		}
	} while ( value >= 0 );

        int check; 
        do{ 

        printf("\nPlease enter your choices:\n 1,Add an element to the queue.\n 2,Delect an element in the queue.\n3,read the top element of the queue .\n 4,Calculate the size of queue.\n");
        scanf("%d",&choice);

switch(choice){
     int top;
     int size;
     case 1:
     printf("\nPlease enter the element that you want to add to the queue\n");
     scanf("%ld",&value);
     err = Enqueue(value);
     break;

     case 2:
     err = Dequeue();
     break;
     
     case 3:
     
     top = QueueTop();
     printf("\nThe top element of the queue is :%d\n",top);    
     break;
     
     case 4:
     
     size = QueueSize();
     printf("\nThe size of the queue is : %d\n",size);
     break;

     default: printf("\nInvalid choice!\n");
              }
     printf("\nEnter a positive number to continue, a negative number to quit.\n");
     scanf("%d",&check);
      } while(check >= 0);
      
     QueueDestroy();
     return 0; 
}


