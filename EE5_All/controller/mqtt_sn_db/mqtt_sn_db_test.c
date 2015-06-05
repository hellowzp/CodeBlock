#include "mqtt_sn_db.h"
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <sqlite3.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "parser/parser.h"
#include "prioritized_list/prioritized_list.h"

/*********************************************************************************
**		Test code
**********************************************************************************/

#define CATCH_SQL_ERROR_PREPARE(sqlite3_error, db)															\
	do{																													\
		if(sqlite3_error != SQLITE_OK && sqlite3_error != SQLITE_DONE									\
			 && sqlite3_error != SQLITE_ROW){																	\
			PRINT_ERROR_STRING_SQL(sqlite3_error,sqlite3_errmsg(db));										\
			return -1;																									\
		}																													\
	}while(0)

#define CATCH_SQL_ERROR(sqlite3_error, stmt_ptr)															\
	CATCH_SQL_ERROR_PREPARE(sqlite3_error, sqlite3_db_handle(stmt_ptr))

#ifdef DEBUG
	#define PRINT_ERROR_STRING_SQL(error_code, error_string)										\
		do{																										\
			printf("\nint line %d of file %s by function %s:\n   Error %d: %s\n",			\
			__LINE__, __FILE__,__func__, error_code, error_string);								\
			fflush(stdout);}																					\
		while(0)
#else
	#define PRINT_ERROR_STRING(c, s)																		\
		do{};																										\
		while(0)
#endif

#ifdef DEBUG
	#define PRINT_ERROR(error_number)																	\
		do{																										\
			printf("\nint line %d of file %s by function %s:\n   Error %d: %s\n",			\
			__LINE__, __FILE__,__func__,error_number, get_error_message(error_number));\
		}while(0)
#else
	#define PRINT_ERROR(s)																					\
		do{}while(0)
#endif

#define TRY(db_function)																			\
    do{																									\
        int db_error = db_function;																\
        if(db_error){																				\
			  PRINT_ERROR(db_error);																\
			  return db_error; 																		\
		  }																								\
    }																										\
    while(0)
		 
#define COMPARE_INT(name, pos)																	\
	do{if(db_compare_int(equal, name, stmt_db, stmt_ref, pos, row, table))			\
			return -1;} while(0)

#define COMPARE_INT64(name, pos)																	\
	do{if(db_compare_int64(equal, name, stmt_db, stmt_ref, pos, row, table))		\
			return -1;} while(0)

#define COMPARE_TEXT(name, pos)																	\
	do{if(db_compare_text(equal, name, stmt_db, stmt_ref, pos, row, table))			\
			return -1;} while(0)

#define COMPARE_BLOB(name, pos)																	\
	do{if(db_compare_blob(equal, name, stmt_db, stmt_ref, pos, row, table))			\
			return -1;} while(0)
#define FINALIZE																						\
	do{TRY(db_sql_finalize(stmt_db));															\
	TRY(db_sql_finalize(stmt_ref));}while(0)													\


/*********************************************************************************
**		datatypes for tests
**********************************************************************************/

typedef struct Db_test_item{
	struct Db_test_item* next;
	pthread_t test;
	uint32_t test_number;
	char* output_file;
	char* output_file_db;
} db_test_item;

typedef struct Test_info{
	int (*test)(char* output_file);
	char* output_file;
	char* output_file_db;
} test_info;

typedef sqlite3_stmt* statement_ptr;

/*********************************************************************************
**		prototypes for testing functions
**********************************************************************************/
void db_add_test(uint32_t test_number, db_test_item** tests, db_test_item** last, char* output_file, char* output_db);
void db_free_tests(db_test_item** tests);
void *db_start_test_process(void* test);
void db_run_test(uint32_t test_number, db_test_item** tests, char* message, int (*test)(char* output_file), char* output_file);
void db_finish_tests(db_test_item** tests, uint32_t* succeded, uint32_t* failed);
static int db_receive_message_pipe(MESSAGE** mes, int fd_in);
static int db_send_message_pipe(MESSAGE* m, int write_ptr);
char* get_error_message(int error_number);
int db_create_message(MESSAGE** mes, ADDRESS address, DATABASE_ID message_id, EXTERN_ID extern_message_id, EXTERN_ID short_topic_id, MESSAGE_TYPE type, TOPIC_TYPE topic_type, QOS qos, time_t duration, uint32_t connection_id, FLAG duplicate, FLAG will, FLAG clean, FLAG retain, return_code return_value, blob* b);

int db_test_1(char* output_file);
int db_test_2(char* output_file);
int db_test_3(char* output_file);
int db_test_4(char* output_file);
int db_test_5(char* output_file);
int db_test_6(char* output_file);
int db_test_7(char* output_file);
void* test_7_output(void * file);

/*-------------------------------------------------------------------------------*/
int main(){
/*-------------------------------------------------------------------------------*/	
	printf("\n\n+--------------------------------------------starting up all the tests---------------------------------------------------+\n");
	printf(    "|                                                                                                                        |\n");
	printf(    "|   +----------------------------------------running the database tests----------------------------------------------+   |\n");
	printf(    "|   |                                                                                                                |   |\n");
	printf(    "|   |                                                                                                                |   |\n");

	db_test_item* tests = NULL;
	uint32_t succes = 0;
	uint32_t failed = 0;

	remove("test_1.db");
	remove("test_2.db");
	remove("test_3.db");
	remove("test_4.db");
	remove("test_5.db");
	remove("test_6.db");
	remove("test_7.db");
//	db_run_test(1, &tests, "Trying to open the database process and close it again. (hard stop with quit signal)", db_test_1, "test_1.txt");
//	db_run_test(2, &tests, "Trying to open the database process and close it again. (soft stop with term signal)", db_test_2, "test_2.txt");
	db_run_test(3, &tests, "Trying to connect a device to the database", db_test_3, "test_3.txt");
	db_run_test(4, &tests, "Trying to connect a device to the database and disconnect it again", db_test_4, "test_4.txt");
	db_run_test(5, &tests, "Trying to connect and subscribe to a topic", db_test_5, "test_5.txt");
	db_run_test(6, &tests, "Trying to connect, subscribe to a topic and send a message to that topic", db_test_6, "test_6.txt");
	db_run_test(7, &tests, "Connecting 2 devices, let 1 register and publish a message and let the other device subscribe. If that is finished, unsubscribe them", db_test_7, "test_7.txt");

	db_finish_tests(&tests, &succes, &failed);

	printf(        "|   |                                                                                                                |   |\n");
	printf(        "|   |                                                                                                                |   |\n");
	printf(        "|   +------------------------------------------All database tests done-----------------------------------------------+   |\n");
	printf(        "|                                                                                                                        |\n");
	printf(        "+---------------------------------------------------All tests done-------------------------------------------------------+\n");
	printf(    "\n\n                                   +----------------results---------------+\n");
	printf(        "                                   |                                      |\n");
	printf(        "                                   |               total:  %3d            |\n", succes+failed);
	printf(        "                                   |               passed: %3d            |\n", succes);
	printf(        "                                   |               failed: %3d            |\n", failed);
	printf(        "                                   +--------------------------------------+\n\n\n");

	exit(0);
}

/*-------------------------------------------------------------------------------*/
void db_add_test(uint32_t test_number, db_test_item** tests, db_test_item** last, char* output, char* output_db){
/*-------------------------------------------------------------------------------*/	
	db_test_item* current = NULL;
	if(*tests == NULL){
		*tests = (db_test_item*) malloc(sizeof(db_test_item));
		(*tests)->next = NULL;
		current = *tests;
	} else {
		current = *tests;
		while(current->next != NULL){
			current = current->next;
		}
		current->next = (db_test_item*) malloc(sizeof(db_test_item));
		current = current->next;
		current->next = NULL;
	}
	current->test_number = test_number;
	current->output_file = output;
	current->output_file_db = output_db;

	*last = current;
}

/*-------------------------------------------------------------------------------*/
void db_free_tests(db_test_item** tests){
/*-------------------------------------------------------------------------------*/	
	db_test_item* current = *tests;
	db_test_item* next = NULL;
	if(current == NULL)
		return;
	
	next = current->next;
	while(next != NULL){
		 free(current);
		current = next;
		next = next->next;
	}
	 free(current->output_file_db);
	 free(current);
	*tests = NULL;
}

/*-------------------------------------------------------------------------------*/
void db_finish_tests(db_test_item** tests, uint32_t* succeded, uint32_t* failed){
/*-------------------------------------------------------------------------------*/	
	db_test_item* current = *tests;
	int c = 0;
	FILE* file;
	if(current == NULL)
		return;
	void* return_value;
	do{
		pthread_join(current->test, &return_value);
		printf(  "|   |   +--------------------------------------------------------------------------------------------------------+   |   |\n");
		printf(  "|   |   | Results of test %3d                                                                                    |   |   |\n", current->test_number);
		if(return_value == NULL){
			printf(  "|   |   | The test was succesfull! Congratulations!                                                              |   |   |\n");
			(*succeded) ++;
		} else {
			printf(  "|   |   | The test failed.                                                                                       |   |   |\n");
			(*failed) ++;
		}
			printf(  "|   |   |                                                                                                        |   |   |\n");
		file = fopen(current->output_file, "r");
		if (file) {
			uint32_t row = 0;
			while ((c = getc(file)) != EOF){
				if(row == 0)
					printf("|   |   |  ");
				if(row == 100){
				 row = 1;
				 printf("  |   |   |\n|   |   |  %c", c);
				}
				if((char) c == '\n'){
					for(; row<102; row++){
						printf(" ");
					}
					printf("|   |   |\n");
					row = 0;
				} else {
					printf("%c", (char) c);
					row ++;
				}
			}
			fclose(file);
		}
		file = fopen(current->output_file_db, "r");
		if (file) {
			uint32_t row = 0;
			while ((c = getc(file)) != EOF){
				if(row == 0)
					printf("|   |   |  ");
				if((char) c == '\n'){
					for(; row<102; row++){
						printf(" ");
					}
					printf("|   |   |\n");
					row = 0;
				} else {
					if(row == 100){
					 	row = 1;
					 	printf("  |   |   |\n|   |   |  %c", c);
					} else {
						printf("%c", (char) c);
						row ++;
					}
				}
			}
			fclose(file);
		}

		remove(current->output_file);
		remove(current->output_file_db);

		printf("|   |   |                                                                                                        |   |   |\n");
		printf("|   |   +--------------------------------------------------------------------------------------------------------+   |   |\n");
		printf("|   |                                                                                                                |   |\n");
		current = current->next;
	} while(current != NULL);
	db_free_tests(tests);
}

/*-------------------------------------------------------------------------------*/
void db_run_test(uint32_t test_number, db_test_item** tests, char* message, int (*test)(char* output_file), char* output){
/*-------------------------------------------------------------------------------*/	
	printf("|   |  -Running test: ");
	uint32_t count = 0;
	uint32_t row = 0;
	for(count=0; count <= strlen(message); count++){
		if(*(message+count) != '\n' && row == 93){
			row = 0;
			printf("  |   |\n|   |                 ");
		}
		if(*(message+count) == '\n'){
			for(;row<93; row++){
				printf(" ");
			}
			row = 0;
			printf("  |   |\n|   |                 ");
		}else {
			row++;
			printf("%c", *(message + count));
		}
	}
	for(;row<94; row++){
		printf(" ");
	}
	printf("  |   |\n");
	printf(  "|   |                                                                                                                |   |\n");
	db_test_item* last;
	test_info* new_test = (test_info*) malloc(sizeof(test_info));
	new_test->test = test;
	new_test->output_file = output;
	new_test->output_file_db = (char*) malloc(strlen(output)+4);

	uint32_t chars = 0;
	for(chars = 0; chars < strlen(output); chars++){
		*((new_test->output_file_db)+chars) = *((new_test->output_file)+chars);
	}
	strcat(new_test->output_file_db + strlen(output), "_db");
	*((new_test->output_file_db)+strlen(output)+3) = '\0';
	db_add_test(test_number, tests, &last, output, new_test->output_file_db);
	pthread_create(&last->test, NULL, db_start_test_process, (void*) new_test);
}

/* start a process to do the test in without risking other tests */
/*-------------------------------------------------------------------------------*/
void *db_start_test_process(void* test){
/*-------------------------------------------------------------------------------*/	
	int64_t test_return_value = -1;
	test_info* this_test = (test_info*) test;
	int(*new_test)(char*) = this_test->test;

	int child_pid;
	child_pid = fork();
	
	if(child_pid == 0){
		FILE* fp = freopen(this_test->output_file, "w+", stdout);
		setlinebuf(fp);
		test_return_value = new_test(this_test->output_file_db);
		printf("\nTest ran compleetly, their where no segmentation faults.\n\nThe output of the database: \n\n");
		fclose(fp);
		 free(this_test);
		exit(test_return_value);
	}

	if(child_pid <0 ){
		printf("---Unable to open a test process\n");
		pthread_exit((void*) -1);
	}

	if(child_pid > 0){
		int process_return = -1;
		waitpid(child_pid, &process_return, 0);
		if(process_return != 0){
			pthread_exit((void*) (int64_t) process_return);
		} else {
			pthread_exit((void*) (int64_t) process_return);
		}
	}
	return 0;
}

static int db_send_message_pipe(MESSAGE* m, int write_ptr){
	uint32_t written = write(write_ptr, m, sizeof(MESSAGE));
	if(written != sizeof(MESSAGE)){
		printf("not everything written\n");
		//TODO do something
	} else {
		if(m->length != 0){
			written = write(write_ptr, m->message, m->length);
			if(written != m->length){
				//TODO do something (the same)
			} else {
				//TODO don't free but reuse
				 free((void*) m->message);
				 free((void*)m);
			}
		} else {
			 free((void*)m);
		}
	}
	return 0;
}

static int db_receive_message_pipe(MESSAGE** mes, int fd_in){
		*mes = (MESSAGE*) malloc(sizeof(MESSAGE));
		MESSAGE* m = *mes;
		if(m == NULL){
			printf("ERROR: malloc did not work! crashing,...\n");
			exit(0);
		}

		uint32_t bytes_read = read(fd_in, m, sizeof(MESSAGE));
		if(bytes_read != sizeof(MESSAGE)){
			return -1;
		}

		if(m->length != 0){
			void* data = malloc(m->length);
			bytes_read = read(fd_in, data, m->length);
			m->message = data;
		}

	return 0;
}

char* get_error_message(int error_number){
	printf("error number: %d\n", error_number);
	return "not yet implemented";
}

/*typedef struct Message{
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
 } message;*/


int db_create_message(MESSAGE** mes, ADDRESS address, DATABASE_ID message_id, EXTERN_ID extern_message_id, EXTERN_ID short_topic_id, MESSAGE_TYPE type, TOPIC_TYPE topic_type, QOS qos, time_t duration, uint32_t connection_id, FLAG duplicate, FLAG will, FLAG clean, FLAG retain, return_code return_value, blob* b){
	MESSAGE* m = (MESSAGE*) malloc (sizeof(MESSAGE));
	m->address = address;
	m->message_id = message_id;
	m->extern_message_id = extern_message_id;
	m->short_topic_id = short_topic_id;
	m->type = type;
	m->topic_type = topic_type;
	m->qos = qos;
	m->duration = duration;
	m->connection_id = connection_id;
	m->duplicate = duplicate;
	m->will = will;
	m->clean = clean;
	m->retain = retain;
	m->return_value = return_value;

	if(b != NULL){
		m->length = b->size;
		m->message = b->data;
		 free(b);
	} else {
		m->length = 0;
		m->message = NULL;
	}
	*mes = m;
	return 0;
}

int db_create_blob(blob** blb, char* input){
	blob* b = (blob*) malloc(sizeof(blob));
	b->data = malloc(strlen(input)+1);
	strncpy((char*) b->data, input, strlen(input)+1);
	b->size = strlen(input)+1;
	*blb = b;
	return 0;
}

/*********************************************************************************
**		SQL functions
**********************************************************************************/

//static int db_sql(sqlite3* db, char* sql_statement);
static int db_sql_prepare(sqlite3* db, statement_ptr *stmt_ptr, char* sql);
//static int db_sql_bind_null(statement_ptr stmt_ptr, uint32_t index);
//static int db_sql_bind_int(statement_ptr stmt_ptr, uint32_t index, uint32_t input);
//static int db_sql_bind_int64(statement_ptr stmt_ptr, uint32_t index, uint64_t input);
//static int db_sql_bind_text(sqlite3* db, statement_ptr stmt_ptr, uint32_t index, char* txt, uint32_t length, void (*destroy) (void*));
//static int db_sql_bind_blob(statement_ptr stmt_ptr, uint32_t index, const void* blob, uint32_t length, void(*destroy)(void*));
static int db_sql_step(statement_ptr stmt_ptr, FLAG* result);
static int db_sql_get_int(statement_ptr stmt_ptr, uint32_t index, uint32_t* result);
static int db_sql_get_int64(statement_ptr stmt_ptr, uint32_t index, uint64_t* result);
static int db_sql_get_text(statement_ptr stmt_ptr, uint32_t index, char** result);
static int db_sql_get_blob(statement_ptr stmt_ptr, uint32_t index, blob* result);
//static int db_sql_reset(statement_ptr stmt_ptr);
static int db_sql_finalize(statement_ptr stmt_ptr);

/*********************************************************************************
**		SQL statement wrapper
**********************************************************************************/

/* this statement wrapper function around prepare step and finalize */
/*-------------------------------------------------------------------------------*/
//static int db_sql(sqlite3* db, char* sql_statement){
/*-------------------------------------------------------------------------------*/
/*	statement_ptr sql_stmt;

	TRY(db_sql_prepare(db, &sql_stmt, sql_statement));
	TRY(db_sql_step(sql_stmt, NULL));
	TRY(db_sql_finalize(sql_stmt));

	return 0;
}
*/
/*********************************************************************************
**		SQL prepare statements
**********************************************************************************/

/* sends a prepared statement to the database to be parsed */
/*-------------------------------------------------------------------------------*/
static int db_sql_prepare(sqlite3* db, statement_ptr *stmt_ptr, char* sql){
/*-------------------------------------------------------------------------------*/
	uint32_t sqlite3_error = 0;
	sqlite3_error = sqlite3_prepare_v2(db, (void*) sql, strlen(sql), stmt_ptr, NULL);
	CATCH_SQL_ERROR_PREPARE(sqlite3_error, db);
	return 0;
}

/*********************************************************************************
**		SQL bind data
**********************************************************************************/

/* binds NULL to a prepared statement */
/*-------------------------------------------------------------------------------*/
//static int db_sql_bind_null(statement_ptr stmt_ptr, uint32_t index){
/*-------------------------------------------------------------------------------*/
/*	uint32_t sqlite3_error = 0;
	sqlite3_error = sqlite3_bind_null(stmt_ptr, index);
	CATCH_SQL_ERROR(sqlite3_error, stmt_ptr);
	return 0;
}
*/
/* binds an integer to a prepared statement */
/*-------------------------------------------------------------------------------*/
//static int db_sql_bind_int(statement_ptr stmt_ptr, uint32_t index, uint32_t input){
/*-------------------------------------------------------------------------------*/
/*	uint32_t sqlite3_error = 0;
	sqlite3_error = sqlite3_bind_int(stmt_ptr, index, input);
	CATCH_SQL_ERROR(sqlite3_error, stmt_ptr);
	return 0;
}
*/
/* binds an integer of 64 to a prepared statement */
/*-------------------------------------------------------------------------------*/
//static int db_sql_bind_int64(statement_ptr stmt_ptr, uint32_t index, uint64_t input){
/*-------------------------------------------------------------------------------*/
/*	uint32_t sqlite3_error = 0;
	sqlite3_error = sqlite3_bind_int64(stmt_ptr, index, input);
	CATCH_SQL_ERROR(sqlite3_error, stmt_ptr);
	return 0;
}
*/
/* binds a text to a prepared statement */
/*-------------------------------------------------------------------------------*/
//static int db_sql_bind_text(sqlite3* db, statement_ptr stmt_ptr, uint32_t index, char* txt, uint32_t length, void (*destroy) (void*)){
/*-------------------------------------------------------------------------------*/
/*	uint32_t sqlite3_error = 0;
	sqlite3_error = sqlite3_bind_text(stmt_ptr, index, txt, length, destroy);
	CATCH_SQL_ERROR(sqlite3_error, stmt_ptr);
	return 0;
}
*/
/* binds a blob to a prepared statement */ 
/*-------------------------------------------------------------------------------*/
//static int db_sql_bind_blob(statement_ptr stmt_ptr, uint32_t index, const void* blob, uint32_t length, void(*destroy)(void*)){
/*-------------------------------------------------------------------------------*/
/*	uint32_t sqlite3_error = 0;
	sqlite3_error = sqlite3_bind_blob(stmt_ptr, index, blob, length, destroy);
	CATCH_SQL_ERROR(sqlite3_error, stmt_ptr);
	return 0;
	}
*/
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
		CATCH_SQL_ERROR(sqlite3_error, stmt_ptr);
	return 0;
	} else {
		uint32_t sqlite3_error = 0;
		sqlite3_error = sqlite3_step(stmt_ptr);
		CATCH_SQL_ERROR(sqlite3_error, stmt_ptr);
		return sqlite3_error;
	}
}

/*********************************************************************************
**		SQL get data
**********************************************************************************/

/* returns a integer from an executed statement */ 
/*-------------------------------------------------------------------------------*/
static int db_sql_get_int(statement_ptr stmt_ptr, uint32_t index, uint32_t* result){
/*-------------------------------------------------------------------------------*/
	*result = 0;
	*result = sqlite3_column_int(stmt_ptr, index);
	return 0;
}

/* returns a int of 64 bit from an executed statement */ 
/*-------------------------------------------------------------------------------*/
static int db_sql_get_int64(statement_ptr stmt_ptr, uint32_t index, uint64_t* result){
/*-------------------------------------------------------------------------------*/
	*result = 0;
	*result = sqlite3_column_int64(stmt_ptr, index);
	return 0;
}

/* returns a text from an executed statement */ 
/*-------------------------------------------------------------------------------*/
static int db_sql_get_text(statement_ptr stmt_ptr, uint32_t index, char** result){
/*-------------------------------------------------------------------------------*/
	*result = (char*) sqlite3_column_text(stmt_ptr, index);
	return 0;
}

/* returns a blob of data from an executed statement */ 
/*-------------------------------------------------------------------------------*/
static int db_sql_get_blob(statement_ptr stmt_ptr, uint32_t index, blob* result){
/*-------------------------------------------------------------------------------*/
	blob new_blob;
	new_blob.data = sqlite3_column_blob(stmt_ptr, index);
	new_blob.size = sqlite3_column_bytes(stmt_ptr, index);
	*result = new_blob;
	return 0;
}

/*********************************************************************************
**		SQL reset and finalize
**********************************************************************************/

/* resets a statement and prepares it to bind new parameters and execute again */
/*-------------------------------------------------------------------------------*/
//static int db_sql_reset(statement_ptr stmt_ptr){
/*-------------------------------------------------------------------------------*/
/*	uint32_t sqlite3_error = 0;
	sqlite3_error = sqlite3_reset(stmt_ptr);
	CATCH_SQL_ERROR(sqlite3_error, stmt_ptr);
	return 0;
}
*/
/* finalizes a statement if it is not used anymore */
/*-------------------------------------------------------------------------------*/
static int db_sql_finalize(statement_ptr stmt_ptr){
/*-------------------------------------------------------------------------------*/
	uint32_t sqlite3_error = 0;
	sqlite3_error = sqlite3_finalize(stmt_ptr);
	CATCH_SQL_ERROR(sqlite3_error, stmt_ptr);
	return 0;
}


/*********************************************************************************
**		Database compare functions
**********************************************************************************/

static int db_compare_int(FLAG* equal, char* var_name, statement_ptr stmt_db, statement_ptr stmt_ref, uint32_t pos, uint32_t row, char* table);
static int db_compare_int64(FLAG* equal, char* var_name, statement_ptr stmt_db, statement_ptr stmt_ref, uint32_t pos, uint32_t row, char* table);
static int db_compare_text(FLAG* equal, char* var_name, statement_ptr stmt_db, statement_ptr stmt_ref, uint32_t pos, uint32_t row, char* table);
static int db_compare_blob(FLAG* equal, char* var_name, statement_ptr stmt_db, statement_ptr stmt_ref, uint32_t pos, uint32_t row, char* table);
static int db_compare_device(FLAG* equal, sqlite3* db, sqlite3* ref);
static int db_compare_topic(FLAG* equal, sqlite3* db, sqlite3* ref);
static int db_compare_will(FLAG* equal, sqlite3* db, sqlite3* ref);
static int db_compare_topic_device(FLAG* equal, sqlite3* db, sqlite3* ref);
static int db_compare_connect(FLAG* equal, sqlite3* db, sqlite3* ref);
static int db_compare_message_device(FLAG* equal, sqlite3* db, sqlite3* ref);
static int db_compare_message(FLAG* equal, sqlite3* db, sqlite3* ref);

static int db_compare(FLAG* equal, char* db_s, char* ref_s){
	*equal = 1;
	sqlite3* db;
	sqlite3* ref;

	uint32_t sqlite3_error = sqlite3_open(db_s, &db);
	if( sqlite3_error ){
		printf("Could not open database: %s\n", db_s);
		return -1;
	}

	sqlite3_error = sqlite3_open(ref_s, &ref);
	if( sqlite3_error ){
		printf("Could not open reference database: %s\n", ref_s);
		return -1;
	}

	db_compare_device(equal, db, ref);
	db_compare_topic(equal, db, ref);
	db_compare_will(equal, db, ref);
	db_compare_topic_device(equal, db, ref);
	db_compare_connect(equal, db, ref);
	db_compare_message_device(equal, db, ref);
	db_compare_message(equal, db, ref);
	return 0;
}

static int db_compare_device(FLAG* equal, sqlite3* db, sqlite3* ref){
	char* sql_prepare = "SELECT device_id, client_id, valid FROM device";
	char* table = "DEVICE";
	statement_ptr stmt_db;
	statement_ptr stmt_ref;
	TRY(db_sql_prepare(db, &stmt_db, sql_prepare));
	TRY(db_sql_prepare(ref, &stmt_ref, sql_prepare));

	FLAG result_db = 0;
	FLAG result_ref = 0;
	
	TRY(db_sql_step(stmt_db, &result_db));
	TRY(db_sql_step(stmt_ref, &result_ref));
	
	uint32_t row = 0;
	while(result_db != 0 || result_ref != 0){
		row++;
		COMPARE_INT("device_id", 0);
		COMPARE_TEXT("client_id", 1);
		COMPARE_INT("valid", 2);
		TRY(db_sql_step(stmt_db, &result_db));
		TRY(db_sql_step(stmt_ref, &result_ref));
	};

	if(result_db == 0 && result_ref == 0){
		FINALIZE;
		return 0;
	} else {
		printf("The table DEVICE does not have the same number of rows, missing row: %d\n", row);
		FINALIZE;
		return -1;
	}
}

static int db_compare_topic(FLAG* equal, sqlite3* db, sqlite3* ref){
	char* sql_prepare = "SELECT topic_id, name, parent_topic, created_by, valid, predefined, predefined_topic_id FROM topic";
	char* table = "TOPIC";
	statement_ptr stmt_db;
	statement_ptr stmt_ref;
	TRY(db_sql_prepare(db, &stmt_db, sql_prepare));
	TRY(db_sql_prepare(ref, &stmt_ref, sql_prepare));
	
	FLAG result_db = 0;
	FLAG result_ref = 0;
	
	TRY(db_sql_step(stmt_db, &result_db));
	TRY(db_sql_step(stmt_ref, &result_ref));
	
	uint32_t row = 0;
	while(result_db != 0 || result_ref != 0){
		row++;
		COMPARE_INT("topic_id", 0);
		COMPARE_TEXT("name", 1);
		COMPARE_INT("parent_topic", 2);
		COMPARE_INT("created_by", 3);
		COMPARE_INT("valid", 4);
		COMPARE_INT("predefined", 5);
		COMPARE_INT("predefined_topic_id", 6);
		
		TRY(db_sql_step(stmt_db, &result_db));
		TRY(db_sql_step(stmt_ref, &result_ref));
	};

	if(result_db == 0 && result_ref == 0){
		FINALIZE;
		return 0;
	} else {
		printf("The table TOPIC does not have the same number of rows, missing row: %d\n", row);
		FINALIZE;
		return -1;
	}
}

static int db_compare_will(FLAG* equal, sqlite3* db, sqlite3* ref){
	char* sql_prepare = "SELECT will_id, device_id, topic_id, message_id, valid, qos, incompleet, retain FROM will";
	char* table = "WILL";
	statement_ptr stmt_db;
	statement_ptr stmt_ref;
	TRY(db_sql_prepare(db, &stmt_db, sql_prepare));
	TRY(db_sql_prepare(ref, &stmt_ref, sql_prepare));
		FLAG result_db = 0;
	FLAG result_ref = 0;
	
	TRY(db_sql_step(stmt_db, &result_db));
	TRY(db_sql_step(stmt_ref, &result_ref));
	
	uint32_t row = 0;
	while(result_db != 0 || result_ref != 0){
		row++;
		COMPARE_INT("will_id", 0);
		COMPARE_INT("device_id", 1);
		COMPARE_INT("topic_id", 2);
		COMPARE_INT("message_id", 3);
		COMPARE_INT("valid", 4);
		COMPARE_INT("qos", 5);
		COMPARE_INT("incompleet", 6);
		COMPARE_INT("retain", 7);

		TRY(db_sql_step(stmt_db, &result_db));
		TRY(db_sql_step(stmt_ref, &result_ref));
	};

	if(result_db == 0 && result_ref == 0){
		FINALIZE;
		return 0;
	} else {
		printf("The table WILL does not have the same number of rows, missing row: %d\n", row);
		FINALIZE;
		return -1;
	}
}

static int db_compare_topic_device(FLAG* equal, sqlite3* db, sqlite3* ref){
	char* sql_prepare = "SELECT topic_device_id, device_id, topic_id, short_topic_id, valid, qos, send_messages FROM topic_device";
	char* table = "TOPIC DEVICE";
	statement_ptr stmt_db;
	statement_ptr stmt_ref;
	TRY(db_sql_prepare(db, &stmt_db, sql_prepare));
	TRY(db_sql_prepare(ref, &stmt_ref, sql_prepare));

	FLAG result_db = 0;
	FLAG result_ref = 0;
	
	TRY(db_sql_step(stmt_db, &result_db));
	TRY(db_sql_step(stmt_ref, &result_ref));
	
	uint32_t row = 0;
	while(result_db != 0 || result_ref != 0){
		row++;
		COMPARE_INT("topic_device_id", 0);
		COMPARE_INT("device_id", 1);
		COMPARE_INT("topic_id", 2);
		COMPARE_INT("short_topic_id", 3);
		COMPARE_INT("valid", 4);
		COMPARE_INT("qos", 5);
		COMPARE_INT("send_messages", 6);
		TRY(db_sql_step(stmt_db, &result_db));
		TRY(db_sql_step(stmt_ref, &result_ref));
	};

	if(result_db == 0 && result_ref == 0){
		FINALIZE;
		return 0;
	} else {
		printf("The table TOPIC_DEVICE does not have the same number of rows, missing row: %d\n", row);
		FINALIZE;
		return -1;
	}
}

static int db_compare_connect(FLAG* equal, sqlite3* db, sqlite3* ref){
	char* sql_prepare = "SELECT connect_id, device_id, duration, valid, address, clean, awake FROM connect";
	char* table = "CONNECT";
	statement_ptr stmt_db;
	statement_ptr stmt_ref;
	TRY(db_sql_prepare(db, &stmt_db, sql_prepare));
	TRY(db_sql_prepare(ref, &stmt_ref, sql_prepare));

	FLAG result_db = 0;
	FLAG result_ref = 0;
	
	TRY(db_sql_step(stmt_db, &result_db));
	TRY(db_sql_step(stmt_ref, &result_ref));
	
	uint32_t row = 0;
	while(result_db != 0 || result_ref != 0){
		row++;
		COMPARE_INT("connect_id", 0);
		COMPARE_INT("device_id", 1);
		COMPARE_INT("duration", 2);
		COMPARE_INT("valid", 3);
		COMPARE_INT64("address", 4);
		COMPARE_INT("clean", 5);
		COMPARE_INT("awake", 6);
		
		TRY(db_sql_step(stmt_db, &result_db));
		TRY(db_sql_step(stmt_ref, &result_ref));
	};

	if(result_db == 0 && result_ref == 0){
		FINALIZE;
		return 0;
	} else {
		printf("The table CONNECT does not have the same number of rows, missing row: %d\n", row);
		FINALIZE;
		return -1;
	}
}

static int db_compare_message_device(FLAG* equal, sqlite3* db, sqlite3* ref){
	char* sql_prepare = "SELECT message_device_id, to_database, device_id, message_id, extern_message_id, qos, state, valid FROM message_device";
	char* table = "MESSAGE DEVICE";
	statement_ptr stmt_db;
	statement_ptr stmt_ref;
	TRY(db_sql_prepare(db, &stmt_db, sql_prepare));
	TRY(db_sql_prepare(ref, &stmt_ref, sql_prepare));
	
	FLAG result_db = 0;
	FLAG result_ref = 0;
	
	TRY(db_sql_step(stmt_db, &result_db));
	TRY(db_sql_step(stmt_ref, &result_ref));
	
	uint32_t row = 0;
	while(result_db != 0 || result_ref != 0){
		row++;
		COMPARE_INT("message_device_id", 0);
		COMPARE_INT("to_database", 1);
		COMPARE_INT("device_id", 2);
		COMPARE_INT("message_id", 3);
		COMPARE_INT("extern_message_id", 4);
		COMPARE_INT("qos", 5);
		COMPARE_INT("state", 6);
		COMPARE_INT("valid", 7);

		TRY(db_sql_step(stmt_db, &result_db));
		TRY(db_sql_step(stmt_ref, &result_ref));
	};

	if(result_db == 0 && result_ref == 0){
		FINALIZE;
		return 0;
	} else {
		printf("The table MESSAGE_DEVICE does not have the same number of rows, missing row: %d\n", row);
		FINALIZE;
		return -1;
	}
}

static int db_compare_message(FLAG* equal, sqlite3* db, sqlite3* ref){
	char* sql_prepare = "SELECT message_id, topic_id, added_by, data, retain, valid FROM message";
	char* table = "MESSAGE";
	statement_ptr stmt_db;
	statement_ptr stmt_ref;
	TRY(db_sql_prepare(db, &stmt_db, sql_prepare));
	TRY(db_sql_prepare(ref, &stmt_ref, sql_prepare));
		
	FLAG result_db = 0;
	FLAG result_ref = 0;
	
	TRY(db_sql_step(stmt_db, &result_db));
	TRY(db_sql_step(stmt_ref, &result_ref));
	
	uint32_t row = 0;
	while(result_db != 0 || result_ref != 0){
		row++;
		COMPARE_INT("message_id", 0);
		COMPARE_INT("topic_id", 1);
		COMPARE_INT("added_by", 2);
		COMPARE_BLOB("data", 3);
		COMPARE_INT("retain", 4);
		COMPARE_INT("valid", 5);

		TRY(db_sql_step(stmt_db, &result_db));
		TRY(db_sql_step(stmt_ref, &result_ref));
	};

	if(result_db == 0 && result_ref == 0){
		FINALIZE;
		return 0;
	} else {
		printf("The table MESSAGE does not have the same number of rows, missing row: %d\n", row);
		FINALIZE;
		return -1;
	}
}

static int db_compare_int(FLAG* equal, char* var_name, statement_ptr stmt_db, statement_ptr stmt_ref, uint32_t pos, uint32_t row, char* table){
	uint32_t int_1;
	uint32_t int_2;
	TRY(db_sql_get_int(stmt_db, pos, &int_1));	
	TRY(db_sql_get_int(stmt_ref, pos, &int_2));
	if(int_1 != int_2){
		printf("\nAt row %d from table %s, the %s does not match!\n", row, table, var_name);
		printf("The integer expected is %d but in the database this value is %d\n", int_1, int_2);
		*equal = 0;
		return -1;
	} else{
		return 0;
	}
}

static int db_compare_int64(FLAG* equal, char* var_name, statement_ptr stmt_db, statement_ptr stmt_ref, uint32_t pos, uint32_t row, char* table){
	uint64_t int_1;
	uint64_t int_2;
	TRY(db_sql_get_int64(stmt_db, pos, &int_1));	
	TRY(db_sql_get_int64(stmt_ref, pos, &int_2));
	if(int_1 != int_2){
		printf("\nAt row %d from table %s, the %s does not match!\n", row, table, var_name);
		printf("The integer expected is %ld but in the database this value is %ld\n", int_1, int_2);
		*equal = 0;
		return -1;
	} else{
		return 0;
	}
}

static int db_compare_text(FLAG* equal, char* var_name, statement_ptr stmt_db, statement_ptr stmt_ref, uint32_t pos, uint32_t row, char* table){
	char* string_1;
	char* string_2;
	TRY(db_sql_get_text(stmt_db, pos, &string_1));	
	TRY(db_sql_get_text(stmt_ref, pos, &string_2));
	if(string_1 == NULL || string_2 == NULL){
		if(string_1 != string_2){
			*equal = 0;
			return -1;
		} else {
			return 0;
		}
	} else {if(strcmp(string_1, string_2)){
			printf("\nAt row %d from table %s, the %s does not match!\n", row, table, var_name);
			printf("The string expected is: \n     %s\n but in the database this string is: \n     %s\n", string_1, string_2);
			*equal = 0;
			return -1;
		} else{
			return 0;
		}
	}
}

static int db_compare_blob(FLAG* equal, char* var_name, statement_ptr stmt_db, statement_ptr stmt_ref, uint32_t pos, uint32_t row, char* table){
	blob blob_1;
	blob blob_2;
	TRY(db_sql_get_blob(stmt_db, pos, &blob_1));	
	TRY(db_sql_get_blob(stmt_ref, pos, &blob_2));
	
	if(blob_1.size != blob_2.size){
		printf("At row %d from table %s, the %s does not match!\n", row, table, var_name);
		printf("The blob expected has size %d, the one received has size %d\n", blob_1.size, blob_2.size);
		*equal = 0;
		return -1;
	}

	if(strncmp((char*) blob_1.data, (char*) blob_2.data, blob_1.size)){
		printf("At row %d from table %s, the %s does not match!\n", row, table, var_name);
		printf("The data insite the blob did not match: (the first one is the expected one)\n");
		uint32_t index;
		for(index = 0; index <blob_1.size; index ++){
			printf("%X", *((char*) (blob_1.data + index)));
		}
		printf("\n");
		for(index = 0; index <blob_1.size; index ++){
			printf("%X", *((char*) (blob_1.data + index)));
		}
		printf("\n");

		*equal = 0;
		return -1;
	} else{
		return 0;
	}
}

void kill_test( int pid){
	sleep(3);
	printf("killing the database\n");
	kill(pid, SIGKILL);
}

/* try to open the database and kill it using the quit signal (no message to devices) */
/*-------------------------------------------------------------------------------*/
int db_test_1(char* output_file){
/*-------------------------------------------------------------------------------*/	
	int child_pid;
	int file_descriptor_write;
	int file_descriptor_read;
	
	database_open("./test_db/test_1.db", &child_pid, output_file, &file_descriptor_write, &file_descriptor_read);
	sleep(2);
	kill(child_pid, SIGQUIT);

	MESSAGE* m = (MESSAGE*) malloc(sizeof(MESSAGE));	
	/* termination message */
	db_create_message(&m, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL);
	TRY(db_send_message_pipe(m, file_descriptor_write));

	return 0;
}

/* try to open the database and kill it using the term signal (messages to devices) */
/*-------------------------------------------------------------------------------*/
int db_test_2(char* output_file){
/*-------------------------------------------------------------------------------*/	
	int child_pid;
	int file_descriptor_write;
	int file_descriptor_read;
	
	database_open("./test_db/test_2.db", &child_pid, output_file, &file_descriptor_write, &file_descriptor_read);
	sleep(2);
	kill(child_pid, SIGTERM);
	
	MESSAGE* m = (MESSAGE*) malloc(sizeof(MESSAGE));	
	/* termination message */
	db_create_message(&m, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL);
	TRY(db_send_message_pipe(m, file_descriptor_write));

	waitpid(child_pid, NULL, 0);

	return 0;
}

/* try to open the database and subscribe (no will message)*/
/*-------------------------------------------------------------------------------*/
int db_test_3(char* output_file){
/*-------------------------------------------------------------------------------*/	
	int child_pid;
	int file_descriptor_write;
	int file_descriptor_read;
	char* database = "./test_db/test_3.db";
	
	MESSAGE* m;
	blob* b;
	db_create_blob(&b, "sensor_1");
	db_create_message(&m, 123456789, 0, 0, 0, CONNECT, 0, 0, 60, 0, 0, 0, 1, 0, 0, b);

	remove(database);
	database_open( database, &child_pid, output_file, &file_descriptor_write, &file_descriptor_read);
	TRY(db_send_message_pipe(m, file_descriptor_write));

	sleep(7);
	kill(child_pid, SIGTERM);
	
	kill_test(child_pid);

	waitpid(child_pid, NULL, 0);

	FLAG equal = 0;

	db_compare(&equal, "test_db/test_3.db", "test_db_ref/test_3.db");
	if(equal){
		printf("the database where equal\n");
		return 0;
	} else {	
		printf("the database where not equal\n");
		return -1;
	}
}

/* for reference:
int db_create_message(
MESSAGE** mes, 
ADDRESS address, 
DATABASE_ID message_id, 
EXTERN_ID extern_message_id, 
EXTERN_ID short_topic_id, 
MESSAGE_TYPE type, T
OPIC_TYPE topic_type, 
QOS qos, 
time_t duration, 
FLAG timeout, 
FLAG duplicate, 
FLAG will, 
FLAG clean, 
FLAG retain, 
return_code return_value, 
blob* b);


int db_create_message(MESSAGE** mes, ADDRESS address, DATABASE_ID message_id, EXTERN_ID extern_message_id, EXTERN_ID short_topic_id, MESSAGE_TYPE type, TOPIC_TYPE topic_type, QOS qos, time_t duration, FLAG timeout, FLAG duplicate, FLAG will, FLAG clean, FLAG retain, return_code return_value, blob* b);
*/


/* try to open the database and connect and disconnect without sleep (no will message)*/
/*-------------------------------------------------------------------------------*/
int db_test_4(char* output_file){
/*-------------------------------------------------------------------------------*/	
	int child_pid;
	int file_descriptor_write;
	int file_descriptor_read;
	char* database = "./test_db/test_4.db";
	
	MESSAGE* m;
	blob* b;
	db_create_blob(&b, "sensor_1");

	remove(database);
	database_open( database, &child_pid, output_file, &file_descriptor_write, &file_descriptor_read);

	db_create_message(&m, 123456789, 0, 0, 0, CONNECT, 0, 0, 60, 0, 0, 0, 1, 0, 0, b);
	TRY(db_send_message_pipe(m, file_descriptor_write));
	
	db_create_message(&m, 123456789, 0, 0, 0, DISCONNECT, 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL);
	TRY(db_send_message_pipe(m, file_descriptor_write));

	sleep(7);
	kill(child_pid, SIGTERM);
	
	kill_test(child_pid);

	waitpid(child_pid, NULL, 0);

	FLAG equal = 0;

	db_compare(&equal, "test_db/test_4.db", "test_db_ref/test_4.db");
	if(equal){
		printf("the database where equal\n");
		return 0;
	} else {	
		printf("the database where not equal\n");
		return -1;
	}
}

/* try to open the database and connect and disconnect without sleep (no will message)*/
/*-------------------------------------------------------------------------------*/
int db_test_5(char* output_file){
/*-------------------------------------------------------------------------------*/	
	int child_pid;
	int file_descriptor_write;
	int file_descriptor_read;
	char* database = "./test_db/test_5.db";
	
	MESSAGE* m;
	blob* b;

	remove(database);
	database_open( database, &child_pid, output_file, &file_descriptor_write, &file_descriptor_read);

	db_create_blob(&b, "sensor_1");
	db_create_message(&m, 123456789, 0, 0, 0, CONNECT, 0, 0, 60, 0, 0, 0, 1, 0, 0, b);
	TRY(db_send_message_pipe(m, file_descriptor_write));


	blob* topic;

	db_create_blob(&topic, "sensor_1/temp");
	TRY(db_create_message(&m, 123456789, 0, 1, 0, REGISTER, 0, 0, 0, 0, 0, 0, 0, 0, 0, topic));
	TRY(db_send_message_pipe(m, file_descriptor_write));
	
	db_create_message(&m, 123456789, 0, 0, 0, DISCONNECT, 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL);
	TRY(db_send_message_pipe(m, file_descriptor_write));

	sleep(7);
	kill(child_pid, SIGTERM);
	
	kill_test(child_pid);

	waitpid(child_pid, NULL, 0);

	FLAG equal = 0;

	db_compare(&equal, "test_db/test_5.db", "test_db_ref/test_5.db");
	if(equal){
		printf("the database where equal\n");
		return 0;
	} else {	
		printf("the database where not equal\n");
		return -1;
	}	
}

/* try to open the database and connect and disconnect without sleep (no will message)*/
/*-------------------------------------------------------------------------------*/
int db_test_6(char* output_file){
/*-------------------------------------------------------------------------------*/	
	int child_pid;
	int file_descriptor_write;
	int file_descriptor_read;
	char* database = "./test_db/test_6.db";
	
	MESSAGE* m;
	blob* b;

	remove(database);
	database_open( database, &child_pid, output_file, &file_descriptor_write, &file_descriptor_read);

	db_create_blob(&b, "sensor_1");
	db_create_message(&m, 123456789, 0, 0, 0, CONNECT, 0, 0, 60, 0, 0, 0, 1, 0, 0, b);
	TRY(db_send_message_pipe(m, file_descriptor_write));


	blob* topic;

	db_create_blob(&topic, "sensor_1/temp");
	TRY(db_create_message(&m, 123456789, 0, 1, 0, REGISTER, 0, 0, 0, 0, 0, 0, 0, 0, 0, topic));
	TRY(db_send_message_pipe(m, file_descriptor_write));
	
	blob* temp;

	db_create_blob(&temp, "15,5");
	TRY(db_create_message(&m, 123456789, 0, 101, 1, PUBLISH, 2, 0, 0, 0, 0, 0, 0, 0, 0, temp));
	TRY(db_send_message_pipe(m, file_descriptor_write));
	
	TRY(db_create_message(&m, 123456789, 0, 0, 0, DISCONNECT, 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL));
	TRY(db_send_message_pipe(m, file_descriptor_write));

	sleep(10);
	kill(child_pid, SIGTERM);
	
	kill_test(child_pid);

	waitpid(child_pid, NULL, 0);

	FLAG equal = 0;

	db_compare(&equal, "test_db/test_6.db", "test_db_ref/test_6.db");
	if(equal){
		printf("the database where equal\n");
		return 0;
	} else {	
		printf("the database where not equal\n");
		return -1;
	}
}

/* subscribe multiple devices, publish to a topic where the other one is subscribed to */
/*-------------------------------------------------------------------------------*/
int db_test_7(char* output_file){
/*-------------------------------------------------------------------------------*/	
	int child_pid;
	int file_descriptor_write;
	int file_descriptor_read;
	char* database = "./test_db/test_7.db";
	
	MESSAGE* m;
	blob* b;
	
	sleep(7);

	remove(database);
	database_open( database, &child_pid, output_file, &file_descriptor_write, &file_descriptor_read);

	pthread_t read_thread;
	int* file_read = (int*) malloc(sizeof(int));
	*file_read = file_descriptor_read;
	pthread_create(&read_thread, NULL, &test_7_output, (void*) file_read);

	db_create_blob(&b, "sensor_1");
	db_create_message(&m, 123456789, 0, 0, 0, CONNECT, 0, 0, 60, 0, 0, 0, 1, 0, 0, b);
	TRY(db_send_message_pipe(m, file_descriptor_write));

	db_create_blob(&b, "sensor_2");
	db_create_message(&m, 987654321, 0, 0, 0, CONNECT, 0, 0, 60, 0, 0, 0, 1, 0, 0, b);
	TRY(db_send_message_pipe(m, file_descriptor_write));


	blob* topic;

	db_create_blob(&topic, "sensor_1/temp");
	TRY(db_create_message(&m, 123456789, 0, 1, 0, REGISTER, 0, 0, 0, 0, 0, 0, 0, 0, 0, topic));
	TRY(db_send_message_pipe(m, file_descriptor_write));
	
	db_create_blob(&topic, "sensor_1/temp");
	TRY(db_create_message(&m, 987654321, 0, 1, 0, SUBSCRIBE, 0, 0, 0, 0, 0, 0, 0, 0, 0, topic));
	TRY(db_send_message_pipe(m, file_descriptor_write));
	
	blob* temp;

	db_create_blob(&temp, "15,5");
	TRY(db_create_message(&m, 123456789, 0, 101, 1, PUBLISH, 2, 0, 0, 0, 0, 0, 0, 0, 0, temp));
	TRY(db_send_message_pipe(m, file_descriptor_write));
	
	TRY(db_create_message(&m, 123456789, 0, 0, 0, DISCONNECT, 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL));
	TRY(db_send_message_pipe(m, file_descriptor_write));
	
	TRY(db_create_message(&m, 987654321, 0, 0, 0, DISCONNECT, 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL));
	TRY(db_send_message_pipe(m, file_descriptor_write));


	sleep(7);
	
	kill(child_pid, SIGTERM);
	database_request_message();
	database_request_message();
	database_request_message();
	database_request_message();
	database_request_message();
	database_request_message();
	database_request_message();
	database_request_message();

	kill_test(child_pid);
	
	waitpid(child_pid, NULL, 0);

	FLAG equal = 0;

	db_compare(&equal, "test_db/test_7.db", "test_db_ref/test_7.db");
	if(equal){
		printf("the database where equal\n");
		return 0;
	} else {	
		printf("the database where not equal\n");
		return -1;
	}
}

void* test_7_output(void * file){
	int fd_in = *((int*) file);
	MESSAGE* m;
	printf("opened the pipe, fd_in = %d\n", fd_in);
	while(1){
	db_receive_message_pipe(&m, fd_in);
	printf("!");
	if(m->address != 0)
	printf("the address is: %ld, the type is: %d\n", m->address, m->type);
	free(m);
	}
}
