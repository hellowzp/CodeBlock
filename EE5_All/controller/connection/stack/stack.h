#ifndef STACK_H_
#define STACK_H_

/*------------------------------------------------------------------------------
		definitions (defines, typedefs, ...)
------------------------------------------------------------------------------*/

/*-- stack stack_error codes --*/
enum Stack_error { STACK_OK = 0, STACK_STACK_EMPTY, STACK_MEMORY_ERROR, STACK_INIT_ERROR , STACK_INVALID_INPUT, STACK_UNDEFINED_ERROR };

typedef enum Stack_error stack_error;

typedef void * stack;

typedef void * element;

// define CALLBACK functions
typedef void destroy_element_function(element);
typedef void copy_element_function(element data, element* copy);

/*------------------------------------------------------------------------------
		function prototypes
------------------------------------------------------------------------------*/

extern stack_error stack_init( stack *, destroy_element_function *, copy_element_function * ); /* cf. constructor: initialise the new stack */
extern stack_error destroy( stack * ); /* cf. destructor: destroy the stack: free memory, etc. */
extern stack_error push( stack , const element ); /* push element on the stack */
extern stack_error pop( stack ); /* delete element from stack  */
extern stack_error top( const stack, element * ); /* returns top of stack and removes it from the stack*/
extern stack_error peek( const stack, element *); /* returns top element of stack without removing it */
extern unsigned int stack_size( const stack ); /* return number of elements on the stack */
extern char* stack_get_error_message( const stack_error error ); /* return the error message for an error code */

#endif /*STACK_H_*/
