#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "list.h"

list_ptr_t list_alloc ( compareFunc* compare )
{		
#ifdef DEBUG
printf( "\n list_alloc() is called\n" );
#endif

	list_ptr_t list;
	 
	list = (list_ptr_t)malloc(sizeof(list_t));
	
	list->size=0;
	list->head=NULL;
	list->tail=NULL;
	list->compare=compare;
	
	return list;

}

void list_free( list_ptr_t list )
{
#ifdef DEBUG
printf( "\n list_free() is called\n" );
#endif

	Tlist temp = list->head;
	int i;
	for(i=1;i<=list->size;i++)
	{
		free(temp);
		temp = temp->next;
	}
	free(list);
}

int list_size( list_ptr_t list )
{
	
#ifdef DEBUG
printf( "\n list_size() is called\n" );
#endif

	return list->size;
}

list_ptr_t list_insert_at_index( list_ptr_t list, data_ptr_t data, int index)
{
	
#ifdef DEBUG
printf( "\n list_insert_at_index() is called\n" );
#endif

	Tlist newNode;
	newNode = (Tlist)malloc(sizeof(Tnode));
	assert(newNode);
	newNode->data = data;
	if(index <= 0)
	{	
		newNode->next = list->head;
		list->head = newNode;
		newNode->prev = NULL;
	}
	else if(index > list->size)
	{	
		newNode->prev = list->tail;
		list->tail = newNode;
		newNode->next = NULL;
	}
	else
	{
		Tlist temp = list->head;
		int i;
		for(i=1;i<=list->size;i++)
		{
			temp = temp->next;
			if(i == index)
			{
				newNode->next = temp;
				newNode->prev = temp->prev;
				temp->prev->next = newNode;
				temp->prev = newNode;
				i=list->size+1;
			}
		}

	}
	list->size++;
	return list;
	
}

list_ptr_t list_insert_at_reference( list_ptr_t list, data_ptr_t data, list_ptr_t reference )
{
#ifdef DEBUG
printf( "\n list_insert_at_reference() is called\n" );
#endif

	Tlist newNode;
	newNode = (Tlist)malloc(sizeof(Tnode));
	assert(newNode);
	newNode->data = data;
	
	if(reference->size==0)
	{	
		newNode->prev = list->tail;
		list->tail = newNode;
		newNode->next = NULL;
	}
	else
	{
		Tlist temp = list->head;
		int i;
		for(i=1;i<=list->size;i++)
		{
			temp = temp->next;
			if(data == reference->head->data)
			{
				newNode->prev = reference->head->prev;
				reference->head->prev->next = newNode;
				reference->head->prev = newNode;
				newNode->next = reference->head;
			}
		}
		
	}
	list->size++;
	return list;
}

list_ptr_t list_insert_sorted( list_ptr_t list, data_ptr_t data )
{
#ifdef DEBUG
printf( "\n list_insert_sorted() is called\n" );
#endif

	
	Tlist newNode;
	newNode = (Tlist)malloc(sizeof(Tnode));
	assert(newNode);
	newNode->data = data;

	if (list->size == 0)
	{
		list->size=1;
		list->head=newNode;
		list->tail=newNode;
	}
	else
	{
		Tlist temp=list->head;
		int i=1;
		int size=list->size;
		data_ptr_t datas[size];
		
		//put the data of the list into an array		
		for(i=0;i<size;i++)
		{
			datas[i]=temp->data;
			temp=temp->next;
		}
		
		//sort the array
		qsort(datas,size,sizeof(int),*(list->compare));
		
		//put the sorted data back into list
		temp=list->head;
		for(i=0;i<size;i++)
		{
			temp->data=datas[i];
			temp=temp->next;
		}
		
		temp=list->head;
		for(i=1;i<=list->size+1;i++)
		{
			if((*(list->compare))(temp->data,data))
			{
				newNode->prev=temp->prev;
				newNode->next=temp;
				temp->prev->next=newNode;
				temp->prev=newNode;
				i=list->size+1;
			}
			temp=temp->next;
		}
	}


	list->size++;
	return list;
}

list_ptr_t list_free_at_index( list_ptr_t list, int index)
{
#ifdef DEBUG
printf( "\n list_free_at_index() is called\n" );
#endif

	Tlist temp;
	if(list->size == 0)
	{
		printf("The list is NULL!");
	}
	else if(list->size > 0)
	{
		if(index <= 0)
		{		
			temp = list->head;
			list->head = temp->next;
			temp->next->prev = NULL;	
			free(temp->data);
			free(temp);
		}
		else if(index > list->size)
		{
			temp = list->tail;			
			list->tail = temp->prev;
			temp->prev->next = NULL;
			free(temp->data);
			free(temp);
		}
		else
		{
			temp = list->head;
			int i;
			for(i=1;i<=list->size;i++)
			{
				temp = temp->next;
				if(i == index)
				{
					Tlist tempPrev = temp->prev;
					tempPrev->prev->next = temp;
					temp->prev = tempPrev->prev;
					free(tempPrev->data);
					free(tempPrev);
				}
			}

		}
			
		list->size--;
	}

	return list;
}

list_ptr_t list_free_at_reference( list_ptr_t list, list_ptr_t reference )
{
#ifdef DEBUG
printf( "\n list_free_at_reference() is called\n" );
#endif
	
	Tlist temp;
	if(reference->size==0)
	{
		temp = list->tail;			
		list->tail = temp->prev;
		temp->prev->next = NULL;
		free(temp->data);
		free(temp);		
	}
	else
	{
		temp = list->head;
		int i;
		for(i=1;i<=list->size;i++)
		{
			if(temp->data == reference->head->data)
			{
				temp->prev->next = temp->next;
				temp->next->prev = temp->prev;
				free(temp->data);
				free(temp);
			}
			temp = temp->next;
		}
		
	}
	list->size--;
	return list;
}

list_ptr_t list_free_data( list_ptr_t list, data_ptr_t data )
{
#ifdef DEBUG
printf( "\n list_free_data() is called\n" );
#endif

	Tlist temp;
	temp = list->head;
	int i;
	for(i=1;i<=list->size;i++)
	{
		if(temp->data == temp->data)
		{
			temp->prev->next = temp->next;
			temp->next->prev = temp->prev;
			free(temp->data);
			free(temp);
			i=list->size+1;
		}
		temp = temp->next;
	}
	list->size--;
	return list;
}

list_ptr_t list_remove_at_index( list_ptr_t list, int index)
{
#ifdef DEBUG
printf( "\n list_remove_at_index() is called\n" );
#endif

	Tlist temp;
	if(list->size == 0)
	{
		printf("The list is NULL!");
	}
	else if(list->size > 0)
	{
		if(index <= 0)
		{				
			temp = list->head;
			list->head = temp->next;
			temp->next->prev = NULL;	
		}
		else if(index > list->size)
		{
			temp = list->tail;			
			list->tail = temp->prev;
			temp->prev->next = NULL;
		}
		else
		{
			temp = list->head;
			int i;
			for(i=1;i<=list->size;i++)
			{
				temp = temp->next;
				if(i == index)
				{
					Tlist tempPrev = temp->prev;
					tempPrev->prev->next = temp;
					temp->prev = tempPrev->prev;
				}
			}

		}
			
		list->size--;
	}

	return list;
}

list_ptr_t list_remove_at_reference( list_ptr_t list, list_ptr_t reference )
{
#ifdef DEBUG
printf( "\n list_remove_at_reference() is called\n" );
#endif

		
	Tlist temp;
	if(reference->size==0)
	{
		temp = list->tail;			
		list->tail = temp->prev;
		temp->prev->next = NULL;	
	}
	else
	{
		temp = list->head;
		int i;
		for(i=1;i<=list->size;i++)
		{
			if(temp->data == reference->head->data)
			{
				temp->prev->next = temp->next;
				temp->next->prev = temp->prev;
			}
			temp = temp->next;
		}
		
	}
		
	list->size--;
	return list;
}

list_ptr_t list_remove_data( list_ptr_t list, data_ptr_t data )
{
#ifdef DEBUG
printf( "\n list_remove_data() is called\n" );
#endif

	Tlist temp;
	temp = list->head;
	int i;
	for(i=1;i<=list->size;i++)
	{
		if(data == temp->data)
		{
			temp->prev->next = temp->next;
			temp->next->prev = temp->prev;
			i=list->size+1;
		}
		temp = temp->next;
	}
		
	list->size--;
	return list;
}

list_ptr_t list_get_first_reference( list_ptr_t list )
{
#ifdef DEBUG
printf( "\n list_get_first_reference() is called\n" );
#endif

	list_ptr_t reference;
	reference = (list_ptr_t)malloc(sizeof(list_t));
	reference->size=0;
	reference->head=NULL;
	reference->tail=NULL;
	
	if(list->size == 0)
	{
		return NULL;
	}
	else
	{
		reference->size = 1;
		reference->head = list->head;
		reference->tail = list->head;
	}
	return reference;
}

list_ptr_t list_get_last_reference( list_ptr_t list )
{
#ifdef DEBUG
printf( "\n list_get_last_reference() is called\n" );
#endif

	list_ptr_t reference;
	reference = (list_ptr_t)malloc(sizeof(list_t));
	reference->size=0;
	reference->head=NULL;
	reference->tail=NULL;
	
	if(list->size == 0)
	{
		return NULL;
	}
	else
	{
		reference->size = 1;
		reference->head = list->tail;
		reference->tail = list->tail;
	}
	return reference;
}

list_ptr_t list_get_next_reference( list_ptr_t list, list_ptr_t reference )
{
#ifdef DEBUG
printf( "\n list_get_next_reference() is called\n" );
#endif

	Tlist temp = list->head;
	int i;
	for(i=1;i<=list->size;i++)
	{
		if(reference->head->data == temp->data)
		{
			reference->size = 1;
			reference->head = temp->next;
			reference->tail = temp->next;	
			i = list->size + 1;
		}
		temp = temp->next;
	}
	
	if(temp == NULL)
	{
		return NULL;
	}
	else
	{
	return reference;
	}
}

list_ptr_t list_get_previous_reference( list_ptr_t list, list_ptr_t reference )
{
#ifdef DEBUG
printf( "\n list_get_previous_reference() is called\n" );
#endif

	if(reference->head->data == list->head->data)
	{
		return NULL;
	}
	else
	{
		Tlist temp = list->head;
		int i;
		for(i=1;i<=list->size;i++)
		{
			if(reference->head->data == temp->data)
			{
				reference->size = 1;
				reference->head = temp->prev;
				reference->tail = temp->prev;	
				i = list->size + 1;
			}
		    temp = temp->next;
		}
		return reference;
	}
}

int list_get_index_of_reference( list_ptr_t list, list_ptr_t reference )
{
#ifdef DEBUG
printf( "\n list_get_index_of_reference() is called\n" );
#endif

	int index;
	if(reference->size == 0)
	{
		index = list->size;
	}
	else
	{
		Tlist temp = list->head;
		int i;
		for(i=1;i<=list->size;i++)
		{
			if(reference->head->data == temp->data)
			{
				index=i;
				i = list->size + 1;
			}	
			temp = temp->next;
		}
	}
	return index;	
}

list_ptr_t list_get_reference_at_index( list_ptr_t list, int index )
{
#ifdef DEBUG
printf( "\n list_get_reference_at_index() is called\n" );
#endif

	if(list->size==0)
	{
		return NULL;
	}
	else
	{
		list_ptr_t reference;
		reference = (list_ptr_t)malloc(sizeof(list_t));
		reference->size = 0;
		reference->head = NULL;
		reference->tail = NULL;
		
		if(index <= 0)
		{
			reference->size = 1;
			reference->head = list->head;
			reference->tail = list->head;
		}
		else if(index > list->size)
		{
			reference->size = 1;
			reference->head = list->tail;
			reference->tail = list->tail;
		}
		else
		{
			Tlist temp=list->head;
			int i;
			for(i=1;i<=list->size;i++)
			{
				if(i==index)
				{
					reference->size=1;
					reference->head=temp;
					reference->tail=temp;
					i=list->size+1;
				}
				temp=temp->next;
			}
		}
		return reference;
	}	
}

data_ptr_t list_get_data_at_reference( list_ptr_t list, list_ptr_t reference )
{
#ifdef DEBUG
printf( "\n list_get_data_at_reference() is called\n" );
#endif

	data_ptr_t getData;
	if(reference->size==0)
	{
		getData = list->tail->data;
	}
	else
	{
		Tlist temp=list->head;
		int i;
		for(i=1;i<=list->size;i++)
		{
			if(reference->head->data==temp->data)
			{
				getData = temp->data;
				i = list->size + 1;
			}
			temp=temp->next;
		}
	}
	return getData;
}

data_ptr_t list_get_data_at_index( list_ptr_t list, int index )
{
#ifdef DEBUG
printf( "\n list_get_data_at_index() is called\n" );
#endif

	data_ptr_t getData;
	if(index<=0)
	{
		getData =  list->head->data;
	}
	else if(index>list->size)
	{
		getData = list->tail->data;
	}
	else
	{
		Tlist temp=list->head;
		int i;
		for(i=1;i<=list->size+1;i++)
		{
			if(i==index)
			{
				getData = temp->data;
			}
			temp = temp->next;
		}		
	}
	return getData;
}

int list_get_index_of_data( list_ptr_t list, data_ptr_t data )
{
#ifdef DEBUG
printf( "\n list_get_index_of_data() is called\n" );
#endif

	Tlist temp=list->head;
	int i,index;
	for(i=1;i<=list->size+1;i++)
	{
		if(data==temp->data)
		{
			index=i;
			i=list->size+1;
		}
		else
		{
			index=-1;
		}
		temp=temp->next;
	}
	return index;

}

list_ptr_t list_get_reference_of_data( list_ptr_t list, data_ptr_t data )
{
#ifdef DEBUG
printf( "\n list_get_reference_of_data() is called\n" );
#endif

	list_ptr_t reference;
	reference = (list_ptr_t)malloc(sizeof(list_t));
	reference->size = 0;
	reference->head = NULL;
	reference->tail = NULL;
	
	Tlist temp=list->head;
	int i;
	for(i=1;i<=list->size+1;i++)
	{
		if(data==temp->data)
		{
			reference->head=temp;
			reference->tail=temp;
			i=list->size+1;
		}
		else
		{
			reference = NULL;
		}
		temp=temp->next;
	}
	return reference;
}
















