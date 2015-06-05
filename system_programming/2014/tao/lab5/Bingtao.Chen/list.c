#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "list.h"



int list_erro;
// Returns a pointer to a newly-allocated list.
list_ptr_t list_alloc ( compareFunc* compare )
{	
	list_erro=0;	
	list_ptr_t list;
	 
	list = (list_ptr_t)malloc(sizeof(list_t));
	if(list==NULL)
	{
	   list_erro=1;
	   assert(1==0);
	}
	list->size=0;
	list->first=NULL;
	list->last=NULL;
	list->compare=compare;
	
	return list;

}




// Every element of 'list' needs to be deleted (free memory) and finally the list itself needs to be deleted (free all memory)
void list_free( list_ptr_t list )
{

	Tlist temp = list->first;
	int i;
	list_erro=0;
	for(i=1;i<=list->size;i++)
	{
		free(temp);
		temp = temp->next;
	}
	free(list);
}


// Function:Returns the number of elements in 'list'.
int list_size( list_ptr_t list )
{
	   list_erro=0;
	   if(list->size==0)
	   {
		   list_erro=5;
		   return 0;
	   }
	   return list->size;
}



/* Inserts a new element containing 'data' in 'list' at position 'index'  and returns a pointer to the new list. 
If 'index' is 0 or negative, the element is inserted at the start of 'list'. 
If 'index' is bigger than the number of elements in 'list', the element is inserted at the end of 'list'.*/

list_ptr_t list_insert_at_index( list_ptr_t list, data_ptr_t data, int index)
{
	list_erro=0;
	Tlist newNode;
	newNode = (Tlist)malloc(sizeof(Tnode));
	if(newNode==NULL)
	{
		list_erro=1;
		assert(newNode);
	}
	
	newNode->data = data;
	if(index <= 0)
	{	
		newNode->next = list->first;
		list->first = newNode;
		newNode->prev = NULL;
	}
	else if(index > list->size)
	{	
		newNode->prev = list->last;
		list->last = newNode;
		newNode->next = NULL;
	}
	else
	{
		Tlist temp = list->first;
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


// Inserts a new element containing 'data' in the 'list' at position 'reference' 
// and returns a pointer to the new list. If 'reference' is NULL, the element is inserted at the end of 'list'.
list_ptr_t list_insert_at_reference( list_ptr_t list, data_ptr_t data, list_ptr_t reference )
{

	Tlist newNode;
	newNode = (Tlist)malloc(sizeof(Tnode));
	if(newNode==NULL)
	{
		list_erro=1; 
		assert(newNode);
	}

	newNode->data = data;
	
	if(reference->size==0)
	{	
		newNode->prev = list->last;
		list->last = newNode;
		newNode->next = NULL;
	}
	else
	{
		Tlist temp = list->first;
		int i;
		for(i=1;i<=list->size;i++)
		{
			temp = temp->next;
			if(data == reference->first->data)
			{
				newNode->prev = reference->first->prev;
				reference->first->prev->next = newNode;
				reference->first->prev = newNode;
				newNode->next = reference->first;
			}
		}
		
	}
	list->size++;
	return list;
}


// Inserts a new element containing 'data' in the sorted 'list' and returns a pointer to the new list.
// The 'list' must be sorted before calling this function.
list_ptr_t list_insert_sorted( list_ptr_t list, data_ptr_t data )
{
	list_erro=0;
	Tlist newNode;
	newNode = (Tlist)malloc(sizeof(Tnode));
    if(newNode==NULL)
	{
		list_erro=1; 
		assert(newNode);
	}
	newNode->data = data;

	if (list->size == 0)
	{
		list->size=1;
		list->first=newNode;
		list->last=newNode;
	}
	else
	{
		Tlist temp=list->first;
		int i=1;
		int size=list->size;
		data_ptr_t datas[size];
		
		for(i=0;i<size;i++)                                    		    //put the data of the list into an array		
		{
			datas[i]=temp->data;
			temp=temp->next;
		}
		
		qsort(datas,size,sizeof(int),*(list->compare));                 //sort the array
		
		temp=list->first;                                               //put the sorted data back into list
		for(i=0;i<size;i++)
		{
			temp->data=datas[i];
			temp=temp->next;
		}
		
		temp=list->first;
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


/* Deletes the element at index 'index' in 'list'. 
A free() is called on the data pointer of the element to free any dynamic memory allocated to the data pointer. 
If 'index' is 0 or negative, the first element is deleted. 
If 'index' is bigger than the number of elements in 'list', the data of the last element is deleted.*/
list_ptr_t list_free_at_index( list_ptr_t list, int index)
{
    list_erro=0;
	Tlist temp;
	if(list->size == 0)
	{
		printf("The list is NULL!");
	}
	else if(list->size > 0)
	{
		if(index <= 0)
		{		
			temp = list->first;
			list->first = temp->next;
			temp->next->prev = NULL;	
			free(temp->data);
			free(temp);
		}
		else if(index > list->size)
		{
			temp = list->last;			
			list->last = temp->prev;
			temp->prev->next = NULL;
			free(temp->data);
			free(temp);
		}
		else
		{
			temp = list->first;
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


/*Deletes the element with position 'reference' in 'list'. 
A free() is called on the data pointer of the element to free any dynamic memory allocated to the data pointer. 
If 'reference' is NULL, the data of the last element is deleted.*/
list_ptr_t list_free_at_reference( list_ptr_t list, list_ptr_t reference )
{

	list_erro=0;
	Tlist temp;
	if(reference->size==0)
	{
		temp = list->last;			
		list->last = temp->prev;
		temp->prev->next = NULL;
		free(temp->data);
		free(temp);		
	}
	else
	{
		temp = list->first;
		int i;
		for(i=1;i<=list->size;i++)
		{
			if(temp->data == reference->first->data)
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


/*Finds the first element in 'list' that contains 'data' and deletes the element from 'list'.
 A free() is called on the data pointer of the element to free any dynamic memory allocated to the data pointer.*/
list_ptr_t list_free_data( list_ptr_t list, data_ptr_t data )
{

    list_erro=0;
	Tlist temp;
	temp = list->first;
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


/* Removes the element at index 'index' from 'list'. NO free() is called on the data pointer of the element. 
If 'index' is 0 or negative, the first element is removed. 
If 'index' is bigger than the number of elements in 'list', the data of the last element is removed.*/
list_ptr_t list_remove_at_index( list_ptr_t list, int index)
{

    list_erro=0;
	Tlist temp;
	if(list->size == 0)
	{
		printf("The list is NULL!");
	}
	else if(list->size > 0)
	{
		if(index <= 0)
		{				
			temp = list->first;
			list->first = temp->next;
			temp->next->prev = NULL;	
		}
		else if(index > list->size)
		{
			temp = list->last;			
			list->last = temp->prev;
			temp->prev->next = NULL;
		}
		else
		{
			temp = list->first;
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


/* Removes the element with reference 'reference' in 'list'. 
NO free() is called on the data pointer of the element. If 'reference' is NULL, the data of the last element is removed.*/
list_ptr_t list_remove_at_reference( list_ptr_t list, list_ptr_t reference )
{


	list_erro=0;	
	Tlist temp;
	if(reference->size==0)
	{
		temp = list->last;			
		list->last = temp->prev;
		temp->prev->next = NULL;	
	}
	else
	{
		temp = list->first;
		int i;
		for(i=1;i<=list->size;i++)
		{
			if(temp->data == reference->first->data)
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


// Finds the first element in 'list' that contains 'data' and removes the element from 'list'. 
//NO free() is called on the data pointer of the element.
list_ptr_t list_remove_data( list_ptr_t list, data_ptr_t data )
{

    list_erro=0;
	Tlist temp;
	temp = list->first;
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


// Returns a reference to the first element of 'list'. If the list is empty, NULL is returned.
list_ptr_t list_get_first_reference( list_ptr_t list )
{

    list_erro=0;
	list_ptr_t reference;
	reference = (list_ptr_t)malloc(sizeof(list_t));
	reference->size=0;
	reference->first=NULL;
	reference->last=NULL;
	
	if(list->size == 0)
	{
		return NULL;
		list_erro=5;
	}
	else
	{
		reference->size = 1;
		reference->first = list->first;
		reference->last = list->first;
	}
	return reference;
}


// Returns a reference to the last element of 'list'. If the list is empty, NULL is returned.
list_ptr_t list_get_last_reference( list_ptr_t list )
{

    list_erro=0;
	list_ptr_t reference;
	reference = (list_ptr_t)malloc(sizeof(list_t));
	reference->size=0;
	reference->first=NULL;
	reference->last=NULL;
	
	if(list->size == 0)
	{
		return NULL;
		list_erro=4;
	}
	else
	{
		reference->size = 1;
		reference->first = list->last;
		reference->last = list->last;
	}
	return reference;
}


/* Returns a reference to the next element of the element with reference 'reference' in 'list'. 
If the next element doesn't exists, NULL is returned.*/
list_ptr_t list_get_next_reference( list_ptr_t list, list_ptr_t reference )
{
    list_erro=0;
	Tlist temp = list->first;
	int i;
	for(i=1;i<=list->size;i++)
	{
		if(reference->first->data == temp->data)
		{
			reference->size = 1;
			reference->first = temp->next;
			reference->last = temp->next;	
			i = list->size + 1;
		}
		temp = temp->next;
	}
	
	if(temp == NULL)
	{
		return NULL;
		list_erro=4;
	}
	else
	{
	return reference;
	}
}


// Returns a reference to the previous element of the element with reference 'reference' in 'list'. If the previous element doesn't exists, NULL is returned.
list_ptr_t list_get_previous_reference( list_ptr_t list, list_ptr_t reference )
{

    list_erro=0;
	if(reference->first->data == list->first->data)
	{
		return NULL;
	}
	else
	{
		Tlist temp = list->first;
		int i;
		for(i=1;i<=list->size;i++)
		{
			if(reference->first->data == temp->data)
			{
				reference->size = 1;
				reference->first = temp->prev;
				reference->last = temp->prev;	
				i = list->size + 1;
			}
		    temp = temp->next;
		}
		return reference;
	}
}


// Returns the index of the element in the 'list' with reference 'reference'. If 'reference' is NULL, the index of the last element is returned.
int list_get_index_of_reference( list_ptr_t list, list_ptr_t reference )
{
    list_erro=0;
	int index;
	if(reference->size == 0)
	{
		index = list->size;
	}
	else
	{
		Tlist temp = list->first;
		int i;
		for(i=1;i<=list->size;i++)
		{
			if(reference->first->data == temp->data)
			{
				index=i;
				i = list->size + 1;
			}	
			temp = temp->next;
		}
	}
	return index;	
}


// Returns a reference to the element with index 'index' in 'list'. If 'index' is 0 or negative, a reference to the first element is returned. 
//If 'index' is bigger than the number of elements in 'list', a reference to the last element is returned. If the list is empty, NULL is returned.
list_ptr_t list_get_reference_at_index( list_ptr_t list, int index )
{
     list_erro=0;

	if(list->size==0)
	{
		return NULL;
	}
	else
	{
		list_ptr_t reference;
		reference = (list_ptr_t)malloc(sizeof(list_t));
		reference->size = 0;
		reference->first = NULL;
		reference->last = NULL;
		
		if(index <= 0)
		{
			reference->size = 1;
			reference->first = list->first;
			reference->last = list->first;
		}
		else if(index > list->size)
		{
			reference->size = 1;
			reference->first = list->last;
			reference->last = list->last;
		}
		else
		{
			Tlist temp=list->first;
			int i;
			for(i=1;i<=list->size;i++)
			{
				if(i==index)
				{
					reference->size=1;
					reference->first=temp;
					reference->last=temp;
					i=list->size+1;
				}
				temp=temp->next;
			}
		}
		return reference;
	}	
}


// Returns the data pointer contained in the element with reference 'reference' in 'list'. If 'reference' is NULL, the data of the last element is returned.

data_ptr_t list_get_data_at_reference( list_ptr_t list, list_ptr_t reference )
{
    list_erro=0;
	data_ptr_t getData;
	if(reference->size==0)
	{
		getData = list->last->data;
	}
	else
	{
		Tlist temp=list->first;
		int i;
		for(i=1;i<=list->size;i++)
		{
			if(reference->first->data==temp->data)
			{
				getData = temp->data;
				i = list->size + 1;
			}
			temp=temp->next;
		}
	}
	return getData;
}

// Returns the data contained in the element with index 'index' in 'list'. If 'index' is 0 or negative, the data of the first element is returned. 
//If 'index' is bigger than the number of elements in 'list', the data of the last element is returned
data_ptr_t list_get_data_at_index( list_ptr_t list, int index )
{

	data_ptr_t getData;
	if(index<=0)
	{
		getData =  list->first->data;
	}
	else if(index>list->size)
	{
		getData = list->last->data;
	}
	else
	{
		Tlist temp=list->first;
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


// Returns an index to the first element in 'list' containing 'data'.  If 'data' is not found in 'list', -1 is returned.
int list_get_index_of_data( list_ptr_t list, data_ptr_t data )
{

    list_erro=0;
	Tlist temp=list->first;
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


// Returns a reference to the first element in 'list' containing 'data'. If 'data' is not found in 'list', NULL is returned.
list_ptr_t list_get_reference_of_data( list_ptr_t list, data_ptr_t data )
{

    list_erro=0;
	list_ptr_t reference;
	reference = (list_ptr_t)malloc(sizeof(list_t));
	reference->size = 0;
	reference->first = NULL;
	reference->last = NULL;
	
	Tlist temp=list->first;
	int i;
	for(i=1;i<=list->size+1;i++)
	{
		if(data==temp->data)
		{
			reference->first=temp;
			reference->last=temp;
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
















