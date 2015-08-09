#include "list.h"
#include "util.h"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <limits.h>

int list_errno;

struct list_node {
    list_node_ptr_t prev, next;
    element_ptr_t element;
};

/**
 * @brief The list struct
 * copy_func should first allocate memory for the destination pointer
 * free_func and print_func should do nothing if NULL parameter passed
 */
struct list{
    list_node_ptr_t head;
    unsigned int size;
    void (*copy_func)(element_ptr_t* dest, element_ptr_t src);
    void (*free_func)(element_ptr_t* element);
    int (*compare_func)(element_ptr_t x, element_ptr_t y);
    void (*print_func)(element_ptr_t element);
};


list_ptr_t list_create( // callback functions
			  void (*element_copy)(element_ptr_t *dest_element, element_ptr_t src_element),
			  void (*element_free)(element_ptr_t *element),
			  int (*element_compare)(element_ptr_t x, element_ptr_t y),
              void (*element_print)(element_ptr_t element) )
{
    assert( element_copy && element_free && element_print && element_compare);
    list_ptr_t list;
    MALLOC( list, sizeof(list_t));
    list->head=NULL;
    list->size=0;	
    list->copy_func = element_copy;
    list->free_func = element_free;
    list->compare_func = element_compare;
    list->print_func = element_print;	
    return list;
}
			
void list_free( list_ptr_t* list ) {
    assert(list && *list);
    list_node_ptr_t node = (*list)->head;
    while(node!=NULL) {
        (*list)->free_func ( &(node->element) );
        list_node_ptr_t next_node = node->next;
        free(node);
        node = next_node;
    }
    free(*list);
    *list = NULL;
    DEBUG_PRINTF("%s\n","List free succeed!");
}

int list_size( list_ptr_t list ){
    assert(list);
    return list->size;
}

list_ptr_t list_insert_at_index( list_ptr_t list, element_ptr_t element, int index) {
    assert(list && element);
    int size = list_size(list);
    list_node_ptr_t new_node;
    MALLOC(new_node, sizeof(list_node_t) );
    list->copy_func( &new_node->element, element);

    if(index<=0) {
        list_node_ptr_t first_node = list->head;
        if(first_node!=NULL) {
            first_node->prev = new_node;
            new_node->next = first_node;
            new_node->prev = NULL;
            list->head = new_node;
        } else {
            list->head = new_node;
            new_node->prev = NULL;
            new_node->next = NULL;
        }
    } else if(index<size) {
        list_node_ptr_t prev_node = list_get_reference_at_index(list, index-1);
        new_node->prev = prev_node;
        new_node->next = prev_node->next;
        prev_node->next = new_node;
        prev_node->next->prev = new_node;
    } else {
        list_node_ptr_t last_node = list_get_reference_at_index(list, size-1);
        if(last_node!=NULL) {
            last_node->next = new_node;
            new_node->prev = last_node;
            new_node->next = NULL;
            DEBUG_PRINTF("list add element in the end");
        } else {
            DEBUG_PRINTF("empty list add element in the end");
            list->head = new_node;
            new_node->prev = NULL;
            new_node->next = NULL;
        }
    }

    list->size ++;
    return list;
}

list_ptr_t list_insert_at_front( list_ptr_t list, element_ptr_t element ){
    return list_insert_at_index( list, element, -1);
}

list_ptr_t list_insert_at_end( list_ptr_t list, element_ptr_t element ){
    return list_insert_at_index( list, element, INT_MAX);
}

list_ptr_t list_remove_at_index( list_ptr_t list, int index) {
    assert(list);
    if(list_size(list)==0) return NULL;

    list_node_ptr_t node = list_get_reference_at_index(list, index);
    if( node->prev ){
        node->prev->next = node->next;
    } else {
        list->head = node->next;
    }
    if( node->next ){
        node->next->prev = node->prev;
    }

    list->size--;
    return list;
}

list_ptr_t list_free_at_index( list_ptr_t list, int index) {
    assert(list);
    if(list_size(list)==0) return NULL;

    list_node_ptr_t node = list_get_reference_at_index(list, index);
    if( node->prev ){
        node->prev->next = node->next;
    } else {
        list->head = node->next;
    }
    if( node->next ){
        node->next->prev = node->prev;
    }
    list->free_func( &node->element );

    list->size--;
    return list;
}

list_node_ptr_t list_get_reference_at_index( list_ptr_t list, int index ){
    assert(list);
    list_node_ptr_t node = list->head;
    int i = 0;
    while(i<index && node!=NULL) {
        node = node->next;
        i++;
    }
    return node;
}

element_ptr_t list_get_element_at_index( list_ptr_t list, int index ){
    assert(list);
    list_node_ptr_t node = list->head;
    int i = 0;
    while(i<index && node!=NULL) {
        node = node->next;
    }
    return node ? node->element : NULL;
}

int list_get_index_of_element( list_ptr_t list, element_ptr_t element ){
    assert(list);
    list_node_ptr_t node = list->head;
    int i = -1, j = -1;
    while(node!=NULL) {
        i++;
        if( list->compare_func( node->element, element) == 0 ){
            j = i;
            break;
        }
        node = node->next;
    }
    return j;
}

void list_print( list_ptr_t list ){
    assert(list);
    printf("\n*****Print LIst*****\nList size: %d\n", list_size(list));
    list_node_ptr_t node = list->head;
    while(node!=NULL) {
        list->print_func( node->element );
        node = node->next;
    }
    printf("\n%s\n\n", "*****Print Finished*****");
}
