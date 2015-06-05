#ifndef PRIORITIZED_LIST_H
#define PRIORITIZED_LIST_H

/*********************************************************************************
**		Compiler options
**********************************************************************************/

/*
	compile with the option DEBUG to get all the error messages.

	complie with the option PL_NO_COPY and the copy functions will not be used
	to copy the input data you provided to a newly allocated memory. This will boost
	performance buth the downsite is that you cant change the data after you added
	it. Changing the data will result in a change in data in the list. Freeing the
	data will result in an error and will crash the program. Don't give this flag
	for a safer implementation.
*/

/*********************************************************************************
**		Dependencies
**********************************************************************************/
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <signal.h>

/*********************************************************************************
**		Type defenitions
**********************************************************************************/

typedef void* pl_data;
typedef void* prioritized_list;
typedef enum Pl_error pl_error;

/*********************************************************************************
**		Error Codes
**********************************************************************************/

//TODO write specific error codes for mutex errors		    
enum Pl_error{
	PL_OK = 0, 
	PL_NO_DATA, 
	PL_NO_MEMORY_FREE, 
	PL_MUTEX_ERROR, 
	PL_LIST_NOT_FOUND, 
	PL_CLASS_NOT_FOUND, 
	PL_CLASS_ALREADY_EXIST, 
	PL_FULL, 
	PL_LIST_ALREADY_EXIST, 
	PL_EMPTY, 
	PL_IMPLEMENTATION_ERROR
};


/*********************************************************************************
**		Prototypes
**********************************************************************************/

/* debug related funcitons */

extern pl_error get_size_prioritized_list(prioritized_list q, uint32_t* size);
extern void print_prioritized_list(prioritized_list q);
extern char* pl_get_error_message(pl_error error);

/* initialize and destroy functions */

extern pl_error create_prioritized_list(prioritized_list* new_element, uint32_t maximum_elements_used, void (* destroy)(pl_data*), pl_data (* copy)(pl_data));
extern pl_error destroy_prioritized_list(prioritized_list* list);

/* input related functions */

extern pl_error add_data_blocking(prioritized_list q, pl_data data, uint32_t class, uint32_t priority, uint32_t weight);
extern pl_error add_data_try(prioritized_list q, pl_data data, uint32_t class, uint32_t priority, uint32_t weight);

/* output related functions */

extern pl_error get_data_blocking(prioritized_list q, pl_data* data);
extern pl_error get_data_TRY(prioritized_list q, pl_data* data);

#endif
