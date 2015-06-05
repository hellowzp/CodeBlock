#include "prioritized_list.h"

// benchmarks:
// number of message you can add in 1 second: 6 780 000
// percentage of time used for system calls: 10%

/**
 * MACROS
  **/

#ifdef DEBUG
	#define PL_PRINT_ERROR(error_number)																	\
		do{																										\
			printf("\nERROR line %d of file %s by function %s:\n   Error %d: %s\n",			\
			__LINE__, __FILE__,__func__,error_number, pl_get_error_message(error_number));\
			}																										\
			while(0)
#else
	#define PL_PRINT_ERROR(s)																					\
		do{} while(0)
#endif

#ifdef DEBUG
	#define PL_PRINT_ERROR_STRING(error_string)															\
		do(																										\
			printf("\nERROR line %d of file %s by function %s:\n   Error : %s\n",			\
			__LINE__, __FILE__,__func__, error_string)												\
			);																										\
		while(0)
#else
	#define PL_PRINT_ERROR_STRING(s)																		\
		do{};																										\
		while(0)
#endif

#define PL_TRY(db_function)																				\
    do{ uint32_t pl_error = 0;																			\
         pl_error = db_function;																			\
        if(pl_error){																						\
  			  PL_PRINT_ERROR(pl_error);																	\
  			  return pl_error; 																				\
  		  }																										\
    }																												\
    while(0)

#define PL_CATCH(condition)																				\
	if(condition)							

#define MALLOC malloc

#define PL_TRY_LOCK(mutex, blocking, timeout)														\
	do{																											\
		int internal_mutex_error = 0;																		\
		if(!blocking){																							\
			internal_mutex_error = pthread_mutex_trylock(&(mutex));								\
		} else {																									\
			if(timeout == NULL){																				\
				internal_mutex_error = pthread_mutex_lock(&(mutex));								\
			} else {																								\
				internal_mutex_error = pthread_mutex_timedlock(&(mutex), timeout);			\
			}																										\
																													\
			switch(internal_mutex_error){																	\
				case 0: break;																					\
				/* case ETIMEDOUT: return CQ_TIMEOUT; */												\
				default: return PL_MUTEX_ERROR;															\
			}																										\
		}																											\
	} while (0)

#define PL_TRY_UNLOCK(mutex)																				\
	do{																											\
		int internal_mutex_error = 0;																		\
		internal_mutex_error = pthread_mutex_unlock(&(mutex));									\
		if(internal_mutex_error != 0){																	\
			return PL_MUTEX_ERROR;																			\
		}																											\
	} while (0)

#define PL_TRY_INIT_MUTEX(mutex)																				\
	do{																											\
		int internal_mutex_error = 0;																		\
		internal_mutex_error = pthread_mutex_init(&(mutex), NULL);								\
		if(internal_mutex_error != 0)																		\
			return PL_MUTEX_ERROR;																			\
	} while(0)

#define PL_TRY_INIT_COND(mutex)																				\
	do{																											\
		int internal_mutex_error = 0;																		\
		internal_mutex_error = pthread_cond_init(&(mutex), NULL);								\
		if(internal_mutex_error != 0)																		\
			return PL_MUTEX_ERROR;																			\
	} while(0)


//TODO write specific error codes for mutex errors		    

typedef uint8_t pl_flag;

typedef struct Pl_list_element{
    struct Pl_list_element* next;
    struct Pl_list_element* previous;
    pl_data data;
} pl_list_element;

typedef struct Pl_list{
    uint32_t priority;
    uint32_t size;
	 struct Pl_list* next;
	 struct Pl_list* previous;
    pl_list_element* first;
    pl_list_element* last;
    uint32_t to_read;
    uint32_t weight;
    pthread_mutex_t write_lock;
} pl_list;

typedef struct Pl_class{
    uint32_t priority;
    uint32_t size;
    pl_list* first;
    pl_list* current;
	 struct Pl_class* next;
	 struct Pl_class* previous;
} pl_class;

typedef struct Prioritized_list{
    pl_class* first;
    pl_class* current;
    uint32_t elements_used;
    void (* destroy)(pl_data*);
    pl_data (* copy)(pl_data);
    uint32_t maximum_elements_used;
	 uint32_t users;
	 pthread_mutex_t read_lock;
	 pthread_mutex_t write_lock;
	 pthread_mutex_t user_lock;
	 pthread_cond_t user_left;
	 pthread_mutex_t global_lock;
	 pl_error error;
} intern_prioritized_list;

/*********************************************************************************
**		private prototypes
**********************************************************************************/

// debug prints

static void print_pl_list(pl_list* list);
static void print_pl_class( pl_class* current );
static void print_pl_list_element(pl_list_element* element);

// create the data structures

static pl_error create_pl_list(pl_list** new_element);
static pl_error create_list_element(intern_prioritized_list* q, pl_list_element** new_element);

// insert an element

static pl_error insert_list_element(pl_list* list, pl_list_element* to_insert);
static pl_error insert_pl_list(intern_prioritized_list* list, pl_class* q, pl_list* to_insert);
static pl_error insert_pl_class(intern_prioritized_list* q, pl_class* to_insert);

// isert a new element

static pl_error insert_new_element(intern_prioritized_list* q, pl_list* list, pl_data* data);
static pl_error insert_new_list(intern_prioritized_list* list, pl_class* q, uint32_t priority, uint32_t weight);
static pl_error insert_new_class(intern_prioritized_list* q, uint32_t priority);

// get an element

static pl_error get_list_element(pl_list* list, pl_list_element** return_value);
static pl_error get_class(intern_prioritized_list* q, uint32_t priority, pl_class** return_value);
static pl_error get_list(intern_prioritized_list* list, pl_list** result, pl_class** result_class, uint32_t class, uint32_t priority);
static pl_error get_list_locking(intern_prioritized_list* list, pl_list** result, pl_class** result_class, uint32_t class, uint32_t priority, pl_flag blocking, const struct timespec* timeout);

// add data

static pl_error global_unlock(intern_prioritized_list*q);
static pl_error global_lock(intern_prioritized_list* q, pl_flag blocking, const struct timespec* timeout);
static pl_error add_pl_data_list(intern_prioritized_list* q, pl_class* class, pl_list* list, pl_data* data, pl_flag blocking, const struct timespec* timeout);
static pl_error add_data(intern_prioritized_list* q, pl_data* data, uint32_t class, uint32_t priority, uint32_t weight, pl_flag blocking, const struct timespec* timeout);

// get data
static pl_error get_list_read_locking(intern_prioritized_list* list, pl_list** result, pl_class** result_class, pl_flag blocking, const struct timespec* timeout);
static pl_error add_user(intern_prioritized_list* q, pl_flag blocking, const struct timespec* timeout);
static pl_error remove_user(intern_prioritized_list* q, pl_flag blocking, const struct timespec* timeout);
static pl_error read_data_list(intern_prioritized_list* pl, pl_list* list, pl_data* data);
static pl_error get_data(intern_prioritized_list* q, pl_data* data, pl_flag blocking, const struct timespec* timeout);

// destroy element

static pl_error destroy_list(intern_prioritized_list* q, pl_list** list);
static pl_error destroy_class(intern_prioritized_list* q, pl_class** list);

// public

/* the the total number of elements used */
pl_error get_size_prioritized_list(prioritized_list list, uint32_t* size){
	*size = 0;
	intern_prioritized_list* q = (intern_prioritized_list*) list;
	*size = q->elements_used;
	return PL_OK;
}

char* pl_get_error_message(pl_error error){
	switch(error){
		case PL_OK: return "Everyting is ok, no error detected.";
		case PL_NO_DATA: return "Requested data but their is none.";
		case PL_NO_MEMORY_FREE: return "Allocating the memory needed failed";
		case PL_MUTEX_ERROR: return "When trying to lock or free a mutex, an error occured. This can be a try_lock that failed or a timeout, please check further. If better specified, corrent this message.";
		case PL_LIST_NOT_FOUND: return "The list requested for could not be found or created.";
		case PL_CLASS_NOT_FOUND: return "The class requested for could not be found or created.";
		case PL_CLASS_ALREADY_EXIST: return "The class that you tried to add does already exist.";
		case PL_FULL: return "You reached the maximum specified size of your prioritized list. Please stop adding data until you read some or set a higher limit.";
		case PL_LIST_ALREADY_EXIST: return "The list that you tried to add does already exist.";
		case PL_EMPTY: return "The structure is empty, no data can be returned.";
		case PL_IMPLEMENTATION_ERROR: return "The prioritized list contains conflicting information. Please check your implementation";
		default: return "The error is not a well specified error, you should not get this message. Pleas check further.";
	}
}

static pl_error create_list_element(intern_prioritized_list* q, pl_list_element** new_element){
	if((q->maximum_elements_used - q->elements_used) == 0)
		return PL_FULL;
	pl_list_element* tmp = (pl_list_element*) MALLOC(sizeof(pl_list_element));
	if(tmp == NULL)
		return PL_NO_MEMORY_FREE;
	
	tmp->previous = NULL;
	tmp->next = NULL;
	tmp->data = NULL;
	*new_element = tmp;
	return PL_OK;
}

static pl_error destroy_list_element(intern_prioritized_list *list, pl_list_element* to_destroy){
	 if(to_destroy->data != NULL)
		 (list->destroy)(to_destroy->data);
    free(to_destroy);
    return PL_OK;
}

static pl_error insert_list_element(pl_list* list, pl_list_element* to_insert){
	to_insert->previous = list->last;
	to_insert->next = NULL;
	if(to_insert->previous != NULL){
		list->last->next = to_insert;
	}
	if(list->first == NULL){
		list->first = to_insert;
	}
	list->last = to_insert;
	return PL_OK;
}

static pl_error insert_new_element(intern_prioritized_list* q, pl_list* list, pl_data* data){
	pl_list_element* to_insert;
	PL_TRY(create_list_element(q, &to_insert));
	to_insert->data = *data;
	PL_TRY(insert_list_element(list, to_insert));
	return PL_OK;
}

static pl_error read_data_list(intern_prioritized_list* pl, pl_list* list, pl_data* data){
    list->to_read --;
    pl_list_element* list_element;
    PL_TRY(get_list_element(list, &list_element));
    *data = list_element->data;
    list_element->data = NULL;
    PL_TRY(destroy_list_element(pl, list_element));
	 
    return PL_OK;
}

static pl_error destroy_list(intern_prioritized_list* q, pl_list** list){
	uint32_t size = 0;
	
	pl_list_element* current = NULL;

	for(size = 0; size < (*list)->size; size++){
		if((*list)->first == NULL)
			return PL_IMPLEMENTATION_ERROR;
		current = (*list)->first;
		(*list)->first = current->next;
		PL_TRY(destroy_list_element(q, current));
	}

	free(*list);
	*list = NULL;

	return PL_OK;
}

static pl_error get_list_element(pl_list* list, pl_list_element** return_value){
    *return_value = NULL;
    if(list->size == 0)
        return PL_NO_DATA;
    *return_value = list->first;
	 if(list->size > 1){
		 if(list->first->next == NULL)
			 return PL_IMPLEMENTATION_ERROR;
	    list->first = list->first->next;
  		 list->first->previous = NULL;
    } else {
		list->first = NULL;
		list->last = NULL;
	}
    return PL_OK;
}

static pl_error create_pl_list(pl_list** new_element){
	pl_list* tmp = (pl_list*) malloc(sizeof(pl_list));
	if(tmp == NULL)
		return PL_NO_MEMORY_FREE;
	tmp->priority = 0;
	tmp->size = 0;
	tmp->last = NULL;
	tmp->first = NULL;
	tmp->previous = NULL;
	tmp->next = NULL;
	tmp->to_read = 0;
	tmp->weight = 0;

	int mutex_error = pthread_mutex_init(&(tmp->write_lock), NULL);
	if(mutex_error != 0)
		return PL_MUTEX_ERROR;

	*new_element = tmp;
	return PL_OK;
}

static pl_error insert_new_list(intern_prioritized_list* list, pl_class* q, uint32_t priority, uint32_t weight){
	pl_list* tmp;
	PL_TRY(create_pl_list(&tmp));
	tmp->priority = priority;
	tmp->weight = weight;
	PL_TRY(insert_pl_list(list, q, tmp));
	return PL_OK;
}

static pl_error insert_pl_list(intern_prioritized_list* list, pl_class* q, pl_list* to_insert){
	
	if(list->elements_used + to_insert->size > list->maximum_elements_used)
		return PL_FULL;

	if(q->first == NULL){
		q->first = to_insert;
		to_insert->next = NULL;
	} else {
		if(q->first->priority < to_insert->priority){
			to_insert->next = q->first;
			q->first->previous = to_insert;
			to_insert->previous = NULL;
			q->first = to_insert;
		} else {
			pl_list* current = q->first;

			while(current->next != NULL && current->priority > to_insert->priority){
				current = current->next;
			}

			if(current->priority == to_insert->priority)
				return PL_LIST_ALREADY_EXIST;

			if(current->priority < to_insert->priority){
				/* insert the list before this one and after the previous one */
				to_insert->next = current;
				to_insert->previous = current->previous;
				current->previous = to_insert;
				to_insert->previous->next = to_insert;
			} else {
				/* insert the list at the end */
				to_insert->next = NULL;
				to_insert->previous = current;
				current->next = to_insert;
			}
			list->elements_used += to_insert->size;
			q->size += to_insert->size;
		}
	}
	
	return PL_OK;
}

static pl_error create_pl_class(pl_class** new_element){
	pl_class* tmp = (pl_class*) malloc(sizeof(pl_class));
	if(tmp == NULL)
		return PL_NO_MEMORY_FREE;
	tmp->priority = 0;
	tmp->size = 0;
	tmp->first = NULL;
	tmp->current = NULL;
	tmp->next = NULL;
	tmp->previous = NULL;
	*new_element = tmp;
	return PL_OK;
}

static pl_error insert_new_class(intern_prioritized_list* q, uint32_t priority){
	pl_class* tmp;
	PL_TRY(create_pl_class(&tmp));
	tmp->priority = priority;
	PL_TRY(insert_pl_class(q, tmp));
	return PL_OK;
}

static pl_error insert_pl_class(intern_prioritized_list* q, pl_class* to_insert){
	
	if(q->elements_used + to_insert->size > q->maximum_elements_used)
		return PL_FULL;

	if(q->first == NULL){
		q->first = to_insert;
		to_insert->next = NULL;
	} else {
		if(q->first->priority < to_insert->priority){
			to_insert->next = q->first;
			q->first->previous = to_insert;
			to_insert->previous = NULL;
			q->first = to_insert;
		}else{
			pl_class* current = q->first;

			while(current->next != NULL && current->priority > to_insert->priority){
				current = current->next;
			}

			if(current->priority == to_insert->priority)
				return PL_CLASS_ALREADY_EXIST;

			if(current->priority < to_insert->priority){
				/* insert the class before this one and after the previous one */
				to_insert->next = current;
				to_insert->previous = current->previous;
				current->previous = to_insert;
				to_insert->previous->next = to_insert;
			} else {
				/* insert the class at the end */
				to_insert->next = NULL;
				to_insert->previous = current;
				current->next = to_insert;
			}
			q->elements_used += to_insert->size;
		}
	}
	
	return PL_OK;
}

static pl_error destroy_class(intern_prioritized_list* q, pl_class** list){
	pl_list* current = NULL;

	while((*list)->first != NULL){
		current = (*list)->first;
		(*list)->first = current->next;
		PL_TRY(destroy_list(q, &current));
	}

	free(*list);
	*list = NULL;

	return PL_OK;
}


static pl_error get_class(intern_prioritized_list* q, uint32_t priority, pl_class** return_value){
	if(q->first == NULL)
		return PL_CLASS_NOT_FOUND;
	pl_class* current = q->first;
	while(current->priority > priority && current->next != NULL){
		current = current->next;
	}

	if(current->priority == priority){
		*return_value = current;
		return PL_OK;
	}

	return PL_CLASS_NOT_FOUND;
}

pl_error create_prioritized_list(prioritized_list* new_element, uint32_t maximum_elements_used, void (* destroy)(pl_data*), pl_data (* copy)(pl_data)){
	intern_prioritized_list* tmp = (intern_prioritized_list*) malloc(sizeof(intern_prioritized_list));
	if(tmp == NULL)
		return PL_NO_MEMORY_FREE;
	int mutex_error = 0;
	
	tmp->first = NULL;
	tmp->current = NULL;
	tmp->elements_used = 0;
	tmp->destroy = destroy;
	tmp->copy = copy;
	tmp->maximum_elements_used = maximum_elements_used;

	mutex_error = pthread_mutex_init(&(tmp->write_lock), NULL);
	if(mutex_error != 0)
		return PL_MUTEX_ERROR;

	tmp->users = 0;

	PL_TRY_INIT_COND(tmp->user_left);
	PL_TRY_INIT_MUTEX(tmp->write_lock);
	PL_TRY_INIT_MUTEX(tmp->read_lock);
	PL_TRY_INIT_MUTEX(tmp->user_lock);
	PL_TRY_INIT_MUTEX(tmp->global_lock);

	tmp->error = PL_OK;

	*new_element = (prioritized_list) tmp;
	return PL_OK;
}

pl_error destroy_prioritized_list(prioritized_list* q){
	intern_prioritized_list** list = (intern_prioritized_list**) q;
	pl_class* current = NULL;

	while((*list)->first != NULL){
		current = (*list)->first;
		(*list)->first = current->next;
		PL_TRY(destroy_class((*list), &current));
	}

	free(*list);
	*list = NULL;

	return PL_OK;
}



static pl_error get_list_locking(intern_prioritized_list* list, pl_list** result, pl_class** result_class, uint32_t class, uint32_t priority, pl_flag blocking, const struct timespec* timeout){

	/* the data input is a copy of the data, free if the function fails. Also check before calling that their is room */

	/* use the right lock, depending on the input flags */

	PL_TRY_LOCK(list->write_lock, blocking, timeout);

	pl_list* current_list;
	pl_class* current_class;

	if(list->first != NULL){
		current_class= list->first;
	} else {
		PL_TRY_UNLOCK(list->write_lock);
		return PL_CLASS_NOT_FOUND;
	}

	while(current_class->priority > class && current_class->next != NULL){
		current_class = current_class->next;
	}

	if(current_class->priority != class){
		PL_TRY_UNLOCK(list->write_lock);
		return PL_CLASS_NOT_FOUND;
	} else {
		if(current_class->first != NULL){
			current_list = current_class->first;
		} else {
			PL_TRY_UNLOCK(list->write_lock);
			return PL_LIST_NOT_FOUND;
		}

		while(current_list->priority > priority && current_list->next != NULL){
			current_list = current_list->next;
		}

		if(current_list->priority != priority){
			PL_TRY_UNLOCK(list->write_lock);
			return PL_LIST_NOT_FOUND;
		}
	}

	*result_class = current_class;
	*result = current_list;

	PL_TRY_UNLOCK(list->write_lock);
	return PL_OK;
}

static pl_error get_list(intern_prioritized_list* list, pl_list** result, pl_class** result_class, uint32_t class, uint32_t priority){
	pl_list* current_list;
	pl_class* current_class;

	if(list->first != NULL){
		current_class= list->first;
	} else {
		return PL_CLASS_NOT_FOUND;
	}

	while(current_class->priority > class && current_class->next != NULL){
		current_class = current_class->next;
	}

	if(current_class->priority != class){
		return PL_CLASS_NOT_FOUND;
	} else {
		if(current_class->first != NULL){
			current_list = current_class->first;
		} else {
			return PL_LIST_NOT_FOUND;
		}

		while(current_list->priority > priority && current_list->next != NULL){
			current_list = current_list->next;
		}

		if(current_list->priority != priority){
			return PL_LIST_NOT_FOUND;
		}
	}

	*result_class = current_class;
	*result = current_list;
	
	return PL_OK;
}

static pl_error add_pl_data_list(intern_prioritized_list* q, pl_class* class, pl_list* list, pl_data* data, pl_flag blocking, const struct timespec* timeout){

	/* the data input is a copy of the data, free if the function fails. Also check before calling that their is room */

	/* use the right lock, depending on the input flags */

	if(!(q->maximum_elements_used > q->elements_used))
		return PL_FULL;

	PL_TRY_LOCK(list->write_lock, blocking, timeout);
	
	/* write the data en set the number of users 1 higher */

	PL_TRY(add_user(q, blocking, timeout));

	PL_TRY(insert_new_element(q, list, data));

	PL_TRY_LOCK(q->user_lock, blocking, timeout);

	(q->elements_used)++;
	(class->size)++;
	(list->size)++;

	PL_TRY_UNLOCK(q->user_lock);

	PL_TRY(remove_user(q, blocking, timeout));

	PL_TRY_UNLOCK(list->write_lock);	
	return PL_OK;
}

static pl_error add_user(intern_prioritized_list* q, pl_flag blocking, const struct timespec* timeout){
	PL_TRY_LOCK(q->user_lock, blocking, timeout);
	
	q->users++;

	PL_TRY_UNLOCK(q->user_lock);	
	return PL_OK;
}

static pl_error remove_user(intern_prioritized_list* q, pl_flag blocking, const struct timespec* timeout){
	PL_TRY_LOCK(q->user_lock, blocking, timeout);
	
	q->users--;

	pthread_cond_signal(&(q->user_left));

	PL_TRY_UNLOCK(q->user_lock);
	return PL_OK;
}



static pl_error add_data(intern_prioritized_list* q, pl_data* data, uint32_t class, uint32_t priority, uint32_t weight, pl_flag blocking, const struct timespec* timeout){
	//TODO make the needed time corrections
	pl_error error = PL_OK;

	pl_list* current_list;
	pl_class* current_class;
	error = get_list_locking(q, &current_list, &current_class, class, priority, blocking, timeout);


	if(error == PL_CLASS_NOT_FOUND || error == PL_LIST_NOT_FOUND){
		PL_TRY(global_lock(q, blocking, timeout));

		if(error == PL_CLASS_NOT_FOUND)
			PL_TRY(insert_new_class(q, class));

		PL_TRY(get_class(q, class, &current_class));
		PL_TRY(insert_new_list(q, current_class, priority, weight));
		PL_TRY(get_list(q, &current_list, &current_class, class, priority));
		PL_TRY(global_unlock(q));
		
		error = PL_OK;
	} else {
		if(error != PL_OK)
			return error;
	}

#ifdef PL_NO_COPY
	pl_data tmp = *data;
#else
	pl_data tmp =  q->copy(*data);
#endif

	PL_TRY(add_pl_data_list(q, current_class, current_list, &tmp, blocking, timeout));

	return PL_OK;
}

static pl_error get_data(intern_prioritized_list* q, pl_data* data, pl_flag blocking, const struct timespec* timeout){
	//TODO make the needed time corrections
	pl_list* current_list;
	pl_class* current_class;
	PL_TRY(get_list_read_locking(q, &current_list, &current_class, blocking, timeout));

	//TODO use a conditional wait until you lock both mutexes before you use timeouts, otherwise
	// it is posible that the count is not updated

	PL_TRY_LOCK(current_list->write_lock, blocking, timeout);

	PL_TRY(read_data_list(q, current_list, data));

	PL_TRY_UNLOCK(current_list->write_lock);

	PL_TRY_LOCK(q->user_lock, blocking, timeout);
	q->elements_used--;
	current_class->size--;
	current_list->size--;
	PL_TRY_UNLOCK(q->user_lock);

	return PL_OK;
}



static pl_error global_unlock(intern_prioritized_list*q){
	PL_TRY_UNLOCK(q->global_lock);
	PL_TRY_UNLOCK(q->write_lock);
	PL_TRY_UNLOCK(q->read_lock);

	return PL_OK;
}

static pl_error global_lock(intern_prioritized_list* q, pl_flag blocking, const struct timespec* timeout){

	// lock the write lock

	PL_TRY_LOCK(q->write_lock, blocking, timeout);
	PL_TRY_LOCK(q->read_lock, blocking, timeout);

	if(q->users !=0){

		// block the user lock
		PL_TRY_LOCK(q->user_lock, blocking, timeout);
		
		// wait until their are no users left
		while(q->users != 0){
			pthread_cond_wait(&(q->user_left), &(q->user_lock));
		}
		PL_TRY_UNLOCK(q->user_lock);
	}

	// lock the global lock
	PL_TRY_LOCK(q->global_lock, blocking, timeout);

	return PL_OK;
}

static pl_error get_list_read_locking(intern_prioritized_list* list, pl_list** result, pl_class** result_class, pl_flag blocking, const struct timespec* timeout){

	while(list->elements_used == 0 ){
		pthread_cond_signal(&(list->user_left));
	}

	pl_list* current_list;
	pl_class* current_class;

	PL_TRY_LOCK(list->read_lock, blocking, timeout);
	// the list size can only grow bigger after the read lock
	if(list->elements_used != 0){
		current_class= list->first;
	} else {
		if(list->first == NULL){

			PL_TRY_UNLOCK(list->read_lock);
			return PL_EMPTY;
		}

		//TODO wait for a write to happen or return with list empty error
	}

	/* go to the next class if this one is empty and has a next one */
	while(current_class->size == 0 && current_class->next != NULL){
		current_class = current_class->next;
	}
	
	/* if this one is empty it means their is no data, return an error */
	if(current_class->size == 0){
		PL_TRY_UNLOCK(list->read_lock);
		return PL_EMPTY;
	}

	/* if this one has no list in it, return an error */
	if(current_class->first == NULL){
		PL_TRY_UNLOCK(list->read_lock);
		return PL_IMPLEMENTATION_ERROR;
	}

	/* if the current is not set, set it to the first */
	if(current_class->current == NULL){
		current_class->current = current_class->first;	
	}

	current_list = current_class->current;

	/* go to the next list as long as their is on and this one is empty or should not be red */
	while((current_list-> size == 0 || current_list->to_read == 0) && current_list->next != NULL){
		current_list = current_list->next;
	}

	/* if this one is empty and has nothing to read, set the to_read of all the lists on weight */
	if(current_list->size == 0 || current_list->to_read == 0){
		current_class->current = current_class->first;
		current_list = current_class->first;
		current_list->to_read = current_list->weight;
		while(current_list->next != NULL){
			current_list = current_list->next;
			current_list->to_read = current_list->weight;
		}
	
		current_list = current_class->first;
	
		/* go to the next list as long as their is on and this one is empty or should not be red */
		while((current_list-> size == 0 || current_list->to_read == 0) && current_list->next != NULL){
			current_list = current_list->next;
		}

		/* if this one is empty and has nothing to read, set the to_read an error occured */
		if(current_list->size == 0 || current_list->to_read == 0){
			PL_TRY_UNLOCK(list->read_lock);
			return PL_IMPLEMENTATION_ERROR;
		}
	}

	*result = current_list;
	*result_class = current_class;

	PL_TRY_UNLOCK(list->read_lock);
	return PL_OK;
}

pl_error add_data_blocking(prioritized_list list, pl_data data, uint32_t class, uint32_t priority, uint32_t weight){
	intern_prioritized_list* q = (intern_prioritized_list*) list;
	PL_TRY(add_data(q, &data, class, priority, weight, 1, NULL));
	return PL_OK;
}

pl_error add_data_try(prioritized_list list, pl_data data, uint32_t class, uint32_t priority, uint32_t weight){
	intern_prioritized_list* q = (intern_prioritized_list*) list;
	PL_TRY(add_data(q, &data, class, priority, weight, 0, NULL));
	return PL_OK;
}

pl_error get_data_blocking(prioritized_list list, pl_data* data){
	intern_prioritized_list* q = (intern_prioritized_list*) list;
	PL_TRY(get_data(q, data, 1, NULL));
	return PL_OK;
}

pl_error get_data_try(prioritized_list list, pl_data* data){
	intern_prioritized_list* q = (intern_prioritized_list*) list;
	PL_TRY(get_data(q, data, 0, NULL));
	return PL_OK;
}


/* result of the print

 +--------------------------prioritized list----------------------------------
 |
 | first class: (nil)
 | current class: (nil)
 |
 | elements used: 0
 | maximum elements used: 0
 |
 | current number of users: 0
 | current error set: 0
 |
 +----------------------------------------------------------------------------
*/


void print_prioritized_list(prioritized_list list){
	intern_prioritized_list* q = (intern_prioritized_list*) list;
	if(q == NULL){
		printf("no list specified\n");
	} else {
		printf("\n +--------------------------prioritized list----------------------------------\n |\n");
		printf(" | first class: %p\n | current class: %p\n", q->first, q->current);
		printf(" |\n | elements used: %d\n | maximum elements used: %d\n", 
		q->elements_used, q->maximum_elements_used);
		printf(" |\n | current number of users: %d\n", q->users);
		printf(" | current error set: %d\n", q->error);
		printf(" |\n +----------------------------------------------------------------------------\n\n");

		if(q->first == NULL){
			printf("\nNo classes yet\n");
		} else {
			pl_class* current = q->first;
			print_pl_class( current );
			while(current->next != NULL){
				current = current->next;
				print_pl_class( current );
			}
		}
		
	}
}

/* result of the print:

 +----------------------class----------------------
 | address: (nil)
 |
 | priority: 0
 | size: 0
 | first: (nil)
 | current: (nil)
 | next class: (nil)
 | previous class: (nil)
 | Not yet a list in here
 +-------------------------------------------------
*/

static void print_pl_class( pl_class* current ){
	printf("\n +----------------------class----------------------\n | address: %p\n |\n", current);
	printf(" | priority: %d\n | size: %d\n", current->priority, current->size);
	printf(" | first: %p\n | current: %p\n | next class: %p\n | previous class: %p\n", current->first, current->current, current->next, current->previous);
	if(current->first == NULL){
		printf(" | Not yet a list in here\n");
	} else {
		pl_list* current_list = current->first;
		print_pl_list(current_list);
		while(current_list->next != NULL){
			current_list = current_list->next;
			print_pl_list(current_list);
		}
	}
	printf(" +-------------------------------------------------\n");
}

/* result of the print:

|     +-----------------list-----------------
|     | address: (nil)
|     |
|     | priority: 0
|     | size: 0
|     | weight: 0
|     | to read: 0
|     |
|     | previous list: (nil)
|     | next list: (nil)
|     | first element: (nil)
|     | last element: (nil)
|     | This list is empty
|     +--------------------------------------
*/
				  
static void print_pl_list(pl_list* list){
	printf(" |\n |     +-----------------list-----------------\n |     | address: %p\n |     |\n", list);
	printf(" |     | priority: %d\n |     | size: %d\n |     | weight: %d\n |     | to read: %d\n", list->priority, list->size, list->weight, list->to_read);
	printf(" |     |\n |     | previous list: %p\n |     | next list: %p\n |     | first element: %p\n |     | last element: %p\n", list->previous, list->next, list->first, list->last);
	if(list->first != NULL){
		pl_list_element* current = list->first;
		print_pl_list_element(current);
		while(current->next != NULL){
			current = current->next;
			print_pl_list_element(current);
		}
	} else {
		printf(" |     | This list is empty\n");
	}
	printf(" |     +--------------------------------------\n");
}

/* result of the print:

 |     |     #----------element----------
 |     |     # address: (nil)
 |     |     # previous: (nil)
 |     |     # next: (nil)
 |     |     # data: (nil)
 |     |     #---------------------------
*/

static void print_pl_list_element(pl_list_element* element){
	printf(" |     |\n |     |     #----------element----------\n |     |     # address: %p\n |     |     # previous: %p\n |     |     # next: %p\n |     |     # data: %p\n |     |     #---------------------------\n", element, element->previous, element->next, element->data);
}
