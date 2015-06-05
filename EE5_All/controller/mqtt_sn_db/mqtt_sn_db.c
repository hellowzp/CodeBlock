#include "mqtt_sn_db.h"

/* IMPORTANT

--	warning: sqlite3 will bind parameters starting with parameter 1, 
	returning a result and getting the right column will start form 0.

--	important input rule: incomming connect messages must have the lowest priority.
	in that way chanses are as low as posible that the address is reused and the messages
	from the old owner come in and look as if they are from the new owner.

--	important timers: the database will not contain any timers. The user should implement
	his own timers.
	*	for qos 1 and 2: if you send or receive a message, store it IN DATABASE FORMAT and start
  		a timer, if the time expires, give the message to the database and tell it is expired.
	*	for sleep of a device: if you get a disconnect with sleep, start the sleep timer and remember
		address. Reset this timer if needed. If the timer expires, calll the disconnect function with
		the flag unexpected.

--	important note for end nodes: If you request a disconnect (or sleep) or unsubscribe, no new
	messages will be send to you. Old messages that are not finished yet will be send. If you choose
	to ignore them, you will lose them. If QoS is 1 or 2 and a timeout takes place, they will be set
	as being not acknowledged and will wait until you reconnect. You will not loose them. Please handle
	all incomming messages first until completed before going to sleep.

--	auto-disconnect disconnect a device using the address requested without an error

// TODO always close a prepared statement before a return
// TODO when exiting the program unexpected, first call the close function
*/

/**
 * enums
 **/
 

 //ERRORS
 
typedef enum Error{
	DB_OK = 0,
	DB_UNSPECIFIED_ERROR,
	DB_ADDRESS_ALREADY_USED,
	DB_DATABASE_ERROR,
	DB_CANNOT_OPEN_DATABASE,
	DB_NO_MATCH_FOUND,
	DB_DEVICE_NOT_FOUND,
	DB_PAD_TO_LONG,
	DB_NO_WILDCARDS_ALLOWED,
	DB_MEMORY_ALLOCATION_FAILED,
	DB_NO_DATABASE_SELECTED,
	DB_DATABASE_CORRUPT,
	DB_COULD_NOT_FREE,
	DB_NO_INPUT_DATA,
	DB_CANNOT_CREATE_TABLE,
	DB_DANGEROUS_FUNCTION_CALL,
	DB_IMPLEMENTATION_ERROR,
	DB_INVALID_INPUT,
	DB_PRIORITIZED_LIST_ERROR,
	DB_UNDEFINED_BEHAVIOUR,
	DB_DEVICE_NOT_SUBSCRIBED,
	DB_WILL_TOPIC_NOT_SET,
	DB_PIPE_ERROR
} ERROR;

/* defines of error codes, only here for quick reference (28-4-2014) */

// SQLITE_OK           0   /* Successful result */

/* beginning-of-error-codes */

// SQLITE_ERROR        1   /* SQL error or missing database */
// SQLITE_INTERNAL     2   /* Internal logic error in SQLite */
// SQLITE_PERM         3   /* Access permission denied */
// SQLITE_ABORT        4   /* Callback routine requested an abort */
// SQLITE_BUSY         5   /* The database file is locked */
// SQLITE_LOCKED       6   /* A table in the database is locked */
// SQLITE_NOMEM        7   /* A malloc() failed */
// SQLITE_READONLY     8   /* Attempt to write a readonly database */
// SQLITE_INTERRUPT    9   /* Operation terminated by sqlite3_interrupt()*/
// SQLITE_IOERR       10   /* Some kind of disk I/O error occurred */
// SQLITE_CORRUPT     11   /* The database disk image is malformed */
// SQLITE_NOTFOUND    12   /* Unknown opcode in sqlite3_file_control() */
// SQLITE_FULL        13   /* Insertion failed because database is full */
// SQLITE_CANTOPEN    14   /* Unable to open the database file */
// SQLITE_PROTOCOL    15   /* Database lock protocol error */
// SQLITE_EMPTY       16   /* Database is empty */
// SQLITE_SCHEMA      17   /* The database schema changed */
// SQLITE_TOOBIG      18   /* String or BLOB exceeds size limit */
// SQLITE_CONSTRAINT  19   /* Abort due to constraint violation */
// SQLITE_MISMATCH    20   /* Data type mismatch */
// SQLITE_MISUSE      21   /* Library used incorrectly */
// SQLITE_NOLFS       22   /* Uses OS features not supported on host */
// SQLITE_AUTH        23   /* Authorization denied */
// SQLITE_FORMAT      24   /* Auxiliary database format error */
// SQLITE_RANGE       25   /* 2nd parameter to sqlite3_bind out of range */
// SQLITE_NOTADB      26   /* File opened that is not a database file */
// SQLITE_NOTICE      27   /* Notifications from sqlite3_log() */
// SQLITE_WARNING     28   /* Warnings from sqlite3_log() */
// SQLITE_ROW         100  /* sqlite3_step() has another row ready */
// SQLITE_DONE        101  /* sqlite3_step() has finished executing */

/* end-of-error-codes */

/**
 * MACROS
 **/

#ifdef DEBUG
	#define PRINT_ERROR(error_number)																	\
		do{																										\
			fprintf(fd, "\nERROR line %d of file %s by function %s:\n   Error %d: %s\n",	\
			__LINE__, __FILE__,__func__,error_number, db_get_error_message(error_number));\
		}while(0)
#else
	#define PRINT_ERROR(s)																					\
		do{}while(0)
#endif

#define PL_TRY(func)																							\
	/* TODO write code to return mutex related errors	*/											\
	do{ uint32_t pl_error;																					\
	pl_error = (func);																						\
	CATCH(pl_error != 0){																					\
		return DB_PRIORITIZED_LIST_ERROR;																\
	}} while (0)


#ifdef DEBUG
	#define PRINT_ERROR_STRING(error_string)															\
		do{																										\
			fprintf(fd, "\nERROR line %d of file %s by function %s:\n   Error : %s\n",			\
			__LINE__, __FILE__,__func__, error_string);												\
			}																										\
		while(0)
#else
	#define PRINT_ERROR_STRING(s)																			\
		do{}																										\
		while(0)
#endif

#ifdef DEBUG
	#define PRINT_ERROR_STRING_SQL(error_code, error_string)										\
		do{																										\
			fprintf(fd,"\nERROR line %d of file %s by function %s:\n   Error %d: %s\n",			\
			__LINE__, __FILE__,__func__, error_code, error_string);								\
			}																										\
		while(0)
#else
	#define PRINT_ERROR_STRING_SQL(c, s)																\
		do{};																										\
		while(0)
#endif

#define TRY(db_function)																			\
    do{																									\
        ERROR db_error = db_function;															\
        if(db_error){																				\
			  PRINT_ERROR(db_error);																\
			  return db_error; 																		\
		  }																								\
    }																										\
    while(0)

#define CATCH(condition)																			\
	if(condition)							
    
#define CATCH_SQL_ERROR(sqlite3_error)																				\
	do{																														\
		if(sqlite3_error != SQLITE_OK && sqlite3_error != SQLITE_DONE										\
			 && sqlite3_error != SQLITE_ROW){																		\
			PRINT_ERROR_STRING_SQL(sqlite3_error,sqlite3_errmsg(db));										\
			return DB_DATABASE_ERROR;																					\
		}																														\
	}while(0)

#define CATCH_NO_DB()																									\
	do{																														\
		if(db == NULL)																										\
			return DB_NO_DATABASE_SELECTED;																			\
	} while(0)
	
#define TRY_EXIT(func)																									\
	do{int return_value_func = 0;																						\
		return_value_func = func;																						\
		if(return_value_func != 0)																						\
			exit(1);																											\
	} while (0)


typedef sqlite3_stmt* statement_ptr;

/*********************************************************************************
**		Prototypes
**********************************************************************************/

static ERROR db_register_database( void );

static ERROR db_get_request_and_response_topic_id(DATABASE_ID* request_topic_id, DATABASE_ID* response_topic_id);

static ERROR db_semaphore_wait( void );

static ERROR db_semaphore_init( void );

static ERROR db_semaphore_destroy( void );

static void * db_output(void* input);

static ERROR db_message_handler(MESSAGE* m);

static ERROR db_error_handler(ERROR error_number);

static ERROR db_input_get_topic(MESSAGE* m, topic** topic);

static ERROR db_input_check_message_field(MESSAGE* m, FLAG *correct);

static ERROR db_error_log(char* topic, char* mes, MESSAGE* m);

static ERROR db_send_message_pipe(MESSAGE* m, int write_ptr);

static ERROR database_guardian(int database_pid, char* database_name);

static void db_exit_handler(int signum);

static ERROR database_main(char* database_name, int fd_in, int fd_out);

static void destroy_message(pl_data* data);

static pl_data copy_message(pl_data data);

static ERROR db_open(DATABASE db_string);

static void db_close(void);

static ERROR db_connect(ADDRESS address, char* client_id, time_t duration, FLAG will, FLAG clean);

static ERROR db_disconnect(ADDRESS address, SLEEP sleep, FLAG unexpected);

static ERROR db_subscribe(ADDRESS address, QOS qos, topic topic, EXTERN_ID extern_message_id);

static ERROR db_unsubscribe(ADDRESS address, topic topic);
					
static ERROR db_register(ADDRESS address, topic topic, EXTERN_ID extern_message_id);

static ERROR db_set_will_topic(ADDRESS address, FLAG retain, QOS qos, topic will_topic);

static ERROR db_set_will_message(ADDRESS address, MESSAGE* message);

static ERROR db_clear_will(ADDRESS address);

static ERROR db_clear_message(ADDRESS address);

static ERROR db_publish(ADDRESS address, MESSAGE* message);

static ERROR db_clear_subscribtion(ADDRESS address);

/*********************************************************************************
**		Private function implementation
**********************************************************************************/

/* allocates the space for a new message */
static ERROR db_create_new_message(MESSAGE** m);

/* parse the topic to a pad */
static ERROR db_parse_topic(topic t, stack* pad, uint32_t* number_of_wildcards);

/*********************************************************************************
**		Send functions
**********************************************************************************/

/* send the retain message for a device based on its id */
static ERROR db_send_retain_topic_device(DATABASE_ID device_id, DATABASE_ID topic_id);

/* send a message from the database to a device, send it if the device is reachable */
static ERROR db_send_database_message_device(DATABASE_ID device_id, DATABASE_ID topic_id, DATABASE_ID message_id);

/* send the message by allocating memory en sending it to the output buffer */
static ERROR db_send_message_address(ADDRESS address, MESSAGE_TYPE type, DATABASE_ID message_id, EXTERN_ID extern_message_id, EXTERN_ID short_topic_id, QOS qos, time_t duration, FLAG duplicate, return_code return_value, LENGTH length, const void* data);

/* send a message stored in the database to each device subscribed to that topic */
static ERROR db_send_message_topic(DATABASE_ID topic_id, DATABASE_ID message_id);

/*********************************************************************************
**		Set functions
**********************************************************************************/

/* set the connect for a device to the new values where the address is as specified */
static ERROR db_set_connect_address(ADDRESS address, DATABASE_ID device_id, time_t duration, FLAG clean);

/* change the qos for a device to the required qos based on its device id */
static ERROR db_set_qos_subscribtion(DATABASE_ID device_id, DATABASE_ID topic_id, QOS qos);

/* change the send message flag for a device to the required qos based on its device id */
static ERROR db_set_send_message_subscribtion(DATABASE_ID device_id, DATABASE_ID topic_id, FLAG send_message);

/*********************************************************************************
**		Get functions
**********************************************************************************/

/* get the next available external message id for a device */		
static ERROR db_get_next_extern_message_id(DATABASE_ID device_id, EXTERN_ID* extern_message_id);

/* get the topic id for a topic of a device based on its id */
static ERROR db_get_topic_id_device(DATABASE_ID device_id, topic t, TOPIC_ID* topic_id);

/* get the message out of the database based on the message id */
static ERROR db_get_message_database(DATABASE_ID message_id, blob* blob);

/* get the will message of a device based on its id */
static ERROR db_get_will_device(DATABASE_ID device_id, DATABASE_ID* topic_id, blob* data, FLAG* retain);

/* get a new short topic id that is not in use for a device based on its id*/
static ERROR db_get_new_short_topic_id(DATABASE_ID device_id, TOPIC_ID* short_topic_id);

/* get the incompleet will message from a device */
static ERROR db_get_incompleet_will_device(DATABASE_ID device_id, DATABASE_ID* will_id, DATABASE_ID* will_topic, FLAG* retain, QOS* qos);

/* get the topic id of a topic, you need to give the device_id if you provide the short topic id*/
static ERROR db_get_topic_id(topic t, DATABASE_ID device_id, DATABASE_ID* topic_id);

/* get the topic id based on topic id */
static ERROR db_get_topic_id_name(DATABASE_ID device_id, topic t, DATABASE_ID* topic_id);

/* get the topic id based on the predefined topic id */
static ERROR db_get_topic_id_predefined_topic_id(EXTERN_ID predefined_topic_id, DATABASE_ID* topic_id);

/* get the topic id based on the short topic id */
static ERROR db_get_topic_id_short_topic_id(EXTERN_ID short_topic_id, DATABASE_ID device_id, DATABASE_ID* topic_id);

/* get the device id based on the address, check first it is connected */
static ERROR db_get_device_id_address(ADDRESS address, DATABASE_ID* device_id);

/* get the device based on the client id, check first it exists */
static ERROR db_get_device_client_id( char* client_id, DATABASE_ID* device_id );

/* this functions gets the history for a certain toppic based on the message */
static ERROR db_get_history(MESSAGE* m);

/* writes the parameters to memory and sets the first_byte to the first empty byte */
static ERROR db_get_line_request(void** first_byte, uint32_t time, uint32_t length_client_id, void* client_id, uint32_t length_message, void* message);

/*********************************************************************************
**		Add functions
**********************************************************************************/

/* add a message to a topic based on a real message that came in */
static ERROR db_add_message_database_topic(DATABASE_ID topic_id, DATABASE_ID device_id, MESSAGE* m);

/* add a new message in the database, the message id will be set in the original message */
static ERROR db_add_message_database(DATABASE_ID topic_id, DATABASE_ID device_id, MESSAGE* m);

/* create a connection for the specified device based on its id */
static ERROR db_add_connect_device(DATABASE_ID device_id, time_t duration, ADDRESS address, FLAG clean);

/* add a topic to the database based on the topic name. Give it the id of the parent topic */
static ERROR db_add_topic(DATABASE_ID device_id, char* topic_name, DATABASE_ID parent_id);

/* add a device based on the client_id and return the device id*/
static ERROR db_add_device_client_id( char* client_id, DATABASE_ID* device_id);

/*********************************************************************************
**		Check functions to check for conditions
**********************************************************************************/

/* check if the will message is already completed for a device based on its id */
static ERROR db_check_will_message_completed(DATABASE_ID device_id, MESSAGE* message, FLAG* already_completed);

/* check if the will exists based on de device id */
static ERROR db_check_will_exists(DATABASE_ID device_id, FLAG* exists);

/* check if an incompleet will message exists for a device based on its id */
static ERROR db_check_incompleet_will_message(DATABASE_ID device_id, FLAG* incompleet_exists);

/* check if the incompleet will message is set for a device based on its id*/
static ERROR db_check_incomplete_will_is_set(DATABASE_ID device_id, DATABASE_ID topic_id, QOS qos, FLAG retain, FLAG* is_set);

/* check if the device is reachable based on its id */
static ERROR db_check_device_is_reachable(DATABASE_ID device_id, FLAG* is_reachable, ADDRESS* address);

/* get the information about a subscribtion if the device is subscribed. Based on its device id and the topic id */
static ERROR db_check_subscribed(DATABASE_ID device_id, DATABASE_ID topic_id, EXTERN_ID* short_topic_id, QOS* qos, FLAG* subscribed, FLAG* send_messages);

/* check if the topic exists, can handle every type */
static ERROR db_check_topic_exists(topic t, FLAG* is_free);

/* check if the client is  Free based on the topic name */
static ERROR db_check_client_free_topic_name(topic t, FLAG* is_free);

/* check if the client is  Free based on the predefined topic id */
static ERROR db_check_client_free_predefined_topic(EXTERN_ID predefince_topic_id, FLAG* is_free);

/* check if the client is  Free based on the short topic id */
static ERROR db_check_client_free_short_topic_id(EXTERN_ID short_topic_id, FLAG* is_free);

/* check that the short topic id is set for the device and is valid */
static ERROR db_check_short_topic_id_valid(EXTERN_ID short_topic_id, DATABASE_ID device_id, FLAG* valid);

/* check if the addres is still  Free */
static ERROR db_check_address_free(ADDRESS address, FLAG* is_free);

/* check if the client name is still  Free */
static ERROR db_check_client_free(char* client_id, FLAG* is_free);

/* check if the device is connected based on his address */
static ERROR db_check_address_connected(ADDRESS address, FLAG* is_connected);

/* check if the device is connected based on his id */
static ERROR db_check_device_connected(DATABASE_ID device_id, FLAG* is_connected);

/*********************************************************************************
**		Clear functions
**********************************************************************************/

/* remove the will message and the incompleet will message from a device based on its id */
static ERROR db_clear_all_will(DATABASE_ID device_id);

/* remove all the topic a device is subscribed to based on its id */
static ERROR db_clear_topic_device(DATABASE_ID device_id);

/* clear all the connections from a device based on its id */
static ERROR db_clear_connect_device(DATABASE_ID device_id);

/* clear all the connections based on an address */
static ERROR db_clear_connect_address(ADDRESS address);

/* remove all the messages from a device based on its id */
static ERROR db_clear_message_device(DATABASE_ID device_id);

/* clear the incompleet will messages from a device based on its id */
static ERROR db_clear_incompleet_will_device(DATABASE_ID device_id);

/* clear all the messages that are going out from the database for a device based on its id */
static ERROR db_clear_message_to_device(DATABASE_ID device);

/* clear all the messages that are comming in to the database for a device based on its id */
static ERROR db_clear_message_to_database(DATABASE_ID device);

/*********************************************************************************
**		returns an error message
**********************************************************************************/

char* db_get_error_message(ERROR error_number);

/*********************************************************************************
**		SQL functions
**********************************************************************************/

/*********************************************************************************
**		SQL statement wrapper
**********************************************************************************/

/* this statement wrapper function around prepare step and finalize */
static ERROR db_sql(char* sql_statement);

/*********************************************************************************
**		SQL prepare statements
**********************************************************************************/

/* sends a prepared statement to the database to be parsed */
static ERROR db_sql_prepare(statement_ptr *stmt_ptr, char* sql);

/*********************************************************************************
**		SQL bind data
**********************************************************************************/

/* binds NULL to a prepared statement */ //TODO check this behaviour
//static ERROR db_sql_bind_null(statement_ptr stmt_ptr, uint32_t index);

/* binds an integer to a prepared statement */
static ERROR db_sql_bind_int(statement_ptr stmt_ptr, uint32_t index, uint32_t input);

/* binds an integer of 64 to a prepared statement */
static ERROR db_sql_bind_int64(statement_ptr stmt_ptr, uint32_t index, uint64_t input);

/* binds a text to a prepared statement */
static ERROR db_sql_bind_text(statement_ptr stmt_ptr, uint32_t index, char* txt, uint32_t length, void (*destroy) (void*));

/* binds a blob to a prepared statement */ 
static ERROR db_sql_bind_blob(statement_ptr stmt_ptr, uint32_t index, const void* blob, uint32_t length, void(*destroy)(void*));

/*********************************************************************************
**		SQL execute statement
**********************************************************************************/

/* executes a statement */ 
static int db_sql_step(statement_ptr stmt_ptr, FLAG* result);

/*********************************************************************************
**		SQL get data
**********************************************************************************/

/* returns a integer from an executed statement */ 
static ERROR db_sql_get_int(statement_ptr stmt_ptr, uint32_t index, uint32_t* result);

/* returns a int of 64 bit from an executed statement */ 
static ERROR db_sql_get_int64(statement_ptr stmt_ptr, uint32_t index, uint64_t* result);

/* returns a blob of data from an executed statement */ 
static ERROR db_sql_get_text(statement_ptr stmt_ptr, uint32_t index, blob* result);

/* returns a blob of data from an executed statement */ 
static ERROR db_sql_get_blob(statement_ptr stmt_ptr, uint32_t index, blob* result);

/*********************************************************************************
**		SQL reset and finalize
**********************************************************************************/

/* resets a statement and prepares it to bind new parameters and execute again */
static ERROR db_sql_reset(statement_ptr stmt_ptr);

/* finalizes a statement if it is not used anymore */
static ERROR db_sql_finalize(statement_ptr stmt_ptr);




  
/**
 * globals
 **/

//TODO check the input of the public functions

	int semaphore;
	sqlite3* db;
	prioritized_list output = NULL;
	FILE* fd;
	FILE* fd_error;
	FLAG shutdown = 0;
	pthread_t db_output_thread;
	FLAG send_output_message = 0;
	DATABASE_ID database_device_id = 0;
	pthread_mutex_t output_thread_mutex;
	pthread_cond_t output_thread_condition;
	DATABASE_ID response_topic_id = 0;
	DATABASE_ID request_topic_id = 0;

/*********************************************************************************
**		Implementation
**********************************************************************************/
static void destroy_message(pl_data* data){
	if(((MESSAGE*) *data)->message != NULL){
	free(*((void**)&(((MESSAGE*) *data)->message)));
	}
	free(*data);
	*data = NULL;
}
 
static pl_data copy_message(pl_data data){
	MESSAGE* input = (MESSAGE*) data;
	MESSAGE* m = (MESSAGE*) calloc(1, sizeof(MESSAGE));
	m->address = input->address;
	m->message_id = input->message_id;
	m->extern_message_id = input->extern_message_id;
	m->short_topic_id = input->short_topic_id;
	m->type = input->type;
	m->topic_type = input->topic_type;
	m->qos = input->qos;
	m->duration = input->duration;
	m->connection_id = input->connection_id;
	m->duplicate = input->duplicate;
	m->will = input->will;
	m->clean = input->clean;
	m->retain = input->retain;
	m->return_value = input->return_value;
	m->length = input->length;
	m->message = input->message;

	return (pl_data) m;
}
 
/* fork a new process accessing the specified database. If an error occurs in the
database and it crashes, this function will attempt to reopin and restor it. */
/*-------------------------------------------------------------------------------*/
extern_error database_open(char* database_name, int* child_pid, char* output_file, int* file_descriptor_write, int* file_descriptor_read){
/*-------------------------------------------------------------------------------*/	
if(database_name == NULL || child_pid == NULL || output_file == NULL || file_descriptor_write == NULL || file_descriptor_read == NULL)
	return DATABASE_INVALID_INPUT;

TRY(db_semaphore_init());

int fd_in[2];
int fd_out[2];

int err_code = pipe(fd_in);
if(err_code != 0){
	return DATABASE_INITIALIZATION_ERROR;
}

err_code = pipe(fd_out);
if(err_code != 0){
	return DATABASE_INITIALIZATION_ERROR;
}

int return_code = 0;

/* block signals until initialization is done */
signal(SIGTERM, SIG_IGN);
signal(SIGQUIT, SIG_IGN); 

return_code = fork();
if(return_code <0){
	//TODO return the right pid error
	}

if(return_code == 0){

	fd_error = fopen(ERROR_FILE, "w");
	fd = fopen(output_file, "w");
//	FILE* fp = freopen(output_file, "w+", stdout);
	setlinebuf(fd);
	// the child must write in the out, and read out of the in
	close(fd_in[1]); // close the write descriptor to the input pipe
	close(fd_out[0]); // close the read descriptor to the output pipe
	
	database_main(database_name, fd_in[0], fd_out[1]);
	/* for safety, an unreachable exit */
	exit(1);
} else {
	
	signal(SIGTERM, SIG_DFL);
	signal(SIGQUIT, SIG_DFL);

	*child_pid = return_code;
	close(fd_in[0]); // close the read descriptor to the input pipe
//	close(fd_out[1]); // close the write descriptor to the output pipe
	*file_descriptor_write = fd_in[1];
	*file_descriptor_read = fd_out[0];
}
return DATABASE_OK;
}

/*-------------------------------------------------------------------------------*/
static ERROR database_guardian(int database_pid, char* database_name){
/*-------------------------------------------------------------------------------*/	
	//TODO make this run in a saperate thread
	int child_exit_status;
	printf("printing the database name: %s\n", database_name);
	waitpid(database_pid, &child_exit_status, 0);
	if(child_exit_status != 0){}
	return DB_OK;
}

/*-------------------------------------------------------------------------------*/
static void db_exit_handler(int signum){
/*-------------------------------------------------------------------------------*/	
	if(signum == SIGQUIT ){
		exit(0);
	}
	
	if(signum == SIGTERM){
		shutdown = 1;
	}
}

/*-------------------------------------------------------------------------------*/
static ERROR database_main(char* database_name, int fd_in, int fd_out){
/*-------------------------------------------------------------------------------*/	

	atexit(db_close);

	TRY_EXIT(create_prioritized_list(&output, 100000, destroy_message, copy_message));

	int * fd_out_ptr = (int*) malloc(sizeof(int));
	*fd_out_ptr = fd_out;

	pthread_mutex_init(&output_thread_mutex, NULL);
	pthread_cond_init(&output_thread_condition, NULL);
	pthread_create(&db_output_thread, NULL, db_output, (void*) fd_out_ptr);

	int temp = fd_out;
	temp++;

	shutdown = 0;

	signal(SIGTERM, db_exit_handler); // disconnect all devices and stop

	db_open(database_name);

	db_register_database();
	db_get_request_and_response_topic_id(&request_topic_id, &response_topic_id);

	signal(SIGQUIT, db_exit_handler); // just stop

	uint32_t bytes_read = 0;

	while(1){
		
		MESSAGE* m = (MESSAGE*) malloc(sizeof(MESSAGE));
		if(m == NULL){
			exit(0);
		}

		bytes_read = read(fd_in, m, sizeof(MESSAGE));
		if(bytes_read != sizeof(MESSAGE))
			db_error_log(ERROR_TOPIC_INVALID_INPUT, "The pipe returned not enough bytes!", NULL);

		if(m->length != 0){
			void* data = malloc(m->length);
			bytes_read = read(fd_in, data, m->length);
			if(bytes_read != m->length)
				db_error_log(ERROR_TOPIC_INVALID_INPUT, "The pipe returned not not enough bytes!", NULL);
			m->message = data;
		}

		db_error_handler(db_message_handler(m));
	}
}

static ERROR db_semaphore_init(){
	int error = -1;

	key_t key = ftok("/tmp", 'a');
	if( key == (key_t) -1)
		return DB_MEMORY_ALLOCATION_FAILED;

	
	/* allocate the semaphore */
	error = semget(key, 1, 0666 | IPC_CREAT);
	if(error == -1)
		return DB_MEMORY_ALLOCATION_FAILED;

	/* returning the key */
	semaphore = error;

	short values[1];
	values[0] = 0;
	semctl(semaphore, 0, SETALL, values);

	return DB_OK;
}

static ERROR db_semaphore_wait( void ){
	struct sembuf sem_b;
	sem_b.sem_num = 0;
	sem_b.sem_op = -1;
	sem_b.sem_flg = SEM_UNDO;

	int return_code = semop(semaphore, &sem_b, 1);
	if( return_code == -1)
		return DB_UNSPECIFIED_ERROR;
	return DB_OK;
}

extern_error database_request_message( void ){
	struct sembuf sem_b;
	sem_b.sem_num = 0;
	sem_b.sem_op = 1;
	sem_b.sem_flg = SEM_UNDO;

	int return_code = semop(semaphore, &sem_b, 1);
	if( return_code == -1)
		return DATABASE_SEMAPHORE_ERROR;
	return DATABASE_OK;
}

static ERROR db_semaphore_destroy( void ){
	short ignored_argument[1];
	if( semctl(semaphore, 1, IPC_RMID, ignored_argument) == -1)
		return DB_COULD_NOT_FREE;
	return DB_OK;
}

static void * db_output(void* input){

	int fd_out = *((int*) input);
		free(input);

	while(1){
		
		if( (db_semaphore_wait() != DB_OK) ){
			pthread_exit(NULL);
		}

		if(shutdown == 1){
			uint32_t count = 0;
			get_size_prioritized_list(output, &count);
			if( count == 0 ){
				exit(0);
			}
		}
		
		pl_data data;
		
		uint32_t count = 0;
		get_size_prioritized_list(output, &count);

		int32_t pl_error = get_data_blocking(output, &data);
		int32_t pipe_return = db_send_message_pipe((MESSAGE*) data, fd_out);
		if(pipe_return != 0 || pl_error != 0)
			exit(1);
	}
}

static ERROR db_error_handler(ERROR error_number){
	ERROR error;

	if(error_number == 0)
		return DB_OK;
	
	static DATABASE_ID topic_id = 0;
	static const char* topic_name = ERROR_TOPIC_INTERNAL_ERRORS;
	char* error_message = (char*) malloc(30 + strlen(db_get_error_message(error_number)));
	
	sprintf(error_message, "ERROR %3d: %s\n",	error_number, db_get_error_message(error_number));

	if(topic_id == 0){
		topic* t = (topic*) malloc(sizeof(topic));
		
		t->topic_name = malloc(strlen(topic_name+1));
		strcpy((char*)(t->topic_name), topic_name);
		*((char*)(t->topic_name) + strlen(topic_name)) = '\0';
		t->topic_length = strlen(topic_name) + 1;
		t->topic_type = 0;
		
		error = db_get_topic_id(*t, database_device_id, (DATABASE_ID*) &topic_id);
		free(t->topic_name);
		free(t);

		if(error != 0){
			fwrite(error_message, strlen(error_message + 1), 1, fd_error);
			return DB_OK;
		}
	}

	MESSAGE* m = (MESSAGE*) malloc(sizeof(MESSAGE));
	m->length = strlen(error_message + 1);
	m->message = error_message;
	m->retain = 0;
	
	error = db_add_message_database_topic(topic_id, database_device_id, m);
	
	if(error != 0){
		fwrite(error_message, strlen(error_message + 1), 1, fd_error);
		return DB_OK;
	}

	return DB_OK;
}

static ERROR db_register_database( void ){
	FLAG is_free = 0;
	TRY(db_check_client_free("database", &is_free));

	FLAG address_is_free = 0;
	TRY(db_check_address_free(1, &address_is_free));

	if(is_free){
		if(!address_is_free){
#ifdef AUTO_DISCONNECT
			TRY(db_disconnect(1, 0, 1));
#else
			return DB_ADDRESS_ALREADY_USED;
#endif
		}
	} else {
		return DB_IMPLEMENTATION_ERROR;	
	}

	TRY(db_add_device_client_id("database", &database_device_id));
	TRY(db_add_connect_device(database_device_id, 0, 1, 0));

	return DB_OK;
}

static ERROR db_get_request_and_response_topic_id(DATABASE_ID* request_topic_id, DATABASE_ID* response_topic_id){
	topic* t = (topic*) malloc(sizeof(topic));
	if(t == NULL)
		return DB_MEMORY_ALLOCATION_FAILED;
	
	t->topic_type = 0;
	t->topic_length = strlen(REQUEST_TOPIC) + 1;
	asprintf((char**) &(t->topic_name), REQUEST_TOPIC);
	TRY(db_get_topic_id_name(database_device_id, *t, request_topic_id));
	free(t->topic_name);

	t->topic_type = 0;
	t->topic_length = strlen(RESPONSE_TOPIC) + 1;
	asprintf((char**) &(t->topic_name), RESPONSE_TOPIC);
	TRY(db_get_topic_id_name(database_device_id, *t, response_topic_id));
	free(t->topic_name);

	return DB_OK;
}

static ERROR db_send_message_pipe(MESSAGE* m, int write_ptr){
	uint32_t written = write(write_ptr, m, sizeof(MESSAGE));
	if(written != sizeof(MESSAGE)){
		return DB_PIPE_ERROR;
	} 
	
	if(m->length != 0){
		written = write(write_ptr, m->message, m->length);
		if(written != m->length){
			return DB_PIPE_ERROR;
		} else {
			//TODO don't  Free but reuse
			if(m->message == NULL)
				free((void*) m->message);
			free((void*)m);
		}
	} else {
		     free((void*)m);
	}
	
	return DB_OK;
}

// TODO sleep: only new outgoing are impossible, new incomming -> no problem

/*-------------------------------------------------------------------------------*/
static ERROR db_message_handler(MESSAGE* m){
/*-------------------------------------------------------------------------------*/	
	/*the struct for reference

	typedef struct Message{
	ADDRESS address;
	DATABASE_ID message_id;
	EXTERN_ID extern_message_id;
	EXTERN_ID short_topic_id;
	MESSAGE_TYPE type;
	TOPIC_TYPE topic_type;
	QOS qos;
	time_t duration;
	FLAG timeout;
	FLAG duplicate;
	FLAG will;
	FLAG clean;
	FLAG retain;
	return_code return_value;
	LENGTH length;
	const void* message;
 } MESSAGE;
 
 typedef struct Topic{
	TOPIC_TYPE topic_type;
	LENGTH topic_length;
	void* topic_name;
 } topic;
 */

 //TODO change the TRY by HANDLE -->error handler

	FLAG correct = 0;
	topic* new_topic = NULL;
	switch(m->type){
		case ADVERTISE:
		case SEARCHGW:
		case GWINFO:
		case RESERVED_3:
			TRY(db_error_log(ERROR_TOPIC_INVALID_INPUT, "Message not supported by the database", m));
			break;
		case CONNECT:;
			correct = 0;
			TRY(db_input_check_message_field(m, &correct));
			if(correct && m->length != 0){
				TRY(db_connect(m->address, (char*) m->message , m->duration, m->will, m->clean));
			}
			break;
		case CONNACK:
		case WILLTOPICREQ:
			TRY(db_error_log(ERROR_TOPIC_INVALID_INPUT, "Message not supported by the database", m));
			break;
		case WILLTOPIC:;
			new_topic = NULL;
			TRY(db_input_get_topic(m, &new_topic));
			if(new_topic != NULL){
				TRY(db_set_will_topic(m->address, m->retain, m->qos, *new_topic));
				     free( new_topic );
			}
			break;
		case WILLMSGREQ:
			TRY(db_error_log(ERROR_TOPIC_INVALID_INPUT, "Message not supported by the database", m));
			break;
		case WILLMSG:;
			correct = 0;
			TRY(db_input_check_message_field(m, &correct));
			if(correct){
				//TODO check that the message is not  Freed!
				TRY(db_set_will_message(m->address, m));
			}
			break;
		case REGISTER:
			//TODO make sure the topic is not  Freed by the function
			new_topic = NULL;
			TRY(db_input_get_topic(m, &new_topic));
			if(new_topic != NULL){
				TRY(db_register(m->address, *new_topic, m->extern_message_id));
				    free( new_topic );
			}
			break;
		case REGACK:
			//TODO handle this
			TRY(db_error_log("input/not_supported", "Message not yet supported by the database, support will come soon", m));
			break;
		case PUBLISH:
			TRY(db_input_check_message_field(m, &correct));
			if(correct){
				TRY(db_publish(m->address, m));
			}
			break;
		case PUBACK:
		case PUBCOMP:
		case PUBREC:
		case PUBREL:
			//TODO handle this
			TRY(db_error_log("input/not_supported", "Message not yet supported by the database, support will come soon", m));
			break;
		
		case RESERVED_17:
			TRY(db_error_log(ERROR_TOPIC_INVALID_INPUT, "Message not supported by the database", m));
			break;
		
		case SUBSCRIBE:
			//TODO make sure the topic is not  Freed by the function
			
			new_topic = NULL;
			TRY(db_input_get_topic(m, &new_topic));
			if(new_topic != NULL){
				TRY(db_subscribe(m->address, m->qos, *new_topic, m->extern_message_id));
				    free( new_topic );
			}
			break;

		case SUBACK:
			TRY(db_error_log(ERROR_TOPIC_INVALID_INPUT, "Message not supported by the database", m));
			break;
		
		case UNSUBSCRIBE:
			new_topic = NULL;
			TRY(db_input_get_topic(m, &new_topic));
			if(new_topic != NULL){
				TRY(db_unsubscribe(m->address, *new_topic));
				    free( new_topic );
			}
			break;
	
		case UNSUBACK:
			TRY(db_error_log(ERROR_TOPIC_INVALID_INPUT, "Message not supported by the database", m));
			break;

		case PINGREQ:
			//TODO handle this
			TRY(db_error_log("input/not_supported", "Message not yet supported by the database, support will come soon", m));
			break;

		case PINGRESP:
			TRY(db_error_log(ERROR_TOPIC_INVALID_INPUT, "Message not supported by the database", m));
			break;

		case DISCONNECT:
			TRY(db_disconnect(m->address, m->duration, 0));
			break;

		case RESERVED_25:
			TRY(db_error_log(ERROR_TOPIC_INVALID_INPUT, "Message not supported by the database", m));
			break;

		case WILLTOPICUPD:
		case WILLTOPICRESP:
		case WILLMSGUPD:
		case WILLMSGRESP:
			//TODO handle this
				TRY(db_error_log("input/not_supported", "Message not yet supported by the database, support will come soon", m));
			break;
		default:
				TRY(db_error_log(ERROR_TOPIC_INVALID_INPUT, "Message not supported by the database", m));
				break;
	}

	return DB_OK;
}

/* returns NULL in case of failure */
/*-------------------------------------------------------------------------------*/
static ERROR db_input_get_topic(MESSAGE* m, topic** topic_ptr){
/*-------------------------------------------------------------------------------*/	
	*topic_ptr = NULL;
	//TODO make sure the topic is not  Freed by the function
	FLAG correct = 0;
	TRY(db_input_check_message_field(m, &correct));
	if(correct){
		topic* new_topic = (topic*) malloc(sizeof(topic));
		if(new_topic == NULL){
			//TODO write a handler if malloc failes	
		} else {
			new_topic->topic_type = m->topic_type;
			new_topic->topic_length = m->length;
			new_topic->topic_name = (void*) m->message;
			*topic_ptr = new_topic;
		}
	}

	return DB_OK;
}
	
/*-------------------------------------------------------------------------------*/
static ERROR db_input_check_message_field(MESSAGE* m, FLAG *correct){
/*-------------------------------------------------------------------------------*/	
	*correct = 0;
	if((m->length == 0 && m->message != NULL) || (m->length != 0 && m->message == NULL)){
		TRY(db_error_log(ERROR_TOPIC_INVALID_INPUT, "Message containes conflicting information concerning the message or length", m));
	} else {
		*correct = 1;
	}
	return DB_OK;
}

/*-------------------------------------------------------------------------------*/
static ERROR db_error_log(char* topic, char* msg, MESSAGE* m){
/*-------------------------------------------------------------------------------*/	
	printf("the topic is %s, the messate is %s, message ptr %p\n", topic, msg, m);
	return DB_OK;
}

/*-------------------------------------------------------------------------------*/
static ERROR db_open(DATABASE db_string){
/*-------------------------------------------------------------------------------*/

	uint32_t sqlite3_error = sqlite3_open(db_string, &db);
	CATCH( sqlite3_error ){
		return DB_CANNOT_OPEN_DATABASE;
	}

	char* sql_create_table_device = 
	"CREATE TABLE IF NOT EXISTS device(								\
	device_id	INTEGER PRIMARY KEY AUTOINCREMENT,				\
	client_id	TEXT NOT NULL,											\
	size	INTEGER NOT NULL,												\
	added_on		INTEGER NOT NULL,										\
	valid			INTEGER NOT NULL);"									;

	char* sql_create_table_topic = 
	"CREATE TABLE IF NOT EXISTS topic(								\
	topic_id	INTEGER PRIMARY KEY AUTOINCREMENT,					\
	name	TEXT NOT NULL,													\
	added_on		INTEGER NOT NULL,										\
	parent_topic	INTEGER,												\
	created_by INTEGER NOT NULL,										\
	valid	INTEGER NOT NULL,												\
	predefined	INTEGER NOT NULL,										\
	predefined_topic_id	INTEGER,										\
	FOREIGN KEY(created_by) REFERENCES device(device_id));"	;

	char* sql_create_table_message =
	"CREATE TABLE IF NOT EXISTS message(							\
	message_id INTEGER PRIMARY KEY AUTOINCREMENT,				\
	topic_id INTEGER NOT NULL,											\
	added_by	INTEGER	NOT NULL,										\
	size	INTEGER NOT NULL,												\
	data	BLOB	NOT NULL,												\
	added_on	INTEGER NOT NULL,											\
	retain	INTEGER	NOT NULL,										\
	valid	INTEGER NOT NULL,												\
	FOREIGN KEY(topic_id) REFERENCES topic(topic_id),			\
	FOREIGN KEY(added_by) REFERENCES device(device_id));"		;

	char* sql_create_table_will = 
	"CREATE TABLE IF NOT EXISTS will(								\
	will_id	INTEGER PRIMARY KEY AUTOINCREMENT,					\
	device_id INTEGER NOT NULL,										\
	topic_id INTEGER,														\
	message_id INTEGER,													\
	qos		INTEGER NOT NULL,											\
	added_on	INTEGER,														\
	valid	INTEGER NOT NULL,												\
	incompleet INTEGER NOT NULL,										\
	retain INTERGER NOT NULL,											\
	FOREIGN KEY(device_id) REFERENCES device(device_id),		\
	FOREIGN KEY(topic_id) REFERENCES topic(topic_id),			\
	FOREIGN KEY(message_id) REFERENCES message(message_id));";

	char* sql_create_table_topic_device = 
	"CREATE TABLE IF NOT EXISTS topic_device(						\
	topic_device_id INTEGER PRIMARY KEY AUTOINCREMENT,			\
	device_id INTEGER NOT NULL,										\
	topic_id INTEGER NOT NULL,											\
	short_topic_id	INTEGER NOT NULL,									\
	send_messages INTEGER NOT NULL,									\
	valid INTEGER NOT NULL,												\
	qos INTEGER NOT NULL,												\
	FOREIGN KEY(device_id) REFERENCES device(device_id),		\
	FOREIGN KEY(topic_id) REFERENCES topic(topic_id));"		;

	char* sql_create_table_connect =
	"CREATE TABLE IF NOT EXISTS connect(							\
	connect_id	INTEGER PRIMARY KEY AUTOINCREMENT,				\
	device_id	INTEGER NOT NULL,										\
	duration	INTEGER NOT NULL,											\
	valid	INTEGER NOT NULL,												\
	added_on	INTEGER NOT NULL,											\
	address INTEGER NOT NULL,											\
	clean	INTEGER NOT NULL,												\
	awake INTEGER NOT NULL,												\
	FOREIGN KEY(device_id) REFERENCES device(device_id));"	;

	char* sql_create_table_message_device =
	"CREATE TABLE IF NOT EXISTS message_device(					\
	message_device_id	INTEGER PRIMARY KEY AUTOINCREMENT,		\
	to_database	INTEGER NOT NULL,										\
	device_id	INTEGER NOT NULL,										\
	message_id	INTEGER NOT NULL,										\
	extern_message_id	INTEGER NOT NULL,								\
	qos	INTEGER NOT NULL,												\
	state	INTEGER NOT NULL,												\
	last_change	INTEGER NOT NULL,										\
	added_on	INTEGER NOT NULL,											\
	valid INTEGER NOT NULL,												\
	FOREIGN KEY(device_id) REFERENCES device(device_id),		\
	FOREIGN KEY(message_id) REFERENCES message(message_id));";

	TRY(db_sql(sql_create_table_device));
	TRY(db_sql(sql_create_table_topic));
	TRY(db_sql(sql_create_table_message));
	TRY(db_sql(sql_create_table_will));
	TRY(db_sql(sql_create_table_topic_device));
	TRY(db_sql(sql_create_table_connect));
	TRY(db_sql(sql_create_table_message_device));
	
	//TRY(db_get_message_in());
	//TRY(db_get_message_out());

	return DB_OK;
}

/*-------------------------------------------------------------------------------*/
static void db_close(void){
/*-------------------------------------------------------------------------------*/
	//TODO handle errors here
	fclose(fd);
	destroy_prioritized_list( &output);
	pthread_mutex_destroy( &output_thread_mutex);
	pthread_cond_destroy( &output_thread_condition);
	sqlite3_close(db);
	fclose(fd_error);
	db_semaphore_destroy();
	printf("bye!\n");
}

/* duplicate messages will not be detected by the database because 
no timers can be used on buffered messages, also check the protocol*/

/*-------------------------------------------------------------------------------*/
static ERROR db_connect(ADDRESS address, char* client_id, time_t duration, FLAG will, FLAG clean){
/*-------------------------------------------------------------------------------*/
		//TODO check that all the functions that use the connect table check if the device is asleep, you can't send messages then
	DATABASE_ID device_id = 0;

	FLAG is_free = 0;
	TRY(db_check_client_free(client_id, &is_free));

	FLAG address_is_free = 0;
	TRY(db_check_address_free(address, &address_is_free));

	if(is_free){
		if(!address_is_free){
#ifdef AUTO_DISCONNECT
			TRY(db_disconnect(address, 0, 1));
#else
			return DB_ADDRESS_ALREADY_USED;
#endif
		}

		TRY(db_add_device_client_id(client_id, &device_id));
		TRY(db_add_connect_device(device_id, duration, address, clean));
	} else {
		
		TRY(db_get_device_client_id( client_id, &device_id ));

		if(!address_is_free){
			DATABASE_ID device_owning_the_address_id = 0;
			TRY(db_get_device_id_address(address, &device_owning_the_address_id));
			
			if(device_id != device_owning_the_address_id){
#ifdef AUTO_DISCONNECT
				TRY(db_disconnect(address, 0, 1));
#else
				return DB_ADDRESS_ALREADY_USED;
#endif
				TRY(db_add_connect_device(device_id, duration, address, clean));
			} else {
				TRY(db_set_connect_address(device_id, device_id, duration, clean));
			}
		}
	}

	if(clean){
		TRY(db_clear_all_will(device_id));
		TRY(db_clear_topic_device(device_id));
		TRY(db_clear_message_device(device_id));
	} 

	if(will){
		TRY(db_send_message_address(address, WILLTOPICREQ, 0, 0, 0, 0, 0, 0, 0, 0, NULL));
	} else {
		TRY(db_send_message_address(address, CONNACK, 0, 0, 0, 0, 0, 0, ACCEPTED, 0, NULL));
	}

	return DB_OK;

}

/*-------------------------------------------------------------------------------*/
static ERROR db_disconnect(ADDRESS address, SLEEP sleep, FLAG unexpected){
/*-------------------------------------------------------------------------------*/
	DATABASE_ID device_id;
	TRY(db_get_device_id_address(address, &device_id));

	//TODO get rit of all the messages that are being send to this device, it takes for ever to compleet al the messages, high priority?

	if(sleep != 0){
		char* sql_sleep_device_prepare = "UPDATE connect SET duration = ?  awake = 0 WHERE device_id = ? AND valid = 1;";
		//TODO check that all the functions that use the connect table check if the device is asleep, you can't send messages then

		statement_ptr disconnect_device;

		TRY(db_sql_prepare(&disconnect_device, sql_sleep_device_prepare));
		TRY(db_sql_bind_int(disconnect_device, 1, sleep));
		TRY(db_sql_bind_int(disconnect_device, 2, device_id));
		TRY(db_sql_step(disconnect_device, NULL));
		TRY(db_sql_finalize(disconnect_device));

		TRY(db_send_message_address(address, DISCONNECT, 0, 0, 0, 0, 0, 0, 0, 0, NULL));

	} else {
		char* sql_disconnect_device_prepare = "UPDATE connect SET valid = 0 WHERE device_id = ? AND valid = 1;";

		statement_ptr disconnect_device;

		TRY(db_sql_prepare(&disconnect_device, sql_disconnect_device_prepare));
		TRY(db_sql_bind_int(disconnect_device, 1, device_id));
		TRY(db_sql_step(disconnect_device, NULL));
		TRY(db_sql_finalize(disconnect_device));

		if(unexpected)
		{
			//TODO send the will topic;

		//TODO send the right message
		}

	}
	
	return DB_OK;
}

/*-------------------------------------------------------------------------------*/
static ERROR db_register(ADDRESS address, topic topic, EXTERN_ID extern_message_id){
/*-------------------------------------------------------------------------------*/
	DATABASE_ID device_id;
	TRY(db_get_device_id_address(address, &device_id));

	DATABASE_ID topic_id;
	TRY(db_get_topic_id(topic, device_id, &topic_id));
	
	QOS db_qos = 0;
	FLAG subscribed = 0;
	EXTERN_ID short_topic_id = 0;
	FLAG send_messages = 0;

	TRY(db_check_subscribed(device_id, topic_id, &short_topic_id, &db_qos, &subscribed, &send_messages));

	if(!subscribed){
		TRY(db_get_new_short_topic_id(device_id, &short_topic_id));
		char* sql_subscribe_prepare = "INSERT INTO topic_device (device_id, topic_id, short_topic_id, valid, qos, send_messages) VALUES (?, ?, ?, 1, 0, 0);";
		
		statement_ptr subscribe;
	
		TRY(db_sql_prepare(&subscribe, sql_subscribe_prepare));
		TRY(db_sql_bind_int(subscribe, 1, device_id));
		TRY(db_sql_bind_int(subscribe, 2, topic_id));
		TRY(db_sql_bind_int(subscribe, 3, short_topic_id));
		TRY(db_sql_step(subscribe, NULL));
		TRY(db_sql_finalize(subscribe));

		TRY(db_check_subscribed(device_id, topic_id, &short_topic_id, &db_qos, &subscribed, &send_messages));
		if(subscribed == 0)
			return DB_IMPLEMENTATION_ERROR;
	}

	TRY(db_send_message_address(address, REGACK, 0, extern_message_id, short_topic_id, 0, 0, 0, ACCEPTED, 0, NULL));
	return DB_OK;
}

/*-------------------------------------------------------------------------------*/
static ERROR db_subscribe(ADDRESS address, QOS qos, topic topic, EXTERN_ID extern_message_id){
/*-------------------------------------------------------------------------------*/
	DATABASE_ID device_id;
	TRY(db_get_device_id_address(address, &device_id));

	DATABASE_ID topic_id;
	TRY(db_get_topic_id(topic, device_id, &topic_id));
	
	QOS db_qos = 0;
	FLAG subscribed = 0;
	EXTERN_ID short_topic_id = 0;
	FLAG send_messages = 0;
	TRY(db_check_subscribed(device_id, topic_id, &short_topic_id, &db_qos, &subscribed, &send_messages));

	if(db_qos != qos){
		TRY(db_set_qos_subscribtion(device_id, topic_id, qos));
	}
	
	if(send_messages == 0){
		TRY(db_set_send_message_subscribtion(device_id, topic_id, 1));
	}

	if(!subscribed){
		TRY(db_get_new_short_topic_id(device_id, &short_topic_id));
		char* sql_subscribe_prepare = "INSERT INTO topic_device (device_id, topic_id, short_topic_id, valid, qos, send_messages) VALUES (?, ?, ?, 1, ?, 1);";
		
		statement_ptr subscribe;
	
		TRY(db_sql_prepare(&subscribe, sql_subscribe_prepare));
		TRY(db_sql_bind_int(subscribe, 1, device_id));
		TRY(db_sql_bind_int(subscribe, 2, topic_id));
		TRY(db_sql_bind_int(subscribe, 3, short_topic_id));
		TRY(db_sql_bind_int(subscribe, 4, qos));
		TRY(db_sql_step(subscribe, NULL));
		TRY(db_sql_finalize(subscribe));

		TRY(db_check_subscribed(device_id, topic_id, &short_topic_id, &db_qos, &subscribed, &send_messages));
		if(subscribed != 1 && db_qos != qos && send_messages != 1)
			return DB_IMPLEMENTATION_ERROR;
	}
	
	TRY(db_send_message_address(address, SUBACK, 0, extern_message_id, short_topic_id, 0, 0, 0, ACCEPTED, 0, NULL));
	TRY(db_send_retain_topic_device(device_id, topic_id));
	return DB_OK;
}

/*-------------------------------------------------------------------------------*/
static ERROR db_unsubscribe(ADDRESS address, topic topic){
/*-------------------------------------------------------------------------------*/
	
	DATABASE_ID device_id;
	TRY(db_get_device_id_address(address, &device_id));

	DATABASE_ID topic_id;
	TRY(db_get_topic_id(topic, device_id, &topic_id));
	
	QOS db_qos = 0;
	FLAG subscribed = 0;
	EXTERN_ID short_topic_id = 0;
	FLAG send_messages = 0;
	TRY(db_check_subscribed(device_id, topic_id, &short_topic_id, &db_qos, &subscribed, &send_messages));

	char* sql_unsubscribe_prepare = "UPDATE topic_device SET send_messages = 0 WHERE device_id = ? AND topic_id = ? AND valid = 1;";

	statement_ptr unsubscribe;

	TRY(db_sql_prepare(&unsubscribe, sql_unsubscribe_prepare));
	TRY(db_sql_bind_int(unsubscribe, 1, device_id));
	TRY(db_sql_bind_int(unsubscribe, 2, topic_id));
	TRY(db_sql_step(unsubscribe, NULL));
	TRY(db_sql_finalize(unsubscribe));

	return DB_OK;
}

/*-------------------------------------------------------------------------------*/
static ERROR db_set_will_topic(ADDRESS address, FLAG retain, QOS qos, topic will_topic){
/*-------------------------------------------------------------------------------*/
	
	DATABASE_ID device_id;
	TRY(db_get_device_id_address(address, &device_id));

	DATABASE_ID topic_id;
	TRY(db_get_topic_id(will_topic, device_id, &topic_id));

	FLAG is_set = 0;
	TRY(db_check_incomplete_will_is_set(device_id, topic_id, qos, retain, &is_set));

	if(!is_set){
		TRY(db_clear_incompleet_will_device(device_id));

		char* sql_set_will_topic_prepare = "INSET INTO will (device_id, topic_id, message_id, valid, qos, added_on, incompleet, retain) VALUES (?, ?, 0, 1, ?, ?, 1, ?);";
		statement_ptr set_will;

		TRY(db_sql_prepare(&set_will, sql_set_will_topic_prepare));
		TRY(db_sql_bind_int(set_will, 1, device_id));
		TRY(db_sql_bind_int(set_will, 2, topic_id));
		TRY(db_sql_bind_int(set_will, 3, qos));
		TRY(db_sql_bind_int(set_will, 4, time(NULL)));
		TRY(db_sql_bind_int(set_will, 5, retain));
		TRY(db_sql_step(set_will, NULL));
		TRY(db_sql_finalize(set_will));
	}

	TRY(db_send_message_address(address, WILLMSGREQ, 0, 0, 0, 0, 0, 0, 0, 0, 0));

	return DB_OK;

}

/*-------------------------------------------------------------------------------*/
static ERROR db_set_will_message(ADDRESS address, MESSAGE* message){
/*-------------------------------------------------------------------------------*/
	DATABASE_ID device_id;
	TRY(db_get_device_id_address(address, &device_id));

	FLAG incompleet_exists = 1;
	TRY(db_check_incompleet_will_message(device_id, &incompleet_exists));

	if(!incompleet_exists){
		FLAG already_completed = 0;
		TRY(db_check_will_message_completed(device_id, message, &already_completed));
		if(!already_completed)
 			return DB_WILL_TOPIC_NOT_SET;
	
		TRY(db_send_message_address(address, CONNACK, 0, 0, 0, 0, 0, 0, ACCEPTED, 0, NULL));
		return DB_OK;
	}
		
	DATABASE_ID will_id = 0;
	DATABASE_ID will_topic = 0;
	DATABASE_ID message_id = 0;
	FLAG retain = 0;
	message->qos = 0;

	TRY(db_get_incompleet_will_device(device_id, &will_id, &will_topic, &retain, &(message->qos)));
 	  
//	TRY(db_message_in(message, will_topic, 1, retain));

	//TODO set this message to the topic
	message_id = message->message_id;
	CATCH( message_id == 0)
		return DB_IMPLEMENTATION_ERROR;
	
	TRY(db_clear_will(device_id));
   char* sql_set_will_prepare = "UPDATE will SET incompleet = 0, message_id = ?, added_on = ? WHERE will_id = ? AND valid = 1 AND incompleet = 1;";
	
	statement_ptr set_will;

	TRY(db_sql_prepare(&set_will, sql_set_will_prepare));
	TRY(db_sql_bind_int(set_will, 1, message_id));
	TRY(db_sql_bind_int(set_will, 2, time(NULL)));
	TRY(db_sql_bind_int(set_will, 3, will_id));
	TRY(db_sql_step(set_will, NULL));
	TRY(db_sql_finalize(set_will));

	TRY(db_send_message_address(address, CONNACK, 0, 0, 0, 0, 0, 0, ACCEPTED, 0, NULL));
	return DB_OK;
}

/*-------------------------------------------------------------------------------*/
static ERROR db_clear_will(ADDRESS address){
/*-------------------------------------------------------------------------------*/
	DATABASE_ID device_id;
	TRY(db_get_device_id_address(address, &device_id));
 
	char* sql_clear_subscribtion_prepare = "UPDATE WILL SET valid = 0 WHERE device_id = ? AND incompleet = 0;";
	statement_ptr clear_subscribtion;

	TRY(db_sql_prepare(&clear_subscribtion, sql_clear_subscribtion_prepare));
	TRY(db_sql_bind_int(clear_subscribtion, 1, device_id));
	TRY(db_sql_step(clear_subscribtion, NULL));
	TRY(db_sql_finalize(clear_subscribtion));

	return DB_OK;
}

/*-------------------------------------------------------------------------------*/
static ERROR db_clear_message(ADDRESS address){
/*-------------------------------------------------------------------------------*/
    DATABASE_ID device_id;
    TRY(db_get_device_id_address(address, &device_id));
    TRY(db_clear_message_to_device(device_id));
    TRY(db_clear_message_to_database(device_id));
	 
	 return DB_OK;
}

/*-------------------------------------------------------------------------------*/
static ERROR db_publish(ADDRESS address, MESSAGE* m){
/*-------------------------------------------------------------------------------*/
	DATABASE_ID device_id;
	TRY(db_get_device_id_address(address, &device_id));

	FLAG valid = 0;
	TRY(db_check_short_topic_id_valid(m->short_topic_id, device_id, &valid));

	if(valid == 0){
		TRY(db_send_message_address(address, PUBACK, 0, m->extern_message_id, m->short_topic_id, 0, 0, 0, INVALID_TOPIC_ID, 0, NULL));
		TRY(db_error_log(ERROR_TOPIC_INVALID_INPUT, "the short topic id is not registerd for this device at this moment", m));
		return DB_OK;
	}
	
	DATABASE_ID topic_id = 0;
	TRY(db_get_topic_id_short_topic_id(m->short_topic_id, device_id, &topic_id));
	TRY(db_add_message_database_topic(topic_id, device_id, m));
	return DB_OK;
}

/*-------------------------------------------------------------------------------*/
static ERROR db_clear_subscribtion(ADDRESS address){
/*-------------------------------------------------------------------------------*/
	DATABASE_ID device_id;
	TRY(db_get_device_id_address(address, &device_id));
 
	char* sql_clear_subscribtion_prepare = "UPDATE TOPIC_DEVICE SET valid = 0 WHERE device_id = ?;";
	statement_ptr clear_subscribtion;

	TRY(db_sql_prepare(&clear_subscribtion, sql_clear_subscribtion_prepare));
	TRY(db_sql_bind_int(clear_subscribtion, 1, device_id));
	TRY(db_sql_step(clear_subscribtion, NULL));
	TRY(db_sql_finalize(clear_subscribtion));

	return DB_OK;
}


/*********************************************************************************
**		Private function implementation
**********************************************************************************/

/* allocates the space for a new message */
/*-------------------------------------------------------------------------------*/
static ERROR db_create_new_message(MESSAGE** m){
/*-------------------------------------------------------------------------------*/
	*m = NULL;
	MESSAGE* tmp = (MESSAGE*) calloc(1, sizeof(MESSAGE));
	if(tmp == NULL)
		return DB_MEMORY_ALLOCATION_FAILED;
	*m = tmp;
	return DB_OK;
}

/* parse the topic to a pad */
/*-------------------------------------------------------------------------------*/
static ERROR db_parse_topic(topic t, stack* pad, uint32_t* number_of_wildcards){
/*-------------------------------------------------------------------------------*/
	if(t.topic_type != 0 || number_of_wildcards == NULL)
		return DB_INVALID_INPUT;

	*number_of_wildcards = 0;

	TRY(parser_string_to_pad(t.topic_length, (char*) t.topic_name, pad, number_of_wildcards));
	return DB_OK;
}

/*********************************************************************************
**		Send functions
**********************************************************************************/

/*
the struct for easy reference:
 
typedef struct Message{
	ADDRESS address;
	DATABASE_ID message_id;
	EXTERN_ID extern_message_id;
	EXTERN_ID short_topic_id;
	MESSAGE_TYPE type;
	QOS qos;
	time_t duration;
	FLAG duplicate;
	return_code return_value;
	LENGTH length;
	const void* message;
 } MESSAGE;
*/

/* send the retain message for a device based on its id */
/*-------------------------------------------------------------------------------*/
static ERROR db_send_retain_topic_device(DATABASE_ID device_id, DATABASE_ID topic_id){
/*-------------------------------------------------------------------------------*/
	//TODO correct this
	char* sql_get_retain_prepare = "SELECT message_id, added_on FROM message WHERE topic_id = ? AND retain = 1 AND valid = 1 ORDER BY added_on DESC LIMIT 1;";
	
	statement_ptr get_retain;

	FLAG result = 0;
	TRY(db_sql_prepare(&get_retain, sql_get_retain_prepare));
	TRY(db_sql_bind_int(get_retain, 1, topic_id));
	TRY(db_sql_step(get_retain, &result));

	if(result == 0){
		TRY(db_sql_finalize(get_retain));
		return DB_OK;
	}

	DATABASE_ID message_id = 0;

	TRY(db_sql_get_int(get_retain, 0, &message_id));
		
	TRY(db_sql_step(get_retain, &result));
	if(result == 1){
		TRY(db_sql_finalize(get_retain));
		return DB_IMPLEMENTATION_ERROR;
	}

	TRY(db_sql_finalize(get_retain));

	TRY(db_send_database_message_device(device_id, topic_id, message_id));
	return DB_OK;
}

/* send a message from the database to a device, send it if the device is reachable */
/*-------------------------------------------------------------------------------*/
static ERROR db_send_database_message_device(DATABASE_ID device_id, DATABASE_ID topic_id, DATABASE_ID message_id){
/*-------------------------------------------------------------------------------*/

	FLAG is_subscribed = 0;
	QOS qos = 0;
	EXTERN_ID short_topic_id = 0;
	EXTERN_ID extern_message_id = 0;
	FLAG send_messages = 0;

	TRY(db_check_subscribed(device_id, topic_id, &short_topic_id, &qos, &is_subscribed, &send_messages));
	
	if(is_subscribed == 0 || send_messages ==0)
		return DB_DEVICE_NOT_SUBSCRIBED;

	if(qos == 1 || qos == 2)
		TRY(db_get_next_extern_message_id(device_id, &extern_message_id));

	char* sql_send_message_prepare = "INSERT INTO message_device (to_database, device_id, message_id, qos, extern_message_id, state, valid, last_change, added_on) VALUES (0, ?, ?, ?, ?, 0, 1, 0, ?);";
	
	statement_ptr send_message;

	TRY(db_sql_prepare(&send_message, sql_send_message_prepare));
	TRY(db_sql_bind_int(send_message, 1, device_id));
	TRY(db_sql_bind_int(send_message, 2, message_id));
	TRY(db_sql_bind_int(send_message, 3, qos));
	TRY(db_sql_bind_int(send_message, 4, extern_message_id));
	TRY(db_sql_bind_int(send_message, 5, time(NULL)));
	TRY(db_sql_step(send_message, NULL));
	TRY(db_sql_finalize(send_message));

	ADDRESS address = 0;

	FLAG is_reachable = 0;
	TRY(db_check_device_is_reachable(device_id, &is_reachable, &address));

	if(is_reachable){
		blob data;

		TRY(db_get_message_database(message_id, &data));

		TRY(db_send_message_address(address, PUBLISH, message_id, (EXTERN_ID) message_id, short_topic_id, qos, 0, 0, 0, data.size, data.data));
	}

	return DB_OK;
}


/* send the message by allocating memory en sending it to the output buffer */
/*-------------------------------------------------------------------------------*/
static ERROR db_send_message_address(ADDRESS address, MESSAGE_TYPE type, DATABASE_ID message_id, EXTERN_ID extern_message_id, EXTERN_ID short_topic_id, QOS qos, time_t duration, FLAG duplicate, return_code return_value, LENGTH length, const void* data){
/*-------------------------------------------------------------------------------*/
	MESSAGE* m = NULL;
	TRY(db_create_new_message(&m));
	
	m->address = address;
	m->type = type;
	//TODO fill in the duration field for every message that you send
	uint32_t class = 0; //lowest priority
	uint32_t priority = 0; //lowest priority
	uint32_t weight = 5;

	switch(type){
		case CONNACK:
			class = 10;
			priority = 10;
			weight = 10;
			m->return_value = return_value;
			break;
		case WILLTOPICREQ:
			class = 10;
			priority = 11;
			weight = 10;
		case WILLMSGREQ:
			class = 10;
			priority = 12;
			weight = 10;
		case REGACK:
			class = 9;
			priority = 1;
			weight = 3;
			
			m->short_topic_id = short_topic_id;
			m->message_id = message_id;

			break;
		case SUBACK:
			class = 9;
			priority = 2;
			weight = 3;

			m->qos = qos;
			m->short_topic_id = short_topic_id;
			m->extern_message_id = extern_message_id;
			m->return_value = return_value;
			
			break;
		case PUBACK:
			return DB_IMPLEMENTATION_ERROR;
		case PUBLISH:
			class = 5;
			priority = 1;
			weight = 3;
			
			m->message_id = message_id;
			m->short_topic_id = short_topic_id;
			m->extern_message_id = extern_message_id;
			m->qos = qos;
			m->duplicate = duplicate;
			m->length = length;
			m->message = data;

			break;
		case DISCONNECT:
			class = 2;
			priority = 1;
			weight = 3;
		default: return DB_IMPLEMENTATION_ERROR;
	}

	PL_TRY(add_data_blocking(output, (pl_data) m, class, priority, weight));	

	return DB_OK;
}

/* send a message stored in the database to each device subscribed to that topic */
/*-------------------------------------------------------------------------------*/
static ERROR db_send_message_topic(DATABASE_ID topic_id, DATABASE_ID message_id){
/*-------------------------------------------------------------------------------*/
	
	char* sql_get_device_id_prepare = "SELECT device_id, short_topic_id, qos FROM topic_device WHERE topic_id = ? AND valid = 1 AND send_messages = 1;";
	statement_ptr get_device_id;

	TRY(db_sql_prepare(&get_device_id, sql_get_device_id_prepare));
	TRY(db_sql_bind_int(get_device_id, 1, topic_id));

	FLAG result = 0;

	TRY(db_sql_step(get_device_id, &result));
	
	if(result == 0){
		TRY(db_sql_finalize(get_device_id));
		return DB_OK;
	}

	do{
		uint32_t qos = 0;
		uint32_t short_topic_id = 0;
		uint32_t device_id;
		TRY(db_sql_get_int(get_device_id, 0, &device_id));
		TRY(db_sql_get_int(get_device_id, 1, &short_topic_id));
		TRY(db_sql_get_int(get_device_id, 2, &qos));
		TRY(db_send_database_message_device(device_id, topic_id, message_id));
		TRY(db_sql_step(get_device_id, &result));
	}
	while(result);

	TRY(db_sql_finalize(get_device_id));

	return DB_OK;

}

/*********************************************************************************
**		Set functions
**********************************************************************************/

/* set the connect for a device to the new values where the address is as specified */
/*-------------------------------------------------------------------------------*/
static ERROR db_set_connect_address(ADDRESS address, DATABASE_ID device_id, time_t duration, FLAG clean){
/*-------------------------------------------------------------------------------*/
		
	char* sql_update_connect_prepare = "UPDATE connect SET duration = ?, clean = ? WHERE device_id = ? AND valid = 1 AND address = ? ;";
	
	statement_ptr connect_device;

	TRY(db_sql_prepare(&connect_device, sql_update_connect_prepare));
	TRY(db_sql_bind_int(connect_device, 1, duration));
	TRY(db_sql_bind_int(connect_device, 2, clean));
	TRY(db_sql_bind_int(connect_device, 3, device_id));
	TRY(db_sql_bind_int(connect_device, 4, address));
	TRY(db_sql_step(connect_device, NULL));
	TRY(db_sql_finalize(connect_device));

	return DB_OK;
}

/* change the qos for a device to the required qos based on its device id */
/*-------------------------------------------------------------------------------*/
static ERROR db_set_qos_subscribtion(DATABASE_ID device_id, DATABASE_ID topic_id, QOS qos){
/*-------------------------------------------------------------------------------*/
	char* sql_check_subscribed_qos_prepare = "UPDATE topic_device SET qos = ? WHERE device_id = ? AND topic_id = ? AND valid = 1;";
	statement_ptr check_qos_subscribed;

	TRY(db_sql_prepare(&check_qos_subscribed, sql_check_subscribed_qos_prepare));
	TRY(db_sql_bind_int(check_qos_subscribed, 1, qos));
	TRY(db_sql_bind_int(check_qos_subscribed, 2, device_id));
	TRY(db_sql_bind_int(check_qos_subscribed, 3, topic_id));
	TRY(db_sql_step(check_qos_subscribed, NULL));
	TRY(db_sql_finalize(check_qos_subscribed));

	return DB_OK;
}

/* change the send message flag for a device to the required qos based on its device id */
/*-------------------------------------------------------------------------------*/
static ERROR db_set_send_message_subscribtion(DATABASE_ID device_id, DATABASE_ID topic_id, FLAG send_message){
/*-------------------------------------------------------------------------------*/
	char* sql_check_subscribed_qos_prepare = "UPDATE topic_device SET send_messages = ? WHERE device_id = ? AND topic_id = ? AND valid = 1;";
	statement_ptr check_qos_subscribed;

	TRY(db_sql_prepare(&check_qos_subscribed, sql_check_subscribed_qos_prepare));
	TRY(db_sql_bind_int(check_qos_subscribed, 1, send_message));
	TRY(db_sql_bind_int(check_qos_subscribed, 2, device_id));
	TRY(db_sql_bind_int(check_qos_subscribed, 3, topic_id));
	TRY(db_sql_step(check_qos_subscribed, NULL));
	TRY(db_sql_finalize(check_qos_subscribed));

	return DB_OK;
}



/*********************************************************************************
**		Get functions
**********************************************************************************/
		
/* get the next available external message id for a device */		
/*-------------------------------------------------------------------------------*/
static ERROR db_get_next_extern_message_id(DATABASE_ID device_id, EXTERN_ID* extern_message_id){
/*-------------------------------------------------------------------------------*/	
	*extern_message_id = 0;
	char* sql_get_last_extern_message_id_prepare = "SELECT extern_message_id FROM MESSAGE_DEVICE WHERE device_id = ? ORDER BY added_on DESC LIMIT 1;";
	 
	statement_ptr get_last_external_message;

	FLAG result = 0;

	TRY(db_sql_prepare(&get_last_external_message, sql_get_last_extern_message_id_prepare));
	TRY(db_sql_bind_int(get_last_external_message, 1, device_id));
	TRY(db_sql_step(get_last_external_message, &result));

	if(result ==0){
		TRY(db_sql_finalize(get_last_external_message));
		*extern_message_id = 1;
		return DB_OK;
	}

	uint32_t tmp_extern_message_id = 0;
	TRY(db_sql_step(get_last_external_message, &result));
	TRY(db_sql_get_int(get_last_external_message, 0, &tmp_extern_message_id));
	TRY(db_sql_finalize(get_last_external_message));

	*extern_message_id = (EXTERN_ID) tmp_extern_message_id;

	extern_message_id ++;

	if(*extern_message_id == 65534)
		*extern_message_id = 1;

	return DB_OK;
}

/* get the topic id for a topic of a device based on its id */
/*-------------------------------------------------------------------------------*/
static ERROR db_get_topic_id_device(DATABASE_ID device_id, topic t, TOPIC_ID* topic_id){
/*-------------------------------------------------------------------------------*/

	switch(t.topic_type){
		case 0:
		case 1:
		CATCH( t.topic_length != 2 && t.topic_type == 1)
			return DB_INVALID_INPUT;

		*topic_id = 0;
		DATABASE_ID db_topic_id;
		TRY(db_get_topic_id(t, device_id, &db_topic_id));
  	  
		char* sql_get_topic_id_prepare = "SELECT short_topic_id FROM TOPIC_DEVICE WHERE device_id = ? AND topic_id = ? AND valid = 1 AND short_topic_id IS NOT NULL;";
		 
		statement_ptr get_topic;
	
		uint32_t tmp_topic_id = 0;
		FLAG result = 0;
	
		TRY(db_sql_prepare(&get_topic, sql_get_topic_id_prepare));
		TRY(db_sql_bind_int(get_topic, 1, device_id));
		TRY(db_sql_bind_int(get_topic, 2, db_topic_id));
		TRY(db_sql_step(get_topic, &result));
		TRY(db_sql_get_int(get_topic, 0, &tmp_topic_id));
	
		if(result ==0){
			TRY(db_sql_finalize(get_topic));
			return DB_NO_MATCH_FOUND;
		}
		
		TRY(db_sql_step(get_topic, &result));
		TRY(db_sql_finalize(get_topic));

		CATCH(result == 1)
			return DB_DATABASE_CORRUPT;
	
	
		*topic_id = (TOPIC_ID) tmp_topic_id;
	
		return DB_OK;
		case 2: 
		CATCH( t.topic_length != 2)
			return DB_INVALID_INPUT;
		*topic_id = *((uint16_t*)(t.topic_name));
		return DB_OK;
		default: return DB_INVALID_INPUT;
	}
}

/* get the message out of the database based on the message id */
/*-------------------------------------------------------------------------------*/
static ERROR db_get_message_database(DATABASE_ID message_id, blob* blob){
/*-------------------------------------------------------------------------------*/
	char* sql_get_message_prepare = "SELECT data FROM message WHERE message_id = ? AND valid = 1;";
	FLAG result;
	
	statement_ptr get_message;

	TRY(db_sql_prepare(&get_message, sql_get_message_prepare));
	TRY(db_sql_bind_int(get_message, 1, message_id));
	TRY(db_sql_step(get_message, &result));

	if(result == 0){
		TRY(db_sql_finalize(get_message));
		return DB_IMPLEMENTATION_ERROR;
	}

	TRY(db_sql_get_blob(get_message, 0, blob));
	
	TRY(db_sql_step(get_message, &result));
	TRY(db_sql_finalize(get_message));
	
	if(result == 1){
		return DB_IMPLEMENTATION_ERROR;
	}

	return DB_OK;
}

/* get the will message of a device based on its id */
/*-------------------------------------------------------------------------------*/
static ERROR db_get_will_device(DATABASE_ID device_id, DATABASE_ID* topic_id, blob* data, FLAG* retain){
/*-------------------------------------------------------------------------------*/
	FLAG result = 0;
	char* sql_get_compleet_will_prepare = "SELECT topic_id, message_id, retain FROM WILL WHERE device_id = ? AND incompleet = 0 AND VALID = 1;";
	statement_ptr get_will;

	TRY(db_sql_prepare(&get_will, sql_get_compleet_will_prepare));
	TRY(db_sql_bind_int(get_will, 1, device_id));
	TRY(db_sql_step(get_will, &result));

	if(result == 0){
		TRY(db_sql_finalize(get_will));
		return DB_WILL_TOPIC_NOT_SET;
	}

	DATABASE_ID tmp_topic_id = 0;
	uint32_t message_id = 0;
	uint32_t tmp_retain;

	TRY(db_sql_get_int(get_will, 0, &tmp_topic_id));
	TRY(db_sql_get_int(get_will, 1, &message_id));
	TRY(db_sql_get_int(get_will, 2, &tmp_retain));
	TRY(db_sql_step(get_will, &result));
	TRY(db_sql_finalize(get_will));
	
	if(result == 1){
		TRY(db_sql_finalize(get_will));
	}
		
	TRY(db_get_message_database(message_id, data));
	*topic_id = (DATABASE_ID) tmp_topic_id;
	*retain = (FLAG) tmp_retain;

	return DB_OK;
}


/* get a new short topic id that is not in use for a device based on its id*/
/*-------------------------------------------------------------------------------*/
static ERROR db_get_new_short_topic_id(DATABASE_ID device_id, TOPIC_ID* short_topic_id){
/*-------------------------------------------------------------------------------*/
	*short_topic_id = 0;

	/* get the highest short topic id that is in use */
	char* sql_get_topic_id_max_prepare = "SELECT MAX(short_topic_id) FROM topic_device WHERE device_id = ? AND valid = 1;";

	uint32_t max = 0;
	statement_ptr count_topic;

	TRY(db_sql_prepare(&count_topic, sql_get_topic_id_max_prepare));
	TRY(db_sql_bind_int(count_topic, 1, device_id));
	TRY(db_sql_step(count_topic, NULL));
	TRY(db_sql_get_int(count_topic, 0, &max));
	TRY(db_sql_finalize(count_topic));
	
	/* if the highest number is used, search for old ones */
	if(max > 65534){
			if(max > 65535)
				return DB_DATABASE_CORRUPT;
		
		/* get a sorted list of all the short topic id's used*/
		char* sql_get_topic_id_prepare = "SELECT short_topic_id FROM topic_device WHERE device_id = ? AND valid = 1 ORDER BY short_topic_id ASC;";

		uint32_t first_free = 1;
		uint32_t current = 1;
		FLAG result = 0;
		statement_ptr list_topic;

		TRY(db_sql_prepare(&list_topic, sql_get_topic_id_prepare));
		TRY(db_sql_bind_int(list_topic, 1, device_id));
		TRY(db_sql_step(list_topic, &result));

		/* you should have al least 1 match */
		if(result == 0){
			TRY(db_sql_finalize(list_topic));
			return DB_IMPLEMENTATION_ERROR;
		}

		do{
			TRY(db_sql_get_int(list_topic, 0, &current));
			if(current == first_free){
				/* all the short topic id's are used, return an error */
				if(current > 65534){
					TRY(db_sql_finalize(list_topic));
					return DB_NO_MATCH_FOUND;
				}

				first_free++;
				TRY(db_sql_step(list_topic, &result));
			} else {
				*short_topic_id = first_free;
				TRY(db_sql_finalize(list_topic));
				return DB_OK;
			}
		} while(result == 1);

		return DB_IMPLEMENTATION_ERROR; //should have found a solution by now

	} else {
		*short_topic_id = max + 1;	
	}

	return DB_OK;
}

/* get the incompleet will message from a device */
/*-------------------------------------------------------------------------------*/
static ERROR db_get_incompleet_will_device(DATABASE_ID device_id, DATABASE_ID* will_id, DATABASE_ID* will_topic, FLAG* retain, QOS* qos){
/*-------------------------------------------------------------------------------*/
	*will_id = 0;
	*will_topic = 0;
	*retain = 0;
	*qos = 0;
	
	FLAG result = 0;
	char* sql_get_incompleet_will_prepare = "SELECT will_id, topic_id, retain, qos FROM WILL WHERE device_id = ? AND incompleet = 1 AND VALID = 1;";
	statement_ptr get_will;

	TRY(db_sql_prepare(&get_will, sql_get_incompleet_will_prepare));
	TRY(db_sql_bind_int(get_will, 1, device_id));
	TRY(db_sql_step(get_will, &result));

	if(result == 0){
		TRY(db_sql_finalize(get_will));
		return DB_NO_MATCH_FOUND;
	}

	uint32_t tmp_retain = 0;
	uint32_t tmp_qos = 0;

	TRY(db_sql_get_int(get_will, 0, will_id));
	TRY(db_sql_get_int(get_will, 1, will_topic));
	TRY(db_sql_get_int(get_will, 2, &tmp_retain));
	TRY(db_sql_get_int(get_will, 3, &tmp_qos));
	
	TRY(db_sql_step(get_will, &result));
	TRY(db_sql_finalize(get_will));

	if(result != 0)
		return DB_DATABASE_CORRUPT;

	if(tmp_retain != 0 && tmp_retain !=1)
		return DB_DATABASE_CORRUPT;

	if(tmp_qos != 3 && tmp_qos != 0 && tmp_qos != 1 && tmp_qos != 2)
		return DB_DATABASE_CORRUPT;

	*retain = (FLAG) tmp_retain;
	*qos = (FLAG) tmp_qos;

	return DB_OK;
}

/* get the topic id of a topic, you need to give the device_id if you provide the short topic id*/
/*-------------------------------------------------------------------------------*/
static ERROR db_get_topic_id(topic t, DATABASE_ID device_id, DATABASE_ID* topic_id){
/*-------------------------------------------------------------------------------*/
	*topic_id = 0;
	switch(t.topic_type){
		case 0:; //empty statement
			TRY(db_get_topic_id_name(device_id, t, topic_id));
			return DB_OK;
		case 1:; //empty statement
			if(t.topic_length != 2)
				return DB_INVALID_INPUT;
			TRY(db_get_topic_id_predefined_topic_id(*((uint16_t*) (t.topic_name)), topic_id));
			return DB_OK;
		case 2:; //empty statement
			if(t.topic_length != 2)
				return DB_INVALID_INPUT;
			TRY(db_get_topic_id_short_topic_id(*((uint16_t*) (t.topic_name)), device_id, topic_id));
			return DB_OK;
		case 3:
		default : return DB_INVALID_INPUT;
	}
}

/* get the topic id based on the topic id */
/*-------------------------------------------------------------------------------*/
static ERROR db_get_topic_id_name(DATABASE_ID device_id, topic t, DATABASE_ID* topic_id){
/*-------------------------------------------------------------------------------*/
	
	parser pad;
	FLAG result = 0;
	uint32_t number_of_wildcards = 0;
		// TODO check for wildcards

   TRY(db_parse_topic(t, &pad, &number_of_wildcards));
	
	uint32_t pad_size = 0;
	parser_number_of_elements(pad, &pad_size);
   
	if(pad_size == 0)
		return DB_NO_INPUT_DATA;

	/* create the prepared statement */

	char* sql_get_topic_prepare = "SELECT topic_id FROM TOPIC WHERE parent_topic = ? AND name = ?;";
	statement_ptr get_topic_statement;

	char* topic_name = 0;
	DATABASE_ID parent = 0;
	DATABASE_ID current = 0;
   
	/* send the prepared statement */
  	TRY(db_sql_prepare(&get_topic_statement, sql_get_topic_prepare));

	while(pad_size > 0){
		/* get the name of the topic you want to search */
		TRY(parser_get_top(pad, &topic_name));
		pad_size--;
		
		/* bind the values to the prepared statement */
		TRY(db_sql_bind_int(get_topic_statement, 1, parent));
		TRY(db_sql_bind_text(get_topic_statement, 2, topic_name, -1, SQLITE_STATIC));
			
		/* execute the prepared statement */
		TRY(db_sql_step(get_topic_statement, &result));
		
		 
		if( result == 0){
			TRY(db_add_topic(device_id, topic_name, parent));
			TRY(db_sql_step(get_topic_statement, &result));
			TRY(db_sql_get_int(get_topic_statement, 0, &current));
		} else {
			TRY(db_sql_get_int(get_topic_statement, 0, &current));
		}
			
		TRY(db_sql_step(get_topic_statement, &result));
		
		if(result == 1){	
			TRY(db_sql_get_int(get_topic_statement, 0, &current));
			TRY(db_sql_finalize(get_topic_statement));
			return DB_DATABASE_CORRUPT;
		}

		if( pad_size > 0){
			parent = current;
			current = 0;
			TRY(db_sql_reset(get_topic_statement));
		}
	}
	
	free(topic_name);

	TRY(db_sql_finalize(get_topic_statement));

	*topic_id = current;

	TRY(destroy(& pad));

	return DB_OK;
}

/* get the topic id based on the predefined topic id */
/*-------------------------------------------------------------------------------*/
static ERROR db_get_topic_id_predefined_topic_id(EXTERN_ID predefined_topic_id, DATABASE_ID* topic_id){
/*-------------------------------------------------------------------------------*/
	FLAG result = 0;
	char* sql_count_predefined_topic_id_prepare = "SELECT topic_id FROM topic WHERE predefined = 1 AND predefince_topic_id = ? AND valid = 1;";
	
	statement_ptr count_topic;

	TRY(db_sql_prepare(&count_topic, sql_count_predefined_topic_id_prepare));
	TRY(db_sql_bind_int(count_topic, 1, predefined_topic_id));
	TRY(db_sql_step(count_topic, &result));

	if(result == 0){
		TRY(db_sql_finalize(count_topic));
		return DB_NO_MATCH_FOUND;
	}

	TRY(db_sql_get_int(count_topic, 0, topic_id));

	TRY(db_sql_step(count_topic, &result));
	TRY(db_sql_finalize(count_topic));

	if(result == 1)
		return DB_DATABASE_CORRUPT;

	return DB_OK;
}

/* get the topic id based on the short topic id */
/*-------------------------------------------------------------------------------*/
static ERROR db_get_topic_id_short_topic_id(EXTERN_ID short_topic_id, DATABASE_ID device_id, DATABASE_ID* topic_id){
/*-------------------------------------------------------------------------------*/
	FLAG result =0;
	char* sql_count_short_topic_id_prepare = "SELECT topic_id FROM topic_device WHERE valid = 1 AND short_topic_id = ? AND device_id = ?;";
	
	statement_ptr count_short_topic;

	TRY(db_sql_prepare(&count_short_topic, sql_count_short_topic_id_prepare));
	TRY(db_sql_bind_int(count_short_topic, 1, short_topic_id));
	TRY(db_sql_bind_int(count_short_topic, 2, device_id));
	TRY(db_sql_step(count_short_topic, &result));

	if(result == 0){
		TRY(db_sql_finalize(count_short_topic));
		return DB_NO_MATCH_FOUND;
	}
	
	TRY(db_sql_get_int(count_short_topic, 0, topic_id));

	TRY(db_sql_step(count_short_topic, &result));
	TRY(db_sql_finalize(count_short_topic));
	if(result == 1){
		return DB_DATABASE_CORRUPT;
	}

	return DB_OK;
}

/* get the device id based on the address, check first it is connected */
/*-------------------------------------------------------------------------------*/
static ERROR db_get_device_id_address(ADDRESS address, DATABASE_ID* device_id){
/*-------------------------------------------------------------------------------*/
	FLAG result = 0;
	*device_id = 0;
	statement_ptr get_device_id;

	char* sql_get_device_id_prepare = "SELECT device_id FROM CONNECT WHERE address = ? AND valid = 1";
    
	TRY(db_sql_prepare(&get_device_id, sql_get_device_id_prepare));
	TRY(db_sql_bind_int64(get_device_id, 1, address));
 	TRY(db_sql_step(get_device_id, &result));
		
	if( result == 0){
		TRY(db_sql_finalize(get_device_id));
		return DB_DEVICE_NOT_FOUND;
	}

	TRY(db_sql_get_int(get_device_id, 0, device_id));
 	TRY(db_sql_step(get_device_id, &result));
	TRY(db_sql_finalize(get_device_id));

	
	if( result == 1)
		return DB_DATABASE_CORRUPT;

	return DB_OK;
}

/* get the device based on the client id, check first it exists */
/*-------------------------------------------------------------------------------*/
static ERROR db_get_device_client_id( char* client_id, DATABASE_ID* device_id ){
/*-------------------------------------------------------------------------------*/
	FLAG result = 0;
	*device_id = 0;
	char* sql_get_device_id_prepare = "SELECT device_id FROM device WHERE client_id = ? AND valid = 1;";
	statement_ptr get_device;

	TRY(db_sql_prepare(&get_device, sql_get_device_id_prepare));
	TRY(db_sql_bind_text(get_device, 1, client_id, -1, NULL));
	TRY(db_sql_step(get_device, &result));
 
	if(result == 0){
		TRY(db_sql_finalize(get_device));
		return DB_DEVICE_NOT_FOUND;
	}

	TRY(db_sql_get_int(get_device, 0, device_id));
	TRY(db_sql_step(get_device, &result));	
	TRY(db_sql_finalize(get_device));

	if( result == 1)
		return DB_DATABASE_CORRUPT;

	return DB_OK;
}

/* this functions gets the history for a certain toppic based on the message */
/*-------------------------------------------------------------------------------*/
static ERROR db_get_history(MESSAGE* m){
/*-------------------------------------------------------------------------------*/	
	char* start_message = (void*) m->message;
	topic* t = (topic*) malloc(sizeof(topic));
	uint32_t i = 0;

	time_t start = *((time_t*) start_message);
	start_message += sizeof(time_t);

	time_t stop = *((time_t*) start_message);
	start_message += sizeof(time_t);

	t->topic_type = 0;
	t->topic_length = (m->length) - 2*sizeof(time_t);
	t->topic_name = malloc(t->topic_length);

	for(i = 0; i<t->topic_length; i++){
		*(((char*) (t->topic_name)) + i) = *(start_message + i);
	}

	FLAG is_free = 0;
	TRY(db_check_topic_exists(*t, &is_free));
	
	if( is_free == 1 ){
		//TODO remove old message
		db_add_message_database_topic(response_topic_id, 1, m);
		return DB_OK;
	}

	DATABASE_ID topic_id;
	TRY(db_get_topic_id_name(0, *t, &topic_id));

	uint64_t size_client_name = 0;
	uint64_t size_message = 0;
	uint32_t count = 0;
	
	FLAG result = 0;
	char* sql_get_history_size_prepare = "SELECT sum(device.size), sum(message.size), count(message.message_id) FROM message JOIN device ON message.added_by = device.device_id where message.valid = 1 AND message.added_on > ? AND message.added_on < ? AND message.topic_id = ? ORDER BY message.added_on ASC;";

	statement_ptr get_size_history;

	TRY(db_sql_prepare(&get_size_history, sql_get_history_size_prepare));
	TRY(db_sql_bind_int(get_size_history, 1, start));
	TRY(db_sql_bind_int(get_size_history, 2, stop));
	TRY(db_sql_bind_int(get_size_history, 3, topic_id));
	TRY(db_sql_step(get_size_history, &result));

	if(result == 0){
		TRY(db_sql_finalize(get_size_history));
		return DB_DATABASE_ERROR;	
	}

	TRY(db_sql_get_int64(get_size_history, 0, &size_client_name));
	TRY(db_sql_get_int64(get_size_history, 1, &size_message));
	TRY(db_sql_get_int(get_size_history, 2, &count));
	TRY(db_sql_finalize(get_size_history));

	if( (count*12 + size_message + size_client_name) > 2000000000){
		char* response_message = "result too big";
		MESSAGE* response = (MESSAGE*) malloc(sizeof(MESSAGE));
		response->length = m->length + strlen(response_message) + 1;
		response->message = malloc(response->length);

		uint32_t j = 0;
		for(j = 0; j<m->length; j++){
			*((uint8_t*) (response->message) + j) = *((uint8_t*) (m->message) + j);
		}

		for(j = m->length; j < (m->length + strlen(response_message) + 1) ; j++){
			*((uint8_t*) (response->message) + j) = *((uint8_t*) (response_message) + j);
		}

		//TODO remove old message
		db_add_message_database_topic(response_topic_id, 1, response);
	}
	
	if( count == 0 ){
		//TODO remove old message
		db_add_message_database_topic(response_topic_id, 1, m);
		return DB_OK;
	}

	char* sql_get_history_prepare = "SELECT message.added_on, device.size, device.client_id, message.size, message.data FROM message JOIN device ON message.added_by = device.device_id where message.valid = 1 AND message.added_on > ? AND message.added_on < ? AND message.topic_id = ? ORDER BY message.added_on ASC;";

	statement_ptr get_history;

	TRY(db_sql_prepare(&get_history, sql_get_history_prepare));
	TRY(db_sql_bind_int(get_history, 1, start));
	TRY(db_sql_bind_int(get_history, 2, stop));
	TRY(db_sql_bind_int(get_history, 3, topic_id));
	
	
	MESSAGE* response = (MESSAGE*) malloc(sizeof(MESSAGE));
	response->length = m->length + (count * 3) + size_client_name + size_message;
	response->message = malloc(response->length);

	uint32_t j = 0;
	for(j = 0; j<m->length; j++){
		*((uint8_t*) (response->message) + j) = *((uint8_t*) (m->message) + j);
	}

	void* first_byte = (void*) response->message + m->length;

	uint32_t time = 0;
	uint32_t length_client_id = 0;
	blob client_blob;
	uint32_t length_message = 0;
	blob message_blob;

	do{
		TRY(db_sql_step(get_history, &result));
		TRY(db_sql_get_int(get_history, 0, &time));
		TRY(db_sql_get_int(get_history, 1, &length_client_id));
		TRY(db_sql_get_text(get_history, 2, &client_blob));
		TRY(db_sql_get_int(get_history, 3, &length_message));
		TRY(db_sql_get_blob(get_history, 4, &message_blob));
		TRY(db_get_line_request(&first_byte, time, length_client_id, (char*)(client_blob.data), length_message, (void*) message_blob.data));
	} while (result == 1);


	TRY(db_sql_finalize(get_history));
	db_add_message_database_topic(response_topic_id, 1, m);
	return DB_OK;
}

/* writes the parameters to memory and sets the first_byte to the first empty byte */
static ERROR db_get_line_request(void** first_byte, uint32_t time, uint32_t length_client_id, void* client_id, uint32_t length_message, void* message){
	
	void* cursor = *first_byte;
	uint32_t i = 0;

	*(uint32_t*) cursor = time;
	cursor += sizeof(uint32_t);

	*(uint32_t*) cursor = length_client_id;
	cursor += sizeof(uint32_t);

	for(i = 0; i<length_client_id; i++){
		*((uint8_t*) cursor + i) = *((uint8_t*) client_id +i);
	}
	cursor += length_client_id; 

	*(uint32_t*) cursor = length_message;
	cursor += sizeof(uint32_t);

	for(i = 0; i<length_message; i++){
		*((uint8_t*) cursor + i) = *((uint8_t*) message +i);
	}
	cursor += length_message;
	return DB_OK;
}

/*********************************************************************************
**		Add functions
**********************************************************************************/

/* add a message to a topic based on a real message that came in */
/*-------------------------------------------------------------------------------*/
static ERROR db_add_message_database_topic(DATABASE_ID topic_id, DATABASE_ID device_id, MESSAGE* m){
/*-------------------------------------------------------------------------------*/
	//TODO make sure that the retain function takes the newest retain message only
	FLAG result = 0;
	uint32_t timeout = time(NULL) - TIMEOUT;

	if(m->qos == 1 || m->qos ==2){
		char* sql_get_message_prepare = "SELECT message_id FROM message_device WHERE extern_message_id = ? AND device_id = ? AND added_on > ? AND valid = 1;";
		statement_ptr get_message;
		
		TRY(db_sql_prepare(&get_message, sql_get_message_prepare));
		TRY(db_sql_bind_int(get_message, 1, (uint32_t) m->extern_message_id));
		TRY(db_sql_bind_int(get_message, 2, device_id));
		TRY(db_sql_bind_int(get_message, 3, timeout));
		TRY(db_sql_step(get_message, &result));

		FLAG is_reachable = 0;
		ADDRESS address = 0;

		if(result == 0){
			
			TRY(db_add_message_database(topic_id, device_id, m));
			char* sql_add_message_prepare = "INSERT INTO message_device (to_database, device_id, message_ID, extern_message_id, qos, state, valid, last_change, added_on) VALUES (1, ?, ?, ?, ?, ?, 1, ?, ?);";
			statement_ptr add_message;

			TRY(db_sql_prepare(&add_message, sql_add_message_prepare));
			TRY(db_sql_bind_int(add_message, 1, device_id));
			TRY(db_sql_bind_int(add_message, 2, m->message_id));
			TRY(db_sql_bind_int(add_message, 3, m->extern_message_id));
			TRY(db_sql_bind_int(add_message, 4, m->qos));
			if(m->qos == 1){
				TRY(db_sql_bind_int(add_message, 5, ACK));
			} else {
				TRY(db_sql_bind_int(add_message, 5, REC));
			}
			TRY(db_sql_bind_int(add_message, 6, time(NULL)));
			TRY(db_sql_bind_int(add_message, 7, time(NULL)));
			TRY(db_sql_step(add_message, NULL));
			TRY(db_sql_finalize(add_message));


			TRY(db_check_device_is_reachable(device_id, &is_reachable, &address));
				
			if(is_reachable){
				if(m->qos == 1){
					TRY(db_send_message_address(address, PUBACK, m->message_id, m->extern_message_id, m->short_topic_id, m->qos, 0, 0, ACCEPTED, 0, NULL));
				}
				if(m->qos == 2){
					TRY(db_send_message_address(address, PUBREC, m->message_id, m->extern_message_id, m->short_topic_id, m->qos, 0, 0, ACCEPTED, 0, NULL));
				}
			}

			TRY(db_sql_finalize(get_message));
			if(topic_id == request_topic_id)
				TRY(db_get_history(m));
			return DB_OK;
		}

		TRY(db_sql_get_int(get_message, 0, &(m->message_id)));
		TRY(db_sql_step(get_message, &result));

		TRY(db_check_device_is_reachable(device_id, &is_reachable, &address));
				
		if(is_reachable){
			if(m->qos == 1){
				TRY(db_send_message_address(address, PUBACK, m->message_id, m->extern_message_id, m->short_topic_id, m->qos, 0, 1, ACCEPTED, 0, NULL));
			}
			if(m->qos == 2){
				TRY(db_send_message_address(address, PUBREC, m->message_id, m->extern_message_id, m->short_topic_id, m->qos, 0, 1, ACCEPTED, 0, NULL));
			}
		}
		
		TRY(db_sql_step(get_message, &result));
		TRY(db_sql_finalize(get_message));

		if(result == 1){
			TRY(db_sql_finalize(get_message));
			return DB_DATABASE_CORRUPT;
		};
		if(topic_id == request_topic_id)
			TRY(db_get_history(m));
		return DB_OK;
	}
	
	TRY(db_add_message_database(topic_id, device_id, m));
	char* sql_add_message_prepare = "INSERT INTO message_device (to_database, device_id, message_ID, extern_message_id, qos, state, valid, last_change, added_on) VALUES (1, ?, ?, ?, ?, 0, 1, ?, ?);";
	statement_ptr add_message;

	TRY(db_sql_prepare(&add_message, sql_add_message_prepare));
	TRY(db_sql_bind_int(add_message, 1, device_id));
	TRY(db_sql_bind_int(add_message, 2, m->message_id));
	TRY(db_sql_bind_int(add_message, 3, m->extern_message_id));
	TRY(db_sql_bind_int(add_message, 4, m->qos));
	TRY(db_sql_bind_int(add_message, 5, time(NULL)));
	TRY(db_sql_bind_int(add_message, 6, time(NULL)));
	TRY(db_sql_step(add_message, NULL));
	TRY(db_sql_finalize(add_message));
	
	TRY(db_send_message_topic(topic_id, m->message_id));
	
	if(topic_id == request_topic_id)
		TRY(db_get_history(m));

	return DB_OK;
}

/* add a new message in the database, the message id will be set in the original message */
/*-------------------------------------------------------------------------------*/
static ERROR db_add_message_database(DATABASE_ID topic_id, DATABASE_ID device_id, MESSAGE* m){
/*-------------------------------------------------------------------------------*/

	uint32_t now = time(NULL);
	FLAG result = 0;
	
	char* sql_add_message_prepare = "INSERT INTO message (topic_id, added_by, data, added_on, retain, size, valid) VALUES (?, ?, ?, ?, ?, ?, 1);";
	statement_ptr add_message;

	TRY(db_sql_prepare(&add_message, sql_add_message_prepare));
	TRY(db_sql_bind_int(add_message, 1, topic_id));
	TRY(db_sql_bind_int(add_message, 2, device_id));
	TRY(db_sql_bind_blob(add_message, 3, m->message, m->length, SQLITE_STATIC));
	TRY(db_sql_bind_int(add_message, 4, now));
	TRY(db_sql_bind_int(add_message, 5, m->retain));
	TRY(db_sql_bind_int(add_message, 6, m->length));
	TRY(db_sql_step(add_message, NULL));
	TRY(db_sql_finalize(add_message));
	
	char* sql_get_message_id_prepare = "SELECT message_id FROM message WHERE topic_id = ? AND added_by = ? AND data = ? AND added_on = ? AND retain = ? AND size = ? AND valid = 1;";
	statement_ptr get_message;

	TRY(db_sql_prepare(&get_message, sql_get_message_id_prepare));
	TRY(db_sql_bind_int(get_message, 1, topic_id));
	TRY(db_sql_bind_int(get_message, 2, device_id));
	TRY(db_sql_bind_blob(get_message, 3, m->message, m->length,  free));
	TRY(db_sql_bind_int(get_message, 4, now));
	TRY(db_sql_bind_int(get_message, 5, m->retain));
	TRY(db_sql_bind_int(get_message, 6, m->length));
	TRY(db_sql_step(get_message, &result));
	if(result == 0){
		TRY(db_sql_finalize(get_message));
		return DB_IMPLEMENTATION_ERROR;
	}
	
	TRY(db_sql_get_int(get_message, 0, &(m->message_id)));
	TRY(db_sql_step(get_message, &result));
	TRY(db_sql_finalize(get_message));
	
	if(result == 1){
		return DB_DATABASE_CORRUPT;
	}

	return DB_OK;
}

/* create a connection for the specified device based on its id */
/*-------------------------------------------------------------------------------*/
static ERROR db_add_connect_device(DATABASE_ID device_id, time_t duration, ADDRESS address, FLAG clean){
/*-------------------------------------------------------------------------------*/
	char* sql_connect_device_prepare = "INSERT INTO connect (device_id, duration, valid, added_on, address, clean, awake) VALUES (?,?,1,?,?,?, 1);";
	
	statement_ptr connect_device;

	TRY(db_sql_prepare(&connect_device, sql_connect_device_prepare));
	TRY(db_sql_bind_int(connect_device, 1, device_id));
	TRY(db_sql_bind_int(connect_device, 2, duration));
	TRY(db_sql_bind_int(connect_device, 3, time(NULL)));
	TRY(db_sql_bind_int64(connect_device, 4, address));
	TRY(db_sql_bind_int(connect_device, 5, clean));
	TRY(db_sql_step(connect_device, NULL));
	TRY(db_sql_finalize(connect_device));
	
	return DB_OK;
}

/* add a topic to the database based on the topic name. Give it the id of the parent topic */
/*-------------------------------------------------------------------------------*/
static ERROR db_add_topic(DATABASE_ID device_id, char* topic_name, DATABASE_ID parent){
/*-------------------------------------------------------------------------------*/
	time_t now = time(NULL);
	 
	char* sql_new_topic_prepared = "INSERT INTO topic (name, added_on, parent_topic, created_by, predefined, valid) VALUES (?,?,?,?,0,1);";
	
	statement_ptr new_topic;

	TRY(db_sql_prepare(&new_topic, sql_new_topic_prepared));
	TRY(db_sql_bind_text(new_topic, 1, topic_name, -1, SQLITE_STATIC));
	TRY(db_sql_bind_int(new_topic, 2, now));
	TRY(db_sql_bind_int(new_topic, 3, parent));
	TRY(db_sql_bind_int(new_topic, 4, device_id));
	TRY(db_sql_step(new_topic, NULL));
	TRY(db_sql_finalize(new_topic));

	return DB_OK;
}

/* add a device based on the client_id and return the device id*/
/*-------------------------------------------------------------------------------*/
static ERROR db_add_device_client_id( char* client_id, DATABASE_ID* device_id){
/*-------------------------------------------------------------------------------*/
	FLAG is_free = 0;

	TRY(db_check_client_free(client_id, &is_free));

	if(!is_free)
		return DB_DANGEROUS_FUNCTION_CALL;

	*device_id = 0;
	char* sql_add_device_prepare = "INSERT INTO device (client_id, added_on, size, valid) VALUES (?, ?, ?, 1);";
	statement_ptr add_device;
	TRY(db_sql_prepare(&add_device, sql_add_device_prepare));
	TRY(db_sql_bind_text(add_device, 1, client_id, -1, NULL));
	TRY(db_sql_bind_int(add_device, 2, time(NULL)));
	TRY(db_sql_bind_int(add_device, 3, strlen(client_id) + 1));
	TRY(db_sql_step(add_device, NULL));
	TRY(db_sql_finalize(add_device));

	TRY(db_get_device_client_id(client_id, device_id));

	return DB_OK;
}

/*********************************************************************************
**		Check functions to check for conditions
**********************************************************************************/

/* check if the will message is already completed for a device based on its id */
/*-------------------------------------------------------------------------------*/
static ERROR db_check_will_message_completed(DATABASE_ID device_id, MESSAGE* message, FLAG* already_completed){
/*-------------------------------------------------------------------------------*/
	*already_completed = 0;

	blob input_data;
	input_data.size = message->length;
	input_data.data = message->message;
	
	FLAG exists = 0;
	TRY(db_check_will_exists(device_id, &exists));

	if(!exists){
		*already_completed = 0;
		return DB_OK;
	}

	FLAG retain = 0;
	DATABASE_ID topic_id = 0;
	blob data;
	TRY(db_get_will_device(device_id, &topic_id, &data, &retain));

	if(data.size != input_data.size){
		*already_completed = 0;
		return DB_OK;
	}

	*already_completed = 1;
	uint32_t i = 0;
	for(i = 0; i<data.size; i++){
		if(*(((char*) data.data)+i) != *(((char*) input_data.data)+i))
			*already_completed = 0;
	}

	return DB_OK;
}

/* check if the will exists based on de device id */
/*-------------------------------------------------------------------------------*/
static ERROR db_check_will_exists(DATABASE_ID device_id, FLAG* exists){
/*-------------------------------------------------------------------------------*/
	*exists = 0;
	FLAG result = 0;

	char* sql_get_compleet_will_prepare = "SELECT topic_id, message_id FROM WILL WHERE device_id = ? AND incompleet = 0 AND VALID = 1;";
	statement_ptr get_will;

	TRY(db_sql_prepare(&get_will, sql_get_compleet_will_prepare));
	TRY(db_sql_bind_int(get_will, 1, device_id));
	TRY(db_sql_step(get_will, &result));

	if(result == 0)
		*exists = 0;
	if(result == 1)
		*exists = 1;

	TRY(db_sql_step(get_will, &result));
	if(result ==1)
		return DB_DATABASE_CORRUPT;

	TRY(db_sql_finalize(get_will));

	return DB_OK;
}

/* check if an incompleet will message exists for a device based on its id */
/*-------------------------------------------------------------------------------*/
static ERROR db_check_incompleet_will_message(DATABASE_ID device_id, FLAG* incompleet_exists){
/*-------------------------------------------------------------------------------*/
	*incompleet_exists = 0;
	FLAG result = 0;

	char* sql_get_incompleet_will_prepare = "SELECT will_id FROM WILL WHERE device_id = ? AND incompleet = 1 AND VALID = 1;";
	statement_ptr get_will;

	TRY(db_sql_prepare(&get_will, sql_get_incompleet_will_prepare));
	TRY(db_sql_bind_int(get_will, 1, device_id));
	TRY(db_sql_step(get_will, &result));

	if(result == 0)
		*incompleet_exists = 0;
	if(result == 1){
		*incompleet_exists = 1;
	}
	
	TRY(db_sql_step(get_will, &result));
	TRY(db_sql_finalize(get_will));
	if(result == 1){
		return DB_DATABASE_CORRUPT;
	}

	return DB_OK;
}

/* check if the incompleet will message is set for a device based on its id*/
/*-------------------------------------------------------------------------------*/
static ERROR db_check_incomplete_will_is_set(DATABASE_ID device_id, DATABASE_ID topic_id, QOS qos, FLAG retain, FLAG* is_set){
/*-------------------------------------------------------------------------------*/
	char* sql_get_will_topic_prepare = "SELECT device_id FROM will WHERE device_id = ? AND topic_id = ? AND message_id = 0 AND valid = 1 AND qos = ? AND incompleet = 1 AND retain = ?;";
	statement_ptr get_will;
	FLAG result = 0;

	TRY(db_sql_prepare(&get_will, sql_get_will_topic_prepare));
	TRY(db_sql_bind_int(get_will, 1, device_id));
	TRY(db_sql_bind_int(get_will, 2, topic_id));
	TRY(db_sql_bind_int(get_will, 3, qos));
	TRY(db_sql_bind_int(get_will, 4, retain));
	TRY(db_sql_step(get_will, &result));
	
	if(result == 0){
		*is_set = 0;
		TRY(db_sql_finalize(get_will));
		return DB_OK;
	} else {
		TRY(db_sql_step(get_will, &result));
		TRY(db_sql_finalize(get_will));
		if(result == 0){
			*is_set = 1;			
			return DB_OK;
		} else {
			//TODO look if it is not possible to fix this error by removing al the incompleet will messages
			return DB_IMPLEMENTATION_ERROR;
		}
	}
}

/* check if the device is reachable based on its id */
/*-------------------------------------------------------------------------------*/
static ERROR db_check_device_is_reachable(DATABASE_ID device_id, FLAG* is_reachable, ADDRESS* address){
/*-------------------------------------------------------------------------------*/
	FLAG result = 0;
	*is_reachable = 0;
	*address = 0;
	
	char* sql_check_reachable_prepare = "SELECT awake, address FROM connect WHERE device_id = ? AND valid = 1;";
	statement_ptr check_reachable;

	TRY(db_sql_prepare(&check_reachable, sql_check_reachable_prepare));
	TRY(db_sql_bind_int(check_reachable, 1, device_id));
	TRY(db_sql_step(check_reachable, &result));

	if(result == 0){
		TRY(db_sql_finalize(check_reachable));
		*is_reachable = 0;
	} else {
		uint32_t tmp_is_reachable = 0;
		TRY(db_sql_get_int(check_reachable, 0, &tmp_is_reachable));
		TRY(db_sql_get_int64(check_reachable, 1, address));
		TRY(db_sql_step(check_reachable, &result));
		TRY(db_sql_finalize(check_reachable));
		*is_reachable = (FLAG) tmp_is_reachable;
		if(result == 1)
			return DB_IMPLEMENTATION_ERROR;
	}

	return DB_OK;
}

/* get the information about a subscribtion if the device is subscribed. Based on its device id and the topic id */
/*-------------------------------------------------------------------------------*/
static ERROR db_check_subscribed(DATABASE_ID device_id, DATABASE_ID topic_id, EXTERN_ID* short_topic_id, QOS* qos, FLAG* subscribed, FLAG* send_messages){
/*-------------------------------------------------------------------------------*/
	*qos = 0;
	*subscribed = 0;
	*send_messages = 0;
	
	FLAG result = 0;
	char* sql_check_subscribed_prepare = "SELECT qos, short_topic_id, send_messages FROM topic_device WHERE device_id = ? AND topic_id = ? AND valid = 1;";
	statement_ptr check_subscribed;

	TRY(db_sql_prepare(&check_subscribed, sql_check_subscribed_prepare));
	TRY(db_sql_bind_int(check_subscribed, 1, device_id));
	TRY(db_sql_bind_int(check_subscribed, 2, topic_id));
	TRY(db_sql_step(check_subscribed, &result));

	if(result == 0){
	 	*subscribed = 0;
	} else {
		uint32_t tmp_qos = 0;
		uint32_t tmp_short_topic_id = 0;
		uint32_t tmp_send_messages = 0;

		TRY(db_sql_get_int(check_subscribed, 0, &tmp_qos));
		TRY(db_sql_get_int(check_subscribed, 1, &tmp_short_topic_id));
		TRY(db_sql_get_int(check_subscribed, 2, &tmp_send_messages));

		*qos = (QOS) tmp_qos;
		*short_topic_id = (EXTERN_ID) tmp_short_topic_id;
		*send_messages = (FLAG) tmp_send_messages;
	
		*subscribed = 1;

		TRY(db_sql_step(check_subscribed, &result));
		
		if(result == 1){
			TRY(db_sql_finalize(check_subscribed));
			return DB_DATABASE_CORRUPT;
		}
	}
	
	TRY(db_sql_finalize(check_subscribed));
	return DB_OK;
}

/* check if the topic exists, can handle every type */
/*-------------------------------------------------------------------------------*/
static ERROR db_check_topic_exists(topic t, FLAG* is_free){
/*-------------------------------------------------------------------------------*/
	*is_free = 0;
	switch(t.topic_type){
		case 0:; // empty statement
			TRY(db_check_client_free_topic_name(t, is_free));
			return DB_OK;
		case 1:; //empty statement
			if(t.topic_length != 2)
				return DB_INVALID_INPUT;
			TRY(db_check_client_free_predefined_topic(*((uint16_t*)(t.topic_name)), is_free));
			return DB_OK;
		case 2:; //empty statement
			if(t.topic_length != 2)
				return DB_INVALID_INPUT;
			TRY(db_check_client_free_short_topic_id(*((uint16_t*) (t.topic_name)), is_free));
			return DB_OK;
		case 3:
		default : return DB_INVALID_INPUT;
	}
}

/* check if the client is  Free based on topic id */
/*-------------------------------------------------------------------------------*/
static ERROR db_check_client_free_topic_name(topic t, FLAG* is_free){	
/*-------------------------------------------------------------------------------*/
	parser pad;
	uint32_t number_of_wildcards = 0;
	// TODO check for wildcards

	TRY(db_parse_topic(t, &pad, &number_of_wildcards));

	uint32_t pad_size = 0;
	parser_number_of_elements(pad, &pad_size);

	if(pad_size == 0){
		return DB_NO_INPUT_DATA;
	}

	FLAG result = 0;

	/* create the prepared statement */

   char* sql_get_topic_prepare = "SELECT topic_id FROM TOPIC WHERE parent_id = ? AND name = ?;";
	statement_ptr get_topic_statement;

	char* topic_name = 0;
	DATABASE_ID parent = 0;
	DATABASE_ID current = 0;

	/* send the prepared statement */
	TRY(db_sql_prepare(&get_topic_statement, sql_get_topic_prepare));

	for(;pad_size>0; pad_size--){
		
		/* get the name of the topic you want to search */
		TRY(parser_get_top(pad, (char**)&(t.topic_name) ));

		/* bind the values to the prepared statement */
		TRY(db_sql_bind_int(get_topic_statement, 1, parent));
		TRY(db_sql_bind_text(get_topic_statement, 2, topic_name, -1, &free));
		
		/* execute the prepared statement */
		TRY(db_sql_step(get_topic_statement, &result));
		
	 
		if( result == 0){
			TRY(db_sql_finalize(get_topic_statement));
			*is_free = 1;
			return DB_OK;
		}

		if( result==1){
			TRY(db_sql_get_int(get_topic_statement, 0, &current));
			TRY(db_sql_step(get_topic_statement, &result));
			if( result == 1){
				TRY(db_sql_finalize(get_topic_statement));
				return DB_DATABASE_CORRUPT;
			}
		}

		if( pad_size > 0){
			parent = current;
			current = 0;
			TRY(db_sql_reset(get_topic_statement));
		}
	}

	TRY(db_sql_finalize(get_topic_statement));

	free( topic_name );

	TRY(destroy(& pad));

	*is_free = 0;

	return DB_OK;
}

/* check if the client is  Free based on the predefined topic id */
/*-------------------------------------------------------------------------------*/
static ERROR db_check_client_free_predefined_topic(EXTERN_ID predefince_topic_id, FLAG* is_free){
/*-------------------------------------------------------------------------------*/
	char* sql_count_predefined_topic_id_prepare = "SELECT count(topic_id) FROM topic WHERE predefined = 1 AND predefince_topic_id = ? AND valid = 1;";
	
	uint32_t count = 0;
	statement_ptr count_topic;

	TRY(db_sql_prepare(&count_topic, sql_count_predefined_topic_id_prepare));
	TRY(db_sql_bind_int(count_topic, 1, predefince_topic_id));
	TRY(db_sql_step(count_topic, NULL));
	TRY(db_sql_get_int(count_topic, 0, &count));
	TRY(db_sql_finalize(count_topic));

	CATCH(count > 1){
		return DB_DATABASE_CORRUPT;
	}
    
	if(count == 0){
		*is_free = 1;
	} else {
		*is_free = 0;
	}
	return DB_OK;
}

/* check if the client is  Free based on the short topic id */
/*-------------------------------------------------------------------------------*/
static ERROR db_check_client_free_short_topic_id(EXTERN_ID short_topic_id, FLAG* is_free){
/*-------------------------------------------------------------------------------*/
	char* sql_count_short_topic_id_prepare = "SELECT count(topic_device_id) FROM topic_device WHERE valid = 1 AND short_topic_id = ?;";
	
	uint32_t count = 0;
	statement_ptr count_short_topic;

	TRY(db_sql_prepare(&count_short_topic, sql_count_short_topic_id_prepare));
	TRY(db_sql_bind_int(count_short_topic, 1, short_topic_id));
	TRY(db_sql_step(count_short_topic, NULL));
	TRY(db_sql_get_int(count_short_topic, 0, &count));
	TRY(db_sql_finalize(count_short_topic));

	CATCH(count > 1){
		return DB_DATABASE_CORRUPT;
	}
    
	if(count == 0){
		*is_free = 1;
	} else {
		*is_free = 0;
	}
	return DB_OK;
}

/* check that the short topic id is set for the device and is valid */
/*-------------------------------------------------------------------------------*/
static ERROR db_check_short_topic_id_valid(EXTERN_ID short_topic_id, DATABASE_ID device_id, FLAG* valid){
/*-------------------------------------------------------------------------------*/
	if(valid == NULL)
		return DB_IMPLEMENTATION_ERROR;
	
	*valid = 0;
	
	FLAG result =0;
	char* sql_count_short_topic_id_prepare = "SELECT topic_id FROM topic_device WHERE valid = 1 AND short_topic_id = ? AND device_id = ?;";
	
	statement_ptr count_short_topic;

	TRY(db_sql_prepare(&count_short_topic, sql_count_short_topic_id_prepare));
	TRY(db_sql_bind_int(count_short_topic, 1, short_topic_id));
	TRY(db_sql_bind_int(count_short_topic, 2, device_id));
	TRY(db_sql_step(count_short_topic, &result));

	if(result == 0){
		TRY(db_sql_finalize(count_short_topic));
		*valid = 0;
		return DB_NO_MATCH_FOUND;
	}
	
	TRY(db_sql_step(count_short_topic, &result));
	TRY(db_sql_finalize(count_short_topic));
	if(result == 1){
		*valid = 0;
		return DB_DATABASE_CORRUPT;
	}
	
	*valid = 1;

	return DB_OK;
}

/* check if the addres is still  Free */
/*-------------------------------------------------------------------------------*/
static ERROR db_check_address_free(ADDRESS address, FLAG* is_free){
/*-------------------------------------------------------------------------------*/
	*is_free = 0;	
	uint32_t count  = 0;
	char* sql_check_address_free_prepare = "SELECT count(device_id) as amount FROM CONNECT WHERE address = ? AND valid = 1";
   
	statement_ptr check_address;

	TRY(db_sql_prepare(&check_address, sql_check_address_free_prepare));
	TRY(db_sql_bind_int64(check_address, 1, address));
	TRY(db_sql_step(check_address, NULL));
	TRY(db_sql_get_int(check_address, 0, &count));
	TRY(db_sql_finalize(check_address));

    CATCH(count > 1){
		return DB_DATABASE_CORRUPT;
    }
    
    if(count == 0){
        *is_free = 1;
    } else {
        *is_free = 0;
    }
    
    return DB_OK;
}

/* check if the client name is still  Free */
/*-------------------------------------------------------------------------------*/
static ERROR db_check_client_free(char* client_id, FLAG* is_free){
/*-------------------------------------------------------------------------------*/
	*is_free = 0;	
	uint32_t count  = 0;
	char * sql_get_client_count_prepare = "SELECT count(device_id) as amount FROM DEVICE WHERE client_id = ? AND valid = 1;";
   
	statement_ptr count_device;

	TRY(db_sql_prepare(&count_device, sql_get_client_count_prepare));
	TRY(db_sql_bind_text(count_device, 1, client_id, -1, NULL));
	TRY(db_sql_step(count_device, NULL));
	TRY(db_sql_get_int(count_device, 0, &count));
	TRY(db_sql_finalize(count_device));

    CATCH(count > 1){
		return DB_DATABASE_CORRUPT;
    }
    
    if(count == 0){
        *is_free = 1;
    } else {
        *is_free = 0;
    }
    
    return DB_OK;
}

/* check if the device is connected based on his address */
/*-------------------------------------------------------------------------------*/
static ERROR db_check_address_connected(ADDRESS address, FLAG* is_connected){
/*-------------------------------------------------------------------------------*/
	*is_connected = 0;	
	
	DATABASE_ID device_id = 0;
	TRY(db_get_device_id_address(address, &device_id));

	uint32_t count  = 0; 
	char * sql_get_client_count_prepare = "SELECT count(device_id) as amount FROM connect WHERE device_id = ? AND valid = 1;";
   
	statement_ptr count_device;

	TRY(db_sql_prepare(&count_device, sql_get_client_count_prepare));
	TRY(db_sql_bind_int(count_device, 1, device_id));
	TRY(db_sql_step(count_device, NULL));
	TRY(db_sql_get_int(count_device, 0, &count));
	TRY(db_sql_finalize(count_device));

    CATCH(count > 1){
		return DB_DATABASE_CORRUPT;
    }

    if(count == 0){
        *is_connected = 1;
    } else {
        *is_connected = 0;
    }
    
    return DB_OK;
}

/* check if the device is connected based on his id */
/*-------------------------------------------------------------------------------*/
static ERROR db_check_device_connected(DATABASE_ID device_id, FLAG* is_connected){
/*-------------------------------------------------------------------------------*/
	*is_connected = 0;	

	uint32_t count  = 0; 
	char * sql_get_client_count_prepare = "SELECT count(device_id) as amount FROM connect WHERE device_id = ? AND valid = 1;";
   
	statement_ptr count_device;

	TRY(db_sql_prepare(&count_device, sql_get_client_count_prepare));
	TRY(db_sql_bind_int(count_device, 1, device_id));
	TRY(db_sql_step(count_device, NULL));
	TRY(db_sql_get_int(count_device, 0, &count));
	TRY(db_sql_finalize(count_device));

    CATCH(count > 1){
		return DB_DATABASE_CORRUPT;
    }

    if(count == 0){
        *is_connected = 1;
    } else {
        *is_connected = 0;
    }
    
    return DB_OK;
}

/*********************************************************************************
**		Clear functions
**********************************************************************************/

/* remove the will message and the incompleet will message from a device based on its id */
/*-------------------------------------------------------------------------------*/
static ERROR db_clear_all_will(DATABASE_ID device_id){
/*-------------------------------------------------------------------------------*/
	char* sql_clear_subscribtion_prepare = "UPDATE WILL SET valid = 0 WHERE device_id = ?;";
	statement_ptr clear_subscribtion;

	TRY(db_sql_prepare(&clear_subscribtion, sql_clear_subscribtion_prepare));
	TRY(db_sql_bind_int(clear_subscribtion, 1, device_id));
	TRY(db_sql_step(clear_subscribtion, NULL));
	TRY(db_sql_finalize(clear_subscribtion));

	return DB_OK;
}

/* remove all the topic a device is subscribed to based on its id */
/*-------------------------------------------------------------------------------*/
static ERROR db_clear_topic_device(DATABASE_ID device_id){
/*-------------------------------------------------------------------------------*/
	char* sql_clear_topic_device_prepare = "UPDATE topic_device SET valid = 0 WHERE device_id = ?;";
	statement_ptr clear_topic_device;

	TRY(db_sql_prepare(&clear_topic_device, sql_clear_topic_device_prepare));
	TRY(db_sql_bind_int(clear_topic_device, 1, device_id));
	TRY(db_sql_step(clear_topic_device, NULL));
	TRY(db_sql_finalize(clear_topic_device));

	return DB_OK;
}

/* clear all the connections from a device based on its id */
/*-------------------------------------------------------------------------------*/
static ERROR db_clear_connect_device(DATABASE_ID device_id){
/*-------------------------------------------------------------------------------*/
	char* sql_clear_connect_prepare = "UPDATE connect SET valid = 0 WHERE device_id = ?;";
	statement_ptr clear_connect;

	TRY(db_sql_prepare(&clear_connect, sql_clear_connect_prepare));
	TRY(db_sql_bind_int(clear_connect, 1, device_id));
	TRY(db_sql_step(clear_connect, NULL));
	TRY(db_sql_finalize(clear_connect));

	return DB_OK;
}

/* clear all the connections based on an address */
/*-------------------------------------------------------------------------------*/
static ERROR db_clear_connect_address(ADDRESS address){
/*-------------------------------------------------------------------------------*/
	char* sql_clear_connect_prepare = "UPDATE connect SET valid = 0 WHERE address = ?;";
	statement_ptr clear_connect;

	TRY(db_sql_prepare(&clear_connect, sql_clear_connect_prepare));
	TRY(db_sql_bind_int64(clear_connect, 1, address));
	TRY(db_sql_step(clear_connect, NULL));
	TRY(db_sql_finalize(clear_connect));

	return DB_OK;
}

/* remove all the messages from a device based on its id */
/*-------------------------------------------------------------------------------*/
static ERROR db_clear_message_device(DATABASE_ID device_id){
/*-------------------------------------------------------------------------------*/
	char* sql_clear_message_device_prepare = "UPDATE message_device SET valid = 0 WHERE device_id = ?;";
	statement_ptr clear_message_device;

	TRY(db_sql_prepare(&clear_message_device, sql_clear_message_device_prepare));
	TRY(db_sql_bind_int(clear_message_device, 1, device_id));
	TRY(db_sql_step(clear_message_device, NULL));
	TRY(db_sql_finalize(clear_message_device));

	return DB_OK;
}

/* clear the incompleet will messages from a device based on its id */
/*-------------------------------------------------------------------------------*/
static ERROR db_clear_incompleet_will_device(DATABASE_ID device_id){
/*-------------------------------------------------------------------------------*/
	char* sql_clear_subscribtion_prepare = "UPDATE WILL SET valid = 0 WHERE device_id = ? AND incompleet = 1;";
	statement_ptr clear_subscribtion;

	TRY(db_sql_prepare(&clear_subscribtion, sql_clear_subscribtion_prepare));
	TRY(db_sql_bind_int(clear_subscribtion, 1, device_id));
	TRY(db_sql_step(clear_subscribtion, NULL));
	TRY(db_sql_finalize(clear_subscribtion));

	return DB_OK;
}

/* clear all the messages that are going out from the database for a device based on its id */
/*-------------------------------------------------------------------------------*/
static ERROR db_clear_message_to_device(DATABASE_ID device){
/*-------------------------------------------------------------------------------*/
	char* clear_message_prepare = "UPDATE MESSAGE_DEVICE SET valid = 0, last_change = ? WHERE device_id = ? AND to_database = 0 AND completed_on IS NULL;";
	 
	statement_ptr clear_message;

	TRY(db_sql_prepare(&clear_message, clear_message_prepare));
	TRY(db_sql_bind_int(clear_message, 1, time(NULL)));
	TRY(db_sql_bind_int(clear_message, 2, device));
	TRY(db_sql_step(clear_message, NULL));
	TRY(db_sql_finalize(clear_message));

	return DB_OK;
}

/* clear all the messages that are comming in to the database for a device based on its id */
/*-------------------------------------------------------------------------------*/
static ERROR db_clear_message_to_database(DATABASE_ID device){
/*-------------------------------------------------------------------------------*/
	char* clear_message_prepare = "UPDATE MESSAGE_DEVICE SET valid = 0, last_change = ? WHERE device_id = ? AND to_database = 1 AND completed_on IS NULL;";
	 
	statement_ptr clear_message;

	TRY(db_sql_prepare(&clear_message, clear_message_prepare));
	TRY(db_sql_bind_int(clear_message, 1, time(NULL)));
	TRY(db_sql_bind_int(clear_message, 2, device));
	TRY(db_sql_step(clear_message, NULL));
	TRY(db_sql_finalize(clear_message));

	return DB_OK;
}

/*********************************************************************************
**		returns an error message
**********************************************************************************/

char* db_get_error_message(ERROR error_number){
	switch(error_number){
		case DB_OK : return "DB_OK: Asked for error message without error";
		case DB_UNSPECIFIED_ERROR : return "DB_UNSPECIFIED_ERROR: The error is not further specified, sorry";
		case DB_ADDRESS_ALREADY_USED : return "DB_ADDRESS_ALREADY_USED: A request came in for an address but the requested address is already in use for another device.";
		case DB_DATABASE_ERROR : return "DB_DATABASE_ERROR: An error occured when trying to execute a query";
		case DB_CANNOT_OPEN_DATABASE : return "DB_CANNOT_OPEN_DATABASE: Unable to open the database, is the database locked?";
		case DB_NO_MATCH_FOUND : return "DB_NO_MATCH_FOUND: When searching in the database for a record matching the givin parameters, no match was found. Normaly, a match should be found.";
		case DB_DEVICE_NOT_FOUND : return "DB_DEVICE_NOT_FOUND: When searching in the database for the device, no match was found. The device was never added to the database or is no longer active.";
		case DB_PAD_TO_LONG : return "DB_PAD_TO_LONG: The proviced pad is to long to parse. Check the pad or set a higher limit.";
		case DB_NO_WILDCARDS_ALLOWED: return "DB_NO_WILDCARD_ALLOWED: The provided pad containes wildcards, these are not allowed. please escape them using \\";
		case DB_MEMORY_ALLOCATION_FAILED: return "DB_MEMORY_ALLOCATION_FAILED: The memory allocation failed. It is no longer possible to uptain the needed memory. Please restart.";
		case DB_NO_DATABASE_SELECTED : return "DB_NO_DATABASE_SELECTED: There is no database selected, pleas look at the init function";
		case DB_DATABASE_CORRUPT : return "DB_DATABASE_CORRUPT: query returned a result witch is not possible if the database is used normal";
		case DB_COULD_NOT_FREE : return "DB_COULD_NOT_FREE: The database was unable to Free the used memory. This is a serious problem. Please restart.";
		case DB_NO_INPUT_DATA : return "DB_NO_INPUT_DATA: Their was not enough input for this function. Please check the input you give.";
		case DB_CANNOT_CREATE_TABLE : return "DAT_CANNOT_CREATE_TABLE: The database was unable to create the requested table. Please check or restart using another database.";
		case DB_DANGEROUS_FUNCTION_CALL : return "DB_DANGEROUS_FUNCTION_CALL: Before running the function, some conditions where checked for safety, \
one condition gave an error. You saved this time, but please check all the conditions before calling the function!";
		case DB_IMPLEMENTATION_ERROR : return "DB_IMPLEMENATION_ERROR: While executing the code, a condition was checked, this condtion was not met. This means, that previously a function was called that return invalid data without throuwing an error. Please find the lazy bastard!";
		case DB_INVALID_INPUT : return "DB_INVALID_INPUT: The input given to the function is not valid, please check.";
		case DB_PRIORITIZED_LIST_ERROR: return "DB_PRIORITIZED_LIST_ERROR: When trying to use the prioritized list implementation, an internal error occured. Please check the feedback from prioritized list implementation for a more detailed discription of the error.";
		case DB_UNDEFINED_BEHAVIOUR: return "DB_UNDEFINED_BEHAVIOUR: The code was compiled without setting the approriate flags or when initializing a data structure, not all options where set. Please correct this error by setting them and write control functions to make sure the variables are set at compile time.";
	 	case DB_DEVICE_NOT_SUBSCRIBED: return "DB_DEVICE_NOT_SUBSCRIBED: The device is not subscribed to the topic, yet it was requested to send a message from this topic. This is a real bad idea and an implementation error.";
		case DB_WILL_TOPIC_NOT_SET: return "DB_WILL_TOPIC_NOT_SET: A request came in to set the will message, their is no incompleet will topic set or the process of setting the will is not started. Please always handle the will topic first before trying to set the will message.";
		case DB_PIPE_ERROR: return "DB_IPE_ERROR: an error occured when trying to write to a pipe. The process will stop, please check the other processes are still running and have an open pipe an reboot.";
		default : return "The error is not further specified, sorry. The error is not even in the normal range of errors. This suggests an error within the error handling.";
	}
}

/*********************************************************************************
**		SQL functions
**********************************************************************************/

/*********************************************************************************
**		SQL statement wrapper
**********************************************************************************/

/* this statement wrapper function around prepare step and finalize */
/*-------------------------------------------------------------------------------*/
static ERROR db_sql(char* sql_statement){
/*-------------------------------------------------------------------------------*/
	CATCH_NO_DB();

	statement_ptr sql_stmt;

	TRY(db_sql_prepare(&sql_stmt, sql_statement));
	TRY(db_sql_step(sql_stmt, NULL));
	TRY(db_sql_finalize(sql_stmt));

	return DB_OK;
}

/*********************************************************************************
**		SQL prepare statements
**********************************************************************************/

/* sends a prepared statement to the database to be parsed */
/*-------------------------------------------------------------------------------*/
static ERROR db_sql_prepare(statement_ptr *stmt_ptr, char* sql){
/*-------------------------------------------------------------------------------*/
	uint32_t sqlite3_error = 0;
	CATCH_NO_DB();
	sqlite3_error = sqlite3_prepare_v2(db, (void*) sql, strlen(sql), stmt_ptr, NULL);
	CATCH_SQL_ERROR(sqlite3_error);
	return DB_OK;
}

/*********************************************************************************
**		SQL bind data
**********************************************************************************/

//TODO there is a problem using this function, please check before using 
// not binding a ? (should default to NULL) doesn't work. (should set a flag to allow?)
/* binds NULL to a prepared statement */
/*-------------------------------------------------------------------------------*/
//static ERROR db_sql_bind_null(statement_ptr stmt_ptr, uint32_t index){
/*-------------------------------------------------------------------------------*/
//	uint32_t sqlite3_error = 0;
//	sqlite3_error = sqlite3_bind_null(stmt_ptr, index);
//	CATCH_SQL_ERROR(sqlite3_error);
//	return DB_OK;
//}

/* binds an integer to a prepared statement */
/*-------------------------------------------------------------------------------*/
static ERROR db_sql_bind_int(statement_ptr stmt_ptr, uint32_t index, uint32_t input){
/*-------------------------------------------------------------------------------*/
	uint32_t sqlite3_error = 0;
	sqlite3_error = sqlite3_bind_int(stmt_ptr, index, input);
	CATCH_SQL_ERROR(sqlite3_error);
	return DB_OK;
}

/* binds an integer of 64 to a prepared statement */
/*-------------------------------------------------------------------------------*/
static ERROR db_sql_bind_int64(statement_ptr stmt_ptr, uint32_t index, uint64_t input){
/*-------------------------------------------------------------------------------*/
	uint32_t sqlite3_error = 0;
	sqlite3_error = sqlite3_bind_int64(stmt_ptr, index, input);
	CATCH_SQL_ERROR(sqlite3_error);
	return DB_OK;
}

/* binds a text to a prepared statement */
/*-------------------------------------------------------------------------------*/
static ERROR db_sql_bind_text(statement_ptr stmt_ptr, uint32_t index, char* txt, uint32_t length, void (*destroy) (void*)){
/*-------------------------------------------------------------------------------*/
	uint32_t sqlite3_error = 0;
	sqlite3_error = sqlite3_bind_text(stmt_ptr, index, txt, length, destroy);
	CATCH_SQL_ERROR(sqlite3_error);
	return DB_OK;
}

/* binds a blob to a prepared statement */ 
/*-------------------------------------------------------------------------------*/
static ERROR db_sql_bind_blob(statement_ptr stmt_ptr, uint32_t index, const void* blob, uint32_t length, void(*destroy)(void*)){
/*-------------------------------------------------------------------------------*/
	uint32_t sqlite3_error = 0;
	sqlite3_error = sqlite3_bind_blob(stmt_ptr, index, blob, length, destroy);
	CATCH_SQL_ERROR(sqlite3_error);
	return DB_OK;
	}

/*********************************************************************************
**		SQL execute statement
**********************************************************************************/

/* executes a statement */ 
/*-------------------------------------------------------------------------------*/
static int db_sql_step(statement_ptr stmt_ptr, FLAG* result){
/*-------------------------------------------------------------------------------*/
	if(result != NULL){
		*result = 0;
		uint32_t sqlite3_error = 0;
		sqlite3_error = sqlite3_step(stmt_ptr);
		if(sqlite3_error == 100)
			*result = 1;
		CATCH_SQL_ERROR(sqlite3_error);
	return DB_OK;
	} else {
		uint32_t sqlite3_error = 0;
		sqlite3_error = sqlite3_step(stmt_ptr);
		CATCH_SQL_ERROR(sqlite3_error);
	}
	return DB_OK;
}

/*********************************************************************************
**		SQL get data
**********************************************************************************/

/* returns a integer from an executed statement */ 
/*-------------------------------------------------------------------------------*/
static ERROR db_sql_get_int(statement_ptr stmt_ptr, uint32_t index, uint32_t* result){
/*-------------------------------------------------------------------------------*/
	*result = 0;
	*result = sqlite3_column_int(stmt_ptr, index);
	return DB_OK;
}

/* returns a int of 64 bit from an executed statement */ 
/*-------------------------------------------------------------------------------*/
static ERROR db_sql_get_int64(statement_ptr stmt_ptr, uint32_t index, uint64_t* result){
/*-------------------------------------------------------------------------------*/
	*result = 0;
	*result = sqlite3_column_int64(stmt_ptr, index);
	return DB_OK;
}

/* returns a blob of data from an executed statement */ 
/*-------------------------------------------------------------------------------*/
static ERROR db_sql_get_text(statement_ptr stmt_ptr, uint32_t index, blob* result){
/*-------------------------------------------------------------------------------*/
	blob new_blob;
	new_blob.data = sqlite3_column_text(stmt_ptr, index);
	new_blob.size = sqlite3_column_bytes(stmt_ptr, index);
	*result = new_blob;
	return DB_OK;
}

/* returns a blob of data from an executed statement */ 
/*-------------------------------------------------------------------------------*/
static ERROR db_sql_get_blob(statement_ptr stmt_ptr, uint32_t index, blob* result){
/*-------------------------------------------------------------------------------*/
	blob new_blob;
	new_blob.data = sqlite3_column_blob(stmt_ptr, index);
	new_blob.size = sqlite3_column_bytes(stmt_ptr, index);
	*result = new_blob;
	return DB_OK;
}

/*********************************************************************************
**		SQL reset and finalize
**********************************************************************************/

/* resets a statement and prepares it to bind new parameters and execute again */
/*-------------------------------------------------------------------------------*/
static ERROR db_sql_reset(statement_ptr stmt_ptr){
/*-------------------------------------------------------------------------------*/
	uint32_t sqlite3_error = 0;
	sqlite3_error = sqlite3_reset(stmt_ptr);
	CATCH_SQL_ERROR(sqlite3_error);
	return DB_OK;
}

/* finalizes a statement if it is not used anymore */
/*-------------------------------------------------------------------------------*/
static ERROR db_sql_finalize(statement_ptr stmt_ptr){
/*-------------------------------------------------------------------------------*/
	uint32_t sqlite3_error = 0;
	sqlite3_error = sqlite3_finalize(stmt_ptr);
	CATCH_SQL_ERROR(sqlite3_error);
	return DB_OK;
}
