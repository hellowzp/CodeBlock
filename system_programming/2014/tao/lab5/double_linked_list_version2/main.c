
/*-----------------------------------------------------------------------------
		include files
------------------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "list.h"

/*------------------------------------------------------------------------------
		definitions (defines, typedefs, ...)
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
		function prototypes
------------------------------------------------------------------------------*/
void printoutToEnd(list_ptr_t list);
void printoutToHead(list_ptr_t list);
//static void ErrHandler( const ErrCode err );


int main( )
//----------------------------------------------------------------------------//
{

    list_ptr_t s,sRefer; 
	data_ptr_t value1,value2,valueTemp;
	value1=(data_ptr_t)malloc(sizeof(data_t));
	value2=(data_ptr_t)malloc(sizeof(data_t));
	s=list_alloc();
	//sRefer=list_alloc();
    int i=0;
	
    do{
	printf("please input value in the list:");
	scanf("\n%ld",value1);	   //here indeed we have input the *value
	i++;
	if((*value1)>0)
	   s=list_insert_at_index(s,value1,10);		
    }while((*value1)>0);	
    printoutToEnd(s);
    printf("\nplease input inserted data in the reference:");
    
    scanf("\n%ld",value2);
    printoutToEnd(s);
    
    
    //s=list_free_data(s,value2);
    //printoutToEnd(s);
    
    
   // sRefer=s;
   // while((sRefer->data!=(*value2))&&(sRefer!=NULL)){
  //     sRefer=sRefer->next;}
 
 
 
 //   s=list_insert_at_reference(s,value2,sRefer);
    
 
 //   s=list_insert_sorted(s,value2);


//   s=list_free_at_index(s,5); 
 
 
 /*  sRefer=s;
    while(sRefer->data!=(*value2)){
       sRefer=sRefer->next;
    }
    s=list_free_at_reference(s,sRefer);
 */
 
 
   // s=list_free_data(s,value2);
    
   // list_free(s);
    
   // s=list_free_at_index(s,3);
    
   // printoutToEnd(s);
    
   // s=list_free_at_index(s,1);
    
    //printoutToEnd(s);
    
   // s=list_remove_at_index(s,3);
   
   
   //s=list_remove_data(s,value2);
    
    //s=list_get_first_reference(s);
    //printf("\n%ld",s->data);
    //s=list_get_last_reference(s);
   // printf("\n%ld",s->data);
    
    
 //   s=list_get_reference_at_index(s,2);
   // printf("\n%ld",s->data);
    
    
   // valueTemp=list_get_data_at_reference(s,sRefer);
    //printf("\n%ld",*valueTemp);
    
    valueTemp=list_get_data_at_index(s,2);
    printf("\n%ld",*valueTemp);
    
    
    printoutToEnd(s);
    printoutToHead(s);
    
   list_free(s);
       
	free(value1);
	free(value2);
	return 0;
}



void printoutToEnd(list_ptr_t list){
	
	list_ptr_t temp;
	temp=list;
	printf("\nsize: %d\n",list_size(list));
	while(temp!=NULL){
	   printf("%5ld",temp->data);
	   temp=temp->next;
	}	
}


void printoutToHead(list_ptr_t list){
	
	list_ptr_t temp;
	temp=list;
	printf("\nsize: %d\n",list_size(list));
	while(temp->next!=NULL){temp=temp->next;}
	while(temp!=NULL){
	   printf("%5ld",temp->data);
	   temp=temp->prev;
	}	
}





/*
	//initialize the stack before using it
	Init(&s);

	// read values from the command prompt and push them on the stack
	do
    {
		printf( "Give a value to push on the stack (negative value to quit): " );
		scanf( "%ld", &value );
		if ( value >= 0 )
		{
			err = Push( s, value );
			ErrHandler( err );
		}
	} while ( value >= 0 );

#ifdef DEBUG
	printf( "\nThe stack size is %d\n", StackSize() );
#endif

	printf( "\nThe stack values are:\n" );
	while ( StackSize( s ) > 0)
	{
		err = Top( s, &value);
		ErrHandler( err );
		printf( "%ld\n", value );
		ErrHandler( Pop( s ) );
	}

	//destroy the stack
	Destroy( &s );

	return 0;
}


//----------------------------------------------------------------------------//
static void ErrHandler( const ErrCode err )
//----------------------------------------------------------------------------//
{
	switch( err ) {
		case ERR_NONE:
			break;
		case  ERR_EMPTY:
			printf( "\nCan't execute this operation while the stack is empty.\n" );
			break;
		case ERR_FULL:
			printf( "\nCan't execute this operation while the stack is full.\n" );
			break;
		case ERR_MEM:
			printf( "\nMemory problem occured while executing this operation on the stack.\n" );
			break;
		case ERR_INIT:
			printf( "\nStack initialization problem.\n" );
			break;

		case ERR_UNDEFINED:
			printf( "\nUndefined problem occured while executing this operation on the stack.\n" );
			break;
		default: // should never come here
			assert( 1==0 );
	}
}

*/




