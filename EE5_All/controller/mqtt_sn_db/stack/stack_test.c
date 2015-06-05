/*******************************************************************************
*  FILENAME: main.c
*
* Version
* An implementation of a stack datastructure;
* Generic data types: type 'void *';
* Generic element type: type 'void *'
*
*******************************************************************************/

/*-----------------------------------------------------------------------------
		include files
------------------------------------------------------------------------------*/
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include "stack.h"
#include <time.h>

/*------------------------------------------------------------------------------
		definitions (defines, typedefs, ...)
------------------------------------------------------------------------------*/

typedef struct Test_results{
	uint32_t succes;
	uint32_t fail;
} test_results;

/*********************************************************************************
**		Macro's
**********************************************************************************/


#ifdef DEBUG
	#define STACK_PRINT_ERROR(error_number)																\
		do{																											\
			printf("\nERROR line %d of file %s by function %s:\n   Error %d: %s\n",				\
			__LINE__, __FILE__,__func__,error_number, stack_get_error_message(error_number));\
			}																											\
			while(0)
#else
	#define STACK_PRINT_ERROR(s)																				\
		do{} while(0)
#endif

#ifdef DEBUG
	#define STACK_PRINT_ERROR_STRING(error_string)														\
		do(																											\
			printf("\nERROR line %d of file %s by function %s:\n   Error : %s\n",				\
			__LINE__, __FILE__,__func__, error_string)													\
			);																											\
		while(0)
#else
	#define STACK_PRINT_ERROR_STRING(s)																		\
		do{};																											\
		while(0)
#endif

#define STACK_TRY(stack_function)																			\
    do{ uint32_t stack_error = 0;																			\
         stack_error = stack_function;																		\
        if(stack_error){																						\
  			  STACK_PRINT_ERROR(stack_error);																\
  			  return stack_error; 																				\
  		  }																											\
    }																													\
    while(0)

#define STACK_TRY_ERROR(stack_function, expected_error)												\
    do{ uint32_t stack_error = 0;																			\
         stack_error = stack_function;																		\
        if(stack_error != expected_error && stack_error != 0){										\
  			  STACK_PRINT_ERROR(stack_error);																\
  			  return stack_error; 																				\
  		  }																											\
		  if(stack_error == 0){																					\
				return STACK_UNDEFINED_ERROR;																	\
			}																											\
    }																													\
    while(0)


#define STACK_TRY_TEST(function, description)															\
	do{	struct timespec start, stop, elapse;															\
			uint32_t error = 0;																					\
			time_t now = time(NULL);																			\
			printf("\n--------------------------Running test--------------------------\n%s\n"\
			, description);																						\
			clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start);											\
			error = function;																						\
			clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &stop);												\
			printf("\n   Test started at: %s", ctime(&now));											\
			elapse.tv_sec = stop.tv_sec-start.tv_sec;														\
			if((stop.tv_nsec - start.tv_nsec) < 0){														\
				elapse.tv_sec--;																					\
				elapse.tv_nsec = 1000000000+(stop.tv_nsec - start.tv_nsec); 						\
			} else {																									\
				elapse.tv_nsec = (stop.tv_nsec - start.tv_nsec);										\
			}																											\
			long miliseconds = elapse.tv_nsec/1000000;													\
			elapse.tv_nsec -= miliseconds*1000000;															\
			long microseconds = elapse.tv_nsec/1000;														\
			elapse.tv_nsec -= microseconds*1000;															\
			long hours = elapse.tv_sec / 3600;																\
			elapse.tv_sec -= hours*3600;																		\
			long minutes = elapse.tv_sec / 60;																\
			elapse.tv_sec -= minutes*60;																		\
			printf("   The test ran for: %ldh %ldm %lds - %ldms %ldµs %ldns\n",					\
			hours, minutes, elapse.tv_sec, miliseconds, microseconds, elapse.tv_nsec);			\
			if(error == 0){																						\
				printf("   The test ran succesfull and returned without an error!\n");			\
				result.succes++;																					\
			} else {																									\
				printf("   Sorry, the test failed and returned the following error code:\n");	\
				result.fail++;																						\
				STACK_PRINT_ERROR(error);																		\
			}																											\
			printf("------------------------Test compleeted-------------------------\n");		\
	} while(0)

#define PRINT_TEST_HEADER()																					\
	do{																												\
		printf("\n\n\n+---------------------------------------------------------------+\n");\
		printf(      "|                    Testing Stack implementation               |\n");\
		printf(      "+---------------------------------------------------------------+\n");\
	} while (0)

#define PRINT_TEST_FOOTER()																					\
	do{																												\
		printf(    "\n+---------------------------------------------------------------+\n");\
		printf(      "|                    Number of test run: %3d                    |\n",	\
		result.fail + result.succes);\
		printf(      "|              Number of test succeeded: %3d                    |\n",	\
		result.succes);\
		printf(      "|                 Number of test failed: %3d                    |\n",	\
		result.fail);\
		printf(      "+---------------------------------------------------------------+\n");\
	} while (0)




/*------------------------------------------------------------------------------
		function prototypes
------------------------------------------------------------------------------*/

void destroy_element(element); 
void copy_element(element, element*);
int create_element(element* e, long int value);

int init_test();
int null_init_test();
int push_and_destroy_test();
int push_and_top_test();
int push_null_test();
int stack_and_restack_test();

/*------------------------------------------------------------------------------
		global variable declarations
------------------------------------------------------------------------------*/


/*------------------------------------------------------------------------------
		implementation code
------------------------------------------------------------------------------*/

int main( void )
{
	test_results result;
	result.fail = 0;
	result.succes = 0;
	PRINT_TEST_HEADER();

	STACK_TRY_TEST(init_test(), 
"Testing the initialisation and destruction of the stack");

	STACK_TRY_TEST(null_init_test(),
"Testing to call the init and destroy functions with invalid input");

	STACK_TRY_TEST(push_and_destroy_test(), 
"Testing if you can insert elements and destroy the stack \n\
without memory leaks. Use valgrind to verify.");

	STACK_TRY_TEST(push_and_top_test(),
"Testing if you can put elements on the stack and can get \n\
them back immediatly.");

	STACK_TRY_TEST(push_null_test(),
"Test invalid input for push and top");

	STACK_TRY_TEST(stack_and_restack_test(),
"Testing if you can create 2 stacks, fill one, fill the second\n one with the first one and do this again. check the output");
	PRINT_TEST_FOOTER();
	return 0;
}

void destroy_element(element data)  {
	free( data );
}


void copy_element(element data, element* copy) {
	*copy = malloc( sizeof(long int) );
	*((long int *)(*copy)) = *(long int*)data;
	return;
}

int init_test(){
	stack s;
	
	//try to initialyze the stack
	STACK_TRY(stack_init(&s, &destroy_element, &copy_element));

	//try to destroy the stack
	STACK_TRY(destroy( &s ));

	return STACK_OK;
}

int null_init_test(){
	stack s;
	
	//try to initialyze the stack
	STACK_TRY_ERROR(stack_init(&s, NULL, &copy_element), STACK_INVALID_INPUT);

	//try to initialyze the stack
	STACK_TRY_ERROR(stack_init(&s, &destroy_element, NULL), STACK_INVALID_INPUT);
	
	//try to initialyze the stack
	STACK_TRY_ERROR(stack_init(NULL, &destroy_element, &copy_element), STACK_INVALID_INPUT);


	//try to destroy the stack
	STACK_TRY_ERROR(destroy(NULL), STACK_INVALID_INPUT);

	return STACK_OK;
}

int push_and_destroy_test(){
	stack s;
	
	//try to initialyze the stack
	STACK_TRY(stack_init(&s, &destroy_element, &copy_element));

	// the element that needs to be inserted
	element tmp;

	// insert 1000 new elements
	uint32_t i = 0;
	for(i = 0; i<1000; i++){
		create_element(&tmp, i);
		STACK_TRY(push(s, tmp));
		free(tmp);
	}

	//try to destroy the stack
	STACK_TRY(destroy( &s ));

	return STACK_OK;
}

int push_and_top_test(){
	stack s;
	
	//try to initialyze the stack
	STACK_TRY(stack_init(&s, &destroy_element, &copy_element));

	// the element that needs to be inserted
	element tmp;

	// insert 1000 new elements
	uint32_t i = 0;
	for(i = 0; i<1000; i++){
		create_element(&tmp, i);
		STACK_TRY(push(s, tmp));
		free(tmp);
		STACK_TRY(top(s, &tmp));
		if(*((long int*) tmp) != i)
			return STACK_UNDEFINED_ERROR;
		free(tmp);
	}

	//try to destroy the stack
	STACK_TRY(destroy( &s ));

	return STACK_OK;
}

int push_null_test(){
	stack s;
	
	//try to initialyze the stack
	STACK_TRY(stack_init(&s, &destroy_element, &copy_element));

	//try push and top with invalid input
	element tmp;
	STACK_TRY_ERROR(push(s, NULL), STACK_INVALID_INPUT);
	STACK_TRY_ERROR(push(NULL, tmp), STACK_INVALID_INPUT);
	STACK_TRY_ERROR(top(s, NULL), STACK_INVALID_INPUT);
	STACK_TRY_ERROR(top(NULL, &tmp), STACK_INVALID_INPUT);

	//try to destroy the stack
	STACK_TRY(destroy( &s ));

	return STACK_OK;

}

int create_element(element* e, long int value){
	*e =  malloc(sizeof(long int));
	if( *e == NULL){
		printf("Test failed, could not allocate memory.\n");
		return STACK_MEMORY_ERROR;
	}
	*((long int*)(*e)) = value;
	return STACK_OK;
}

int stack_and_restack_test(){
	stack s;
	stack s2;
	
	//try to initialyze the stack
	STACK_TRY(stack_init(&s, &destroy_element, &copy_element));
	STACK_TRY(stack_init(&s2, &destroy_element, &copy_element));

	// the element that needs to be inserted
	element tmp;

	// insert 1000 new elements
	uint32_t i = 0;
	for(i = 0; i<100; i++){
		create_element(&tmp, i);
		STACK_TRY(push(s, tmp));
		free(tmp);
	}
	
	for(i = 0; i<100; i++){
		STACK_TRY(top(s, &tmp));
		STACK_TRY(push(s2, tmp));
		free(tmp);
	}
	
	for(i = 0; i<100; i++){
		STACK_TRY(top(s2, &tmp));
		STACK_TRY(push(s, tmp));
		free(tmp);
	}
	
	for(i = 0; i<100; i++){
		STACK_TRY(top(s, &tmp));
		STACK_TRY(push(s2, tmp));
		free(tmp);
	}

	
	for(i = 0; i<100; i++){
		STACK_TRY(top(s2, &tmp));
		if(*((long int*) tmp) != i)
			return STACK_UNDEFINED_ERROR;
		free(tmp);
	}

	//try to destroy the stack
	STACK_TRY(destroy( &s ));
	STACK_TRY(destroy( &s2 ));

	return STACK_OK;
}
