#include "list.h"

int list_errno;

struct list{
	struct list_node_t *head;
	int size;
	
	void (*copy_func)(void* *dest, void *src);
	void (*free_func)(void *element);
	int (*compare_func)(void *x, void *y);
	void (*print_func)(void *element);
}
	
struct list_node{
	list_node_t *next;
	list_node_t *prev;
	void *element;
}	
	
*list_ptr_t list_create 	( // callback functions
			  void (*element_copy)(element_ptr_t *dest_element, element_ptr_t src_element),
			  void (*element_free)(element_ptr_t *element),
			  int (*element_compare)(element_ptr_t x, element_ptr_t y),
			  void (*element_print)(element_ptr_t element)
			){
    list_t *new_list;				
	new_list=malloc(siztof(list_t));
	new_list->head=NULL;
	new_list->size=0;
	
	new_list->copy_func=element_copy;
	new_list->free_func=element_free;
	new_list->compare_func=element_compare;
	new_list->print_func=element_print;
	
	return new_list;
}
			
void list_free( list_ptr_t* list ){}
int list_size( list_ptr_t list ){}
list_ptr_t list_insert_at_index( list_ptr_t list, element_ptr_t element, int index){}
list_ptr_t list_remove_at_index( list_ptr_t list, int index){}
list_ptr_t list_free_at_index( list_ptr_t list, int index){}
list_node_ptr_t list_get_reference_at_index( list_ptr_t list, int index ){}
element_ptr_t list_get_element_at_index( list_ptr_t list, int index ){}
int list_get_index_of_element( list_ptr_t list, element_ptr_t element ){}
void list_print( list_ptr_t list ){}
