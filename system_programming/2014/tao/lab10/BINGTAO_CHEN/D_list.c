/*-----------------------------------------------------------------------------
		include files
------------------------------------------------------------------------------*/
#include <stdio.h>
#include "D_list.h"
#include <stdlib.h>


//----------------------------------------------------------------------------//
list_ptr_t list_insert_sorted( list_ptr_t list, data_ptr_t data )
//----------------------------------------------------------------------------//
{
	int i;
	data_ptr_t _data;
	list = sort_list(list);				//sort the list before inserting

	if (list == NULL) 				//return null, if the list does not exist
	{
		err = NULL_POINTER_ERR;
		return NULL;
	}

	for (i = 0; i < list_size(list) ; i++)
	{
		_data = list_get_data_at_index(list, i);
		if (list->funcs->data_compare(data, _data) <= 0)
		{
			list = list_insert_at_index( list, data, i);
			break;
		}
	}
	err = NONE;
	return list;

}

//----------------------------------------------------------------------------//
list_ptr_t sort_list(list_ptr_t list)
//----------------------------------------------------------------------------//
{

	int i, j;
	data_ptr_t data_med;
	list_ptr_t ptr_1, ptr_2;

	if (list == NULL) 				//return null, if the list does not exist
	{
		err = NULL_POINTER_ERR;
		return NULL;
	}

	if (list_size(list) == 1)
	{
		err = NONE;
		return list;
	}
	
	else
	{
		for (i = 0; i < list_size(list)-1; i++)
		{
			ptr_1 = list_get_reference_at_index(list, i);
			for (j = i + 1; j < list_size(list); j++ )
			{
				ptr_2 = list_get_reference_at_index(list, j);
				if ( list->funcs->data_compare(ptr_1->data, ptr_2->data) > 0) //if data at index i is bigger than at j then exchange
				{
					data_med = ptr_1->data;
					ptr_1->data = ptr_2->data;
					ptr_2->data = data_med;
				}
			}
		}
		err = NONE;
		return list;
	}
}

//----------------------------------------------------------------------------//
list_ptr_t list_get_reference_of_data( list_ptr_t list, data_ptr_t data )
//----------------------------------------------------------------------------//
{
	int index;
	if (list == NULL) 				//return null, if the list does not exist
	{
		err = NULL_POINTER_ERR;
		return NULL;
	}
	
	index = list_get_index_of_data( list, data );
	err = NONE;
	return list_get_reference_at_index(list, index);
	
}

//----------------------------------------------------------------------------//
int list_get_index_of_data( list_ptr_t list, data_ptr_t data )
//----------------------------------------------------------------------------//
{
	int match = 0;
	int index = 0;

	if (list == NULL) 				//return null, if the list does not exist
	{
		err = NULL_POINTER_ERR;
		return -1;
	}

	list = list_get_first_reference(list);		//move the pointer to the first element
	while(list != NULL)
	{	// if data match, list will be pointing to the matching element
		if( list->funcs->data_compare(list->data, data) == 0)
		{
			match = 1;
			break;
		}
		index++;
		list = list->next;
	}

	if (match)
	{
		err = NONE;
		return index;
	}
	else
	{
		err = NO_DATA_ERR;
		return -1;
	}
}

//----------------------------------------------------------------------------//
data_ptr_t list_get_data_at_index( list_ptr_t list, int index )
//----------------------------------------------------------------------------//
{
	if (list == NULL) 				//return null, if the list does not exist
	{
		err = NULL_POINTER_ERR;
		return NULL;
	}
	list_ptr_t reference = list_get_reference_at_index(list, index);
	err = NONE;
	return list_get_data_at_reference(list, reference);
}

//----------------------------------------------------------------------------//
data_ptr_t list_get_data_at_reference( list_ptr_t list, list_ptr_t reference )
//----------------------------------------------------------------------------//
{
	int exist_ref = 0;
	list_ptr_t last = list_get_last_reference(list);		

	if (list == NULL) 				//return null, if the list does not exist
	{
		err = NULL_POINTER_ERR;
		return NULL;
	}
	if (reference == NULL)
	{
		err = NONE;
		return last->data;
	}
	
	list = list_get_first_reference(list);		//move the pointer to the first element
	while(list != NULL)
	{
		if (list == reference)
		{
			exist_ref = 1;
			break;
		}
		list = list->next;
	}
	if (exist_ref)
	{
		err = NONE;
		return reference->data;
	}
	else
	{
		err = NONE;
		return last->data;
	}

	
}

//----------------------------------------------------------------------------//
list_ptr_t list_get_reference_at_index( list_ptr_t list, int index )
//----------------------------------------------------------------------------//
{
	int i;
	if (list == NULL) 				//return null, if the list does not exist
	{
		err = NULL_POINTER_ERR;
		return NULL;
	}
	
	if (index > list_size(list) - 1)
	{
		list = list_get_last_reference(list);		// move the pointer to the end of the list
		err = NONE;
		return list;
	}
	else if (index <= 0)
	{
		list = list_get_first_reference(list);		//move the pointer to the first element
		err = NONE;
		return list;
	}
	else
	{
		list = list_get_first_reference(list);		//move the pointer to the first element
		for (i = 0; i < index; i++)
		{
			list = list->next;
		}
		err = NONE;
		return list;
	}
}

//----------------------------------------------------------------------------//
int list_get_index_of_reference( list_ptr_t list, list_ptr_t reference )
//----------------------------------------------------------------------------//
{
	int index = 0;
	int exist_ref = 0;

	if (list == NULL) 				//return -1, if the list does not exist
	{
		err = NULL_POINTER_ERR;
		return -1;
	}
	if (reference == NULL)
	{	
		err = NONE;	
		 return list_size(list)-1;
	}
	
	list = list_get_first_reference(list);		//move the pointer to the first element
	while(list != NULL)
	{
		if (list == reference)
		{
			exist_ref = 1;
			break;
		}
		list = list->next;
		index++;
	}

	if (exist_ref)
	{
		err = NONE;
		return index;
	}
	else
	{
		err = NONE_EXIST_REF_ERR;
		return -1;
	}
	
}

//----------------------------------------------------------------------------//
list_ptr_t list_get_previous_reference( list_ptr_t list, list_ptr_t reference )
//----------------------------------------------------------------------------//
{
	int exist_ref = 0;
	list = list_get_first_reference(list);		//move the pointer to the first element

	if (list == NULL) 				//return null, if the list does not exist
	{
		err = NULL_POINTER_ERR;
		return NULL;
	}
	if (reference == NULL) 
	{
		err = NONE;
		return NULL;
	}		
	while(list != NULL)
	{
		if (list == reference)
		{
			exist_ref = 1;
			break;
		}
		list = list->next;
	}
	if (exist_ref)
	{
		if (reference->previous == NULL)			//if the element at reference is the last one, return null
		{
			err = NON_PREVIOUS_REF_ERR;
			return NULL;
		}
		else						//if the element at reference has next, return the next;
		{
			err = NONE;
			return reference->previous;
		}
	}
	else							//if reference does exist, return null
	{
		err = NONE_EXIST_REF_ERR;
		return NULL;
	}
	
	

}

//----------------------------------------------------------------------------//
list_ptr_t list_get_next_reference( list_ptr_t list, list_ptr_t reference )
//----------------------------------------------------------------------------//
{
	int exist_ref = 0;

	list = list_get_first_reference(list);		//move the pointer to the first element

	if (list == NULL) 				//return null, if the list does not exist
	{
		err = NULL_POINTER_ERR;
		return NULL;
	}
	if (reference == NULL) 
	{
		err = NONE;	
		return NULL;
	}		
	while(list != NULL)
	{
		if (list == reference)
		{
			exist_ref = 1;
			break;
		}
		list = list->next;
	}
	if (exist_ref)
	{
		if (reference->next == NULL)			//if the element at reference is the last one, return null
		{
			err = NON_NEXT_REF_ERR;
			return NULL;
		}
		else						//if the element at reference has next, return the next;
		{
			err = NONE;
			return reference->next;
		}
	}
	else							//if reference does exist, return null
	{
		err = NONE_EXIST_REF_ERR;
		return NULL;
	}
	
}

//----------------------------------------------------------------------------//
list_ptr_t list_free_data( list_ptr_t list, data_ptr_t data )
//----------------------------------------------------------------------------//
{
	int match = 0;
	list_ptr_t ptr = list;
	list = list_get_first_reference(list);		//move the pointer to the first element

	if (list == NULL) 
	{
		err = NULL_POINTER_ERR;
		return NULL;
	}
	
	while(list != NULL)
	{	// if data match, list will be pointing to the matching element
		if( list->funcs->data_compare(list->data, data) == 0)
		{
			match = 1;
			break;
		}
		list = list->next;
	}
	if (match)
	{
		if (list_size(list) == 1)
		{
			list_free(list);
			err = NONE;
			return NULL;
		}
		else
		{
			if (list->previous == NULL)	//remove element when it is in the beginning
			{
				ptr = list->next;
				ptr->funcs->data_destory(list->data);
				free(list);
				ptr->previous = NULL;
				err = NONE;
				return ptr;
			} 
			else if (list->next == NULL)	//remove element when it is in the end
			{
				ptr = list->previous;
				ptr->funcs->data_destory(list->data);
				free(list);
				ptr->next = NULL;
				err = NONE;
				return ptr;
			}
			else
			{
				ptr = list->previous;
				ptr->next = list->next;
				list->next->previous = ptr;
				ptr->funcs->data_destory(list->data);
				free(list);
				err = NONE;
				return ptr;
			}
		}
	}
	else	// if there is no match data, return the original list pointer and a error message
	{
		err = NO_DATA_ERR;
		err = NONE;
		return ptr;			//if the reference does not exist, return the list pointer back
	}
}

//----------------------------------------------------------------------------//
list_ptr_t list_free_at_reference( list_ptr_t list, list_ptr_t reference )
//----------------------------------------------------------------------------//
{
	int exist_ref = 0;
	list_ptr_t ptr = list;
	
	if (list == NULL) 
	{
		err = NULL_POINTER_ERR;
		return NULL;
	}
		
	if (reference == NULL)
	{
		list = list_get_last_reference(list);		// move the pointer to the end of the list
		if (list_size(list) == 1)
		{
			list_free(list);	//if only one element left, free the list
			err = NONE;
			return NULL;			
		}
		else
		{
			list = list->previous;
			list->funcs->data_destory(list->next->data);
			free(list->next);
			list->next = NULL;
			err = NONE;
			return list;
		}
	}
	else
	{
		list = list_get_first_reference(list);		//move the pointer to the first element
		
		while(list != NULL)
		{
			if (list == reference)
			{
				exist_ref = 1;
				break;
			}
			list = list->next;
		}
		if (exist_ref)
		{
			if (list_size(list) == 1)
			{
				list_free(list);	//if only one element left, free the list
				err = NONE;
				return NULL;			
			}
			else
			{
				if (reference->previous == NULL)		//free when the element is in the beginning
				{
					ptr = reference->next;
					ptr->funcs->data_destory(reference->data);
					free(reference);
					ptr->previous =NULL;
					err = NONE;
					return ptr;
				}
				else if(reference->next == NULL)		//free when the element is in the end
				{
					ptr = reference->previous;
					ptr->funcs->data_destory(reference->data);
					free(reference);
					ptr->next = NULL;
					err = NONE;
					return ptr;
				}
				else
				{
					ptr = reference->previous;
					ptr->next = reference->next;
					reference->next->previous = ptr;
					ptr->funcs->data_destory(reference->data);
					free(reference);
					err = NONE;
					return ptr; 
				}
				
			}
		}
		else
		{
			err = NONE_EXIST_REF_ERR;
			return ptr;			//if the reference does not exist, return the list pointer back
		}
	}
}

//----------------------------------------------------------------------------//
list_ptr_t list_free_at_index( list_ptr_t list, int index)
//----------------------------------------------------------------------------//
{
	list_ptr_t reference = list_get_reference_at_index(list,index);
	list_ptr_t ptr = list_free_at_reference(list, reference);
	err = NONE;
	return ptr;//list_free_at_reference(list, reference);
	
}

//----------------------------------------------------------------------------//
list_ptr_t list_insert_at_reference( list_ptr_t list, data_ptr_t data, list_ptr_t reference )
//----------------------------------------------------------------------------//
{
	int ref_exist = 0;					//set the look up flag
	int index = 0;						// the index of reference
	list_ptr_t new_element;
	list_ptr_t ptr = list;

	new_element = (list_ptr_t)malloc(sizeof(list_t)); 	//allocate the new element
	new_element->funcs = list->funcs;
	new_element->data = list->funcs->data_copy(data);

	if (list == NULL) 
	{
		err = NULL_POINTER_ERR;
		return NULL;
	}
	if (reference == NULL)
	{
		list = list_get_last_reference(list);		// move the pointer to the end of the list
		new_element->next = NULL;
		new_element->previous = list;
		list->next = new_element;
		err = NONE;
		return new_element;
	}
	else
	{
		list = list_get_first_reference(list);		//move the pointer to the first element
		while(list != NULL)
		{	
			if (list == reference) 
			{
				ref_exist = 1;
				break;
			}
			list = list->next;
			index++;
		}

		if(ref_exist)
		{
			if (index == 0)
			{	//insert the new element in the begianning of list
				list = list_get_first_reference(list);	//move the pointer to the first element
				new_element->previous = NULL;
				new_element->next = list;
				list->previous = new_element;
			}
			else
			{	//insert the new element into the position of reference
				new_element->next = reference;
				new_element->previous = reference->previous;
				reference->previous->next = new_element;
				reference->previous = new_element;
			}
			err = NONE;
			return new_element;
		}
		else
		{
			err = NONE_EXIST_REF_ERR;
			return ptr;			//if the reference does not exist, return the list pointer back
		}
	}
}

//----------------------------------------------------------------------------//
list_ptr_t list_insert_at_index( list_ptr_t list, data_ptr_t data, int index)
//----------------------------------------------------------------------------//
{
	list_ptr_t ptr_1;					//declear two list pointers that can help insert
	list_ptr_t ptr_2;
	list_ptr_t new_element;					//declear the new element
	new_element = (list_ptr_t)malloc(sizeof(list_t)); 	//allocate the new element
	new_element->funcs = list->funcs;
	new_element->data = list->funcs->data_copy(data);	//deep copy the data into the new element
	int i;
	
	
	if (list == NULL) 
	{
		err = NULL_POINTER_ERR;
		return NULL;
	}

	if (index > list_size(list) - 1)   			//check if the index is out of range
	{
		list = list_get_last_reference(list);		// move the pointer to the end of the list
		new_element->next = NULL;
		new_element->previous = list;
		list->next = new_element;
	}

	else if(index <= 0)			// new element is inserted in the begainning of list when index is 0 or negative
	{
		list = list_get_first_reference(list);	//move the pointer to the first element
		new_element->previous = NULL;
		new_element->next = list;
		list->previous = new_element;
	}
	else
	{
		list = list_get_first_reference(list);
		ptr_2 = list->next;
		ptr_1 = list;
		for (i = 0; i < (index -1) ;i++)		//move prt_2 to the element at the index and ptr_1 the element before it
		{
			ptr_2 = ptr_2->next;
			ptr_1 = ptr_1->next;
		}
		new_element->next = ptr_2;
		new_element->previous = ptr_1;
		ptr_2->previous = new_element;
		ptr_1->next = new_element; 
	}
	err = NONE;
	return new_element;
}

//----------------------------------------------------------------------------//
void list_free( list_ptr_t list )
//----------------------------------------------------------------------------//
{
	if (list == NULL) 
	{
		err = NULL_POINTER_ERR;
	}
	sidefuncs * funcs = list->funcs;		
					
	list = list_get_first_reference(list);		
	while(list->next != NULL)
	{
		list = list->next;
		funcs->data_destory(list->previous->data);  	//free the data of the previous element
		free(list->previous);
	}
	
	funcs->data_destory(list->data);		//free the data in last element
	free(list);					//free the last element
	
	free(funcs);					//free the malloc funcs whos stores all the callback function pointers
}

//----------------------------------------------------------------------------//
list_ptr_t list_alloc (destory_data_func *data_destory,compare_data_func *data_compare,data_copy_func *data_copy, data_ptr_t data)
//----------------------------------------------------------------------------//
{
	list_ptr_t list = (list_ptr_t)malloc(sizeof(list_t));
	list->funcs = (sidefuncs *)malloc(sizeof(sidefuncs));
	list->funcs->data_destory = data_destory;
	list->funcs->data_copy = data_copy;
	list->funcs->data_compare = data_compare;
	list->next = NULL;
	list->previous = NULL;
	list->data = data_copy(data);	

	err = NONE;
	return list;
}

//----------------------------------------------------------------------------//
int list_size( list_ptr_t list )
//----------------------------------------------------------------------------//
{
	int count = 0;
	if (list == NULL) 
	{
		err = NULL_POINTER_ERR;
		return 0;
	}
	list = list_get_first_reference(list);
	while (list != NULL)
	{
		count++;
		list = list->next;
	}
	err = NONE;
	return count;
}

//----------------------------------------------------------------------------//
list_ptr_t list_get_first_reference( list_ptr_t list )
//----------------------------------------------------------------------------//
{
	if (list == NULL) 
	{
		err = NULL_POINTER_ERR;
		return NULL;
	}
	else
	{	
		while (list->previous != NULL)
		{
			list = list->previous;
		}
		err = NONE;
		return list;
	}
}

//----------------------------------------------------------------------------//
list_ptr_t list_get_last_reference( list_ptr_t list )
//----------------------------------------------------------------------------//
{
	if (list == NULL) 
	{
		err = NULL_POINTER_ERR;
		return NULL;
	}
	else
	{	
		while (list->next != NULL)
		{
			list = list->next;
		}
		err = NONE;
		return list;
	}
}


//-----------------------------------------------------------------------------//
void error_handler(err_code err)
//-----------------------------------------------------------------------------//
{
	switch (err)
	{
		case NONE: 
			break;
		case NULL_POINTER_ERR:
			printf("error:no pointer \n");
			break;
		case NONE_EXIST_REF_ERR:
			printf("no such reference \n");
			break;
		case NON_NEXT_REF_ERR:
			printf(" no next reference \n");
			break;
		case NON_PREVIOUS_REF_ERR:
			printf(" no previous reference\n");
			break;
		case NO_DATA_ERR:
			printf(" no data  \n");
			break;
		default:
			printf(" unknow error \n");
			
		
	}			
}
