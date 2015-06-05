#ifndef DOUBLE_LIST
#define DOUBLE_LIST

/*------------------------------------------------------------------------------
		definitions (defines, typedefs, ...)
------------------------------------------------------------------------------*/
typedef void * data_ptr_t;

typedef enum{NONE = 0, NULL_POINTER_ERR,  NONE_EXIST_REF_ERR, NON_NEXT_REF_ERR,NON_PREVIOUS_REF_ERR,NO_DATA_ERR} err_code;

//call back funcs
typedef void destory_data_func(data_ptr_t);
typedef int compare_data_func(data_ptr_t, data_ptr_t);
typedef data_ptr_t deep_copy_func(data_ptr_t);

typedef struct funcs{
	destory_data_func *destory_data;
	compare_data_func *compare_data;
	deep_copy_func *deep_copy;
} callback_funcs;

typedef struct list{
	data_ptr_t data;
	callback_funcs * funcs;
	struct list * previous;
	struct list * next;
} list_t ,*list_ptr_t;

//typedef list_t * list_ptr_t;

err_code err;

/*------------------------------------------------------------------------------
		function prototypes
------------------------------------------------------------------------------*/
list_ptr_t list_get_first_reference( list_ptr_t list );
list_ptr_t list_get_last_reference( list_ptr_t list );
int list_size( list_ptr_t list ); //
list_ptr_t list_alloc (destory_data_func *, compare_data_func *, deep_copy_func *, data_ptr_t );
void list_free( list_ptr_t list );
list_ptr_t list_insert_at_index( list_ptr_t list, data_ptr_t data, int index);
list_ptr_t list_insert_at_reference( list_ptr_t list, data_ptr_t data, list_ptr_t reference );
int list_get_index_of_reference( list_ptr_t list, list_ptr_t reference );
list_ptr_t list_free_at_index( list_ptr_t list, int index);//
list_ptr_t list_free_at_reference( list_ptr_t list, list_ptr_t reference );
list_ptr_t list_free_data( list_ptr_t list, data_ptr_t data );
list_ptr_t list_get_next_reference( list_ptr_t list, list_ptr_t reference );
list_ptr_t list_get_previous_reference( list_ptr_t list, list_ptr_t reference );
list_ptr_t list_get_reference_at_index( list_ptr_t list, int index );
data_ptr_t list_get_data_at_reference( list_ptr_t list, list_ptr_t reference );
data_ptr_t list_get_data_at_index( list_ptr_t list, int index );
int list_get_index_of_data( list_ptr_t list, data_ptr_t data );//
list_ptr_t list_get_reference_of_data( list_ptr_t list, data_ptr_t data );
list_ptr_t list_insert_sorted( list_ptr_t list, data_ptr_t data );//
list_ptr_t sort_list(list_ptr_t list);//

#endif
