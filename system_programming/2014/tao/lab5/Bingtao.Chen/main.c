#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

#include "list.h"

void ErrHandler( int erro );
int compareInt(const void* a,const void* b);
extern int list_erro;
int main( void )
{

    list_ptr_t l;
    //l=(list_ptr_t)malloc(sizeof(list_t));

	int choose,value,index;
			
	printf( "\n select  your operation:" );
	printf("\n");
	printf("\n");
    printf("\n-1 :exit");
    printf("\n0 :  new list");
    printf("\n1 :  free list");
    printf("\n2 :  size of list;");
    printf("\n3 :  insert data by index");                                       //test the function inplemented in list.c

	scanf( "%d",&choose);
	if(choose>=-1&&choose<=24)
	{
			switch(choose)
			{
				  case -1:
				      exit(0);
				      break;
				  case 0:
				     l=list_alloc();                                  //initialize the list before using it
				     printf("\nGive a value and its index: " );
				     scanf( "%d %d", &value,&index );
				     l=list_insert_at_index(l,(void *)&value, index);                                                    
				     ErrHandler(list_erro);
				     break;
				  case 1:
				     list_free(l);
				     ErrHandler(list_erro);
				     break;
				  case 2:
				     printf("\n%d",list_size(l));
				     ErrHandler(list_erro);
				     break;
				  case 3:
     				 printf("\nplease input index:");    	 
					 scanf("\n%d",&index);
					 l=list_insert_at_index(l,(void*)&value,index);  
					 ErrHandler(list_erro);     					 			         
				     break;
				 	
				  default:                                                      //should never come here
				  assert(1==0);
		      }
		
	}

    
	return 0;
}


int compareInt(const void* a,const void* b)
{
		return *(int *)a-*(int *)b; 
}
//--------------------copy from the lecture source code-----------------------//        
void ErrHandler( int erro )
//----------------------------------------------------------------------------//                    
{
	switch( erro )
	 {
		case 0:
			break;
		case  1:
			printf("\nMemory problem occured while executing this operation on the list.\n" );
			break;
		case  2:
			printf( "\nCan't execute this operation while the list is empty.\n"  );
			break;
		case  3:
		    printf("\n No such data exist in the list\n");	
		    break;
		case  4:
		    printf("\n No such reference exist in the list\n");	
		    break;		    
                case  5:
		    printf("\n list is empty now\n");	
		    break;		    
		    
		default:                                                                // should never come here
			assert( 1==0 );
	}
}
