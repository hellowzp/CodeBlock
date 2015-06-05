#ifndef  SORTED_DOUBLE_LINKED_LIST_H
#define  SORTED_DOUBLE_LINKED_LIST_H

#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include "../mqtt_sn_bridge.h"
#include "sorted_double_linked_list.h"
#include "../../mqtt_sn_db/mqtt_sn_db.h"
#include "../err_handler.h"

struct sensor_data{
    DADDRESS address;
    TIME_PROTOCOL     duration;
    time_t   timeout;
    MESSAGE* message;
};

typedef struct sensor_data  data_element;
typedef data_element* data_ptr_t;

struct list_item{
   struct list_item *previousElement;
   struct list_item *nextElement;
   data_ptr_t data;
};

typedef struct list_item list_t;

typedef list_t* list_ptr_t;

extern ERR_NUM err_num;


list_ptr_t list_init ( void );
// Returns a pointer to a newly-allocated list.

void list_free_all( list_ptr_t* list );
// Every element of 'list' needs to be deleted (free memory) and finally the list itself needs to be deleted (free all memory)

int list_size( list_ptr_t list );
// Returns the number of elements in 'list'.

int list_get_by_address( list_ptr_t list, DADDRESS address, list_ptr_t *return_value);
//return list_item in which the data contain address equal to the address the parameter passed.

list_ptr_t list_put(list_ptr_t *list, DADDRESS address, time_t timeout, TIME_PROTOCOL duration, MESSAGE *message);
//return the whole list. insert new item to the sorted list.

list_ptr_t list_get_element_with_bigger_value(list_ptr_t list, time_t value);
//return the element in which the data contain the timeout value bigger than the value the parameter passed.

list_ptr_t list_get_last_element(list_ptr_t list);
//return the last element in the list.

list_ptr_t list_update(list_ptr_t *list, DADDRESS address, time_t value);
//update the sorted list when new message comes in.

list_ptr_t list_delete_by_address(list_ptr_t *list, DADDRESS address);
//delete the element according to the address.

list_ptr_t list_find_first_output_item(list_ptr_t list);
//get the first element in which the message field is not null.

void print_list(list_ptr_t list);

#endif
