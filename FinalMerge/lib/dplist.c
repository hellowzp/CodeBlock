#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "dplist.h"

#ifdef DEBUG
	#define DEBUG_PRINTF(...) 									\
		do {											\
			printf("\nIn %s - function %s at line %d: ", __FILE__, __func__, __LINE__);	\
			printf(__VA_ARGS__);								\
		} while(0)
#else
	#define DEBUG_PRINTF(...) (void)0
#endif


#define DPLIST_ERR_HANDLER(condition,dplist_errno_value,...)\
	do {						\
		if ((condition))			\
		{					\
		  dplist_errno = dplist_errno_value;	\
		  DEBUG_PRINTF(#condition "failed");	\
		  return __VA_ARGS__;			\
		}					\
		dplist_errno = DPLIST_NO_ERROR;			\
	} while(0)

/*
 * The real definition of struct list / struct node
 */

dplist_t * dpl_sort(dplist_t *list);
struct dplist_node {
  dplist_node_t *prev, *next;
  void * element;
};

struct dplist {
  dplist_node_t *head;
  void * (*element_copy)(void * src_element);			  
  void (*element_free)(void ** element);
  int (*element_compare)(void * x, void * y);
};

dplist_t *dpl_create (// callback functions
			  void * (*element_copy)(void * src_element),
			  void (*element_free)(void ** element),
			  int (*element_compare)(void * x, void * y)
			  )
{
  dplist_t *list;
  list = malloc(sizeof(struct dplist));
  DPLIST_ERR_HANDLER((list==NULL),DPLIST_MEMORY_ERROR,NULL);
  list->head = NULL;  
  list->element_copy = element_copy;
  list->element_free = element_free;
  list->element_compare = element_compare;  
  return list;
}

void dpl_free(dplist_t ** list)
{
    // add your code here
  DPLIST_ERR_HANDLER((list==NULL || *list == NULL),DPLIST_INVALID_ERROR, (void)NULL);
	dplist_node_t *dummy, *temp;
	dummy = (*list)->head;

	while (NULL != dummy)
	{
		temp = dummy;
		dummy = dummy->next;
		(*list)->element_free(&temp->element);
		free(temp);
	}
	free(*list);
	*list = NULL;
}

dplist_t * dpl_insert_at_index(dplist_t * list, void * element, int index, bool insert_copy)
{
    // add your code hered
	
	dplist_node_t *ref_at_index, *list_node;
  DPLIST_ERR_HANDLER((list==NULL),DPLIST_INVALID_ERROR, NULL);
  list_node = malloc(sizeof(dplist_node_t));
  DPLIST_ERR_HANDLER((list_node==NULL),DPLIST_MEMORY_ERROR,NULL);

	if (insert_copy == true)
		list_node->element = list->element_copy(element);
	else
		list_node->element = element;

	if(list->head == NULL)
	{
		list_node->prev = NULL;
		list_node->next = NULL;
		list->head = list_node;
	} else if (index <=0)
	{
		list_node->prev = NULL;
		list_node->next = list->head;
		list->head->prev = list_node;
		list->head = list_node;
	} else
	{
		ref_at_index = dpl_get_reference_at_index( list, index);
    assert( ref_at_index != NULL);
		if (index < dpl_size(list))
		{
      list_node->prev = ref_at_index->prev;
      list_node->next = ref_at_index;
      ref_at_index->prev->next = list_node;
      ref_at_index->prev = list_node;
		} else
		{
      assert(ref_at_index->next == NULL);
      list_node->next = NULL;
      list_node->prev = ref_at_index;
      ref_at_index->next = list_node;    
		}
	}
	
	return list;

}

dplist_t * dpl_remove_at_index( dplist_t * list, int index, bool free_element)
{
  // add your code here
	dplist_node_t *dummy;
  DPLIST_ERR_HANDLER((list==NULL),DPLIST_INVALID_ERROR, NULL);
  if (list->head == NULL ) return list;
	dummy = dpl_get_reference_at_index(list, index);
	if(dummy->prev != NULL)
	{	
		dummy->prev->next = dummy->next;
		if (dummy->next != NULL)
			dummy->next->prev = dummy->prev;
	}
	else 
	{
		list->head = dummy->next;
		if (dummy->next != NULL)
			dummy->next->prev = dummy->prev;
	}

	if (free_element == true)
		list->element_free(&dummy->element);

	free(dummy);
	
	return list;
}

int dpl_size( dplist_t * list )
{
    // add your code here
	int count;
  dplist_node_t * dummy;
  DPLIST_ERR_HANDLER((list==NULL),DPLIST_INVALID_ERROR, 0);
  if (list->head == NULL ) return 0;

  for ( dummy = list->head, count = 0; dummy->next != NULL ; dummy = dummy->next, count++) ;
	return count+1;
	
}

dplist_node_t * dpl_get_reference_at_index( dplist_t * list, int index )
{
    // add your code here
  int count;
  dplist_node_t * dummy;
  DPLIST_ERR_HANDLER((list==NULL),DPLIST_INVALID_ERROR,NULL);
  if (list->head == NULL ) return NULL;
  for ( dummy = list->head, count = 0; dummy->next != NULL ; dummy = dummy->next, count++) 
  { 
    if (count >= index) return dummy;
  }  
  return dummy;  
}


void * dpl_get_element_at_index( dplist_t * list, int index )
{
    // add your code here
	dplist_node_t *dummy;
  DPLIST_ERR_HANDLER((list==NULL),DPLIST_INVALID_ERROR, (void*)NULL);
  if (list->head == NULL ) return (void *)NULL;
	dummy = dpl_get_reference_at_index(list, index);

	return dummy->element;

}

int dpl_get_index_of_element( dplist_t * list, void * element )
{
    // add your code here
	int count;
	dplist_node_t *dummy;
  DPLIST_ERR_HANDLER((list==NULL),DPLIST_INVALID_ERROR, 0);
	dummy = list->head;

	for (dummy = list->head, count = 0; dummy != NULL; dummy = dummy->next, count++)
	{
		if( list->element_compare(dummy->element, element) == 0) return count;
	}
	return -1;
}

// HERE STARTS THE EXTRA SET OF OPERATORS //

// ---- list navigation operators ----//
  
dplist_node_t * dpl_get_first_reference( dplist_t * list )
{
  DPLIST_ERR_HANDLER((list==NULL),DPLIST_INVALID_ERROR,NULL);
  if (list->head == NULL ) return NULL;
	return dpl_get_reference_at_index(list, 0);
}

dplist_node_t * dpl_get_last_reference( dplist_t * list )
{
  DPLIST_ERR_HANDLER((list==NULL),DPLIST_INVALID_ERROR,NULL);
  if (list->head == NULL ) return NULL;
	return dpl_get_reference_at_index(list, dpl_size(list));
}

dplist_node_t * dpl_get_next_reference( dplist_t * list, dplist_node_t * reference )
{
  DPLIST_ERR_HANDLER((list==NULL),DPLIST_INVALID_ERROR,NULL);
  if (list->head == NULL ) return NULL;
	if (reference == NULL) return NULL;

	for (int i = 0; i < dpl_size(list); i++)
	{
		if (reference == dpl_get_reference_at_index(list, i))
			return reference->next;
	}

	return NULL; //'reference' does not exist.
}

dplist_node_t * dpl_get_previous_reference( dplist_t * list, dplist_node_t * reference )
{
  DPLIST_ERR_HANDLER((list==NULL),DPLIST_INVALID_ERROR,NULL);
  if (list->head == NULL ) return NULL;
	if (reference == NULL) return NULL;

	for (int i = 0; i < dpl_size(list); i++)
	{
		if (reference == dpl_get_reference_at_index(list, i))
			return reference->prev;
	}

	return NULL; //'reference' does not exist.
}

// ---- search & find operators ----//  
  
void * dpl_get_element_at_reference( dplist_t * list, dplist_node_t * reference )
{
  DPLIST_ERR_HANDLER((list==NULL),DPLIST_INVALID_ERROR,NULL);
  if (list->head == NULL ) return NULL;
	if (reference == NULL) return dpl_get_element_at_index(list, dpl_size(list));

	dplist_node_t *dummy;
	dummy = list->head;
	while (dummy != NULL)
	{
		if (reference == dummy)
			return dummy->element;
		dummy = dummy->next;
	}

	return NULL;
}

dplist_node_t * dpl_get_reference_of_element( dplist_t * list, void * element )
{
  DPLIST_ERR_HANDLER((list==NULL),DPLIST_INVALID_ERROR,NULL);
  if (list->head == NULL ) return NULL;

	int ret_value = 0;
	if ((ret_value = dpl_get_index_of_element(list, element))  < 0)
		return NULL;
	
	return dpl_get_reference_at_index(list, ret_value);
}

int dpl_get_index_of_reference( dplist_t * list, dplist_node_t * reference )
{
  DPLIST_ERR_HANDLER((list==NULL),DPLIST_INVALID_ERROR, 0);
  if (list->head == NULL ) return -1;
	if (reference == NULL) return (dpl_size(list) - 1);

	int count;
	dplist_node_t *dummy;
	dummy = list->head;

	for (count = 0; count < dpl_size(list); count++, dummy = dummy->next)
	{
		if (dummy == reference) return count;	
	}
	
	return -1;
}
  
// ---- extra insert & remove operators ----//

dplist_t * dpl_insert_at_reference( dplist_t * list, void * element, dplist_node_t * reference, bool insert_copy )
{
  DPLIST_ERR_HANDLER((list==NULL),DPLIST_INVALID_ERROR, NULL);

	int index;
	if (reference == NULL) 
	{
		dpl_insert_at_index(list, element, dpl_size(list), insert_copy);
		return list;
	}
	index  = dpl_get_index_of_reference(list, reference);
	if (index < 0) return list;

	dpl_insert_at_index(list, element, index, insert_copy);

	return list;
}

dplist_t * dpl_insert_sorted( dplist_t * list, void * element, bool insert_copy )
{
  DPLIST_ERR_HANDLER((list==NULL),DPLIST_INVALID_ERROR, NULL);
	dpl_insert_at_index(list, element, 0, insert_copy);
	dpl_sort(list);
	return list;
}

dplist_t * dpl_remove_at_reference( dplist_t * list, dplist_node_t * reference, bool free_element )
{
  DPLIST_ERR_HANDLER((list==NULL),DPLIST_INVALID_ERROR, NULL);
	if (reference == NULL)
	{
		dpl_remove_at_index(list, dpl_size(list), free_element);
		return list;
	}
	int ret_value = 0;
	if ((ret_value = dpl_get_index_of_reference(list, reference)) == -1)
		return list;
	
	dpl_remove_at_index(list, ret_value, free_element);

	return list;
}

dplist_t * dpl_remove_element( dplist_t * list, void * element, bool free_element )
{
  DPLIST_ERR_HANDLER((list==NULL),DPLIST_INVALID_ERROR, NULL);
	int ret_value = 0;
	if ((ret_value = dpl_get_index_of_element(list, element)) == -1)
		return list;

	dpl_remove_at_index(list, ret_value, free_element);

	return list;
}
  
// ---- you can add your extra operators here ----//

dplist_t * dpl_sort(dplist_t *list)
{
  DPLIST_ERR_HANDLER((list==NULL),DPLIST_INVALID_ERROR, NULL);
  if (list->head == NULL ) return list;

	dplist_node_t *sorted = NULL;
	
	while (list->head != NULL)
	{
		dplist_node_t *temp = list->head;
		dplist_node_t ** trail = &sorted;

		list->head = list->head->next;

		while (! ((*trail == NULL) || (list->element_compare(temp->element, (*trail)->element) == -1)))
		{
			trail = & ((*trail)->next);
		}

		temp->next = *trail;
		if (*trail != NULL)
		{
			temp->prev = (*trail)->prev;
			(*trail)->prev = temp;
		}else	if(sorted != NULL){
			dplist_node_t *tail = sorted;
			while (tail->next != NULL)
			{
				tail= tail->next;
			}
			temp->prev = tail;
		}else{
			temp->prev = NULL;
		}

		*trail = temp;
	}

	list->head = sorted;
	return list;
}

