#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

#include "list.h"

int compareInt(const void* a,const void* b);

int main( void )
{

    list_ptr_t l;

	int value,index;
	

	//initialize the list before using it
	l=list_alloc(compareInt);
	do{
		
		
		printf( "\nGive a value and its index to push in the list(negative value to quit): " );
		scanf( "%d %d", &value,&index );
		if(value>0)
		{
			l=list_insert_at_index(l,(void *)&value, index);
			
			printf("The size of the list is %d\n",l->size);
		}
    }while(value>=0);
    
	return 0;
}


int compareInt(const void* a,const void* b)
	{
		return *(int *)a-*(int *)b; 
	}


