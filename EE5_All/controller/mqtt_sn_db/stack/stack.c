/*******************************************************************************
*  FILENAME: stack.c
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
#include <assert.h>
#include <stdlib.h>
#include "stack.h"

/*------------------------------------------------------------------------------
		definitions (defines, typedefs, ...)
------------------------------------------------------------------------------*/

struct Stack_element {
	element data;
	struct Stack_element* next;
	};

typedef struct Stack_element stack_element;

struct The_stack {
	stack_element* top; // the top of the stack
	destroy_element_function *destroy_element; // callback function
	copy_element_function *copy_element;		// callback function
	};

typedef struct The_stack the_stack;

/*------------------------------------------------------------------------------
		prototypes
------------------------------------------------------------------------------*/

static stack_error next_top_element( const stack s );

/*------------------------------------------------------------------------------
		implementation code
------------------------------------------------------------------------------*/

//----------------------------------------------------------------------------//
stack_error stack_init( stack *s, destroy_element_function *destroy_element, copy_element_function *copy_element )
//----------------------------------------------------------------------------//
{
	if(s == NULL || destroy_element == NULL || copy_element == NULL)
		return STACK_INVALID_INPUT;
    the_stack *t;

	t = (the_stack *) malloc ( sizeof(the_stack) );
	if (t == NULL )
    {
        *s = NULL;
        return STACK_MEMORY_ERROR;
    }

	t->top = NULL;	/* indicates empty stack */
	
	t->destroy_element = destroy_element;
	t->copy_element = copy_element;

	*s = (stack)t;
	return STACK_OK;
}

//----------------------------------------------------------------------------//
stack_error destroy( stack *s )
//----------------------------------------------------------------------------//
{
	if(s==NULL)
		return STACK_INVALID_INPUT;
	the_stack *t = (the_stack *)*s;

	if(t->top != NULL){

		do{
			// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
			next_top_element( *s );
			// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	
		}	while ( t->top != NULL ); 
	}

	free(*s);
	*s = NULL;
	return STACK_OK;
}


//----------------------------------------------------------------------------//
stack_error push( stack s, const element element )
//----------------------------------------------------------------------------//
{
	if(s==NULL || element ==NULL)
		return STACK_INVALID_INPUT;
    the_stack *t = (the_stack *)s;

	stack_element *new_element = (stack_element*) malloc(sizeof(stack_element));
	// creates the new stack element
	new_element->next = t->top; // put the top element after this one
	t->top = new_element; // put this element as the top one

	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	t->copy_element( element, &(t->top->data)); // CALL CALLBACK FUNC TO MAKE A DEEP COPY 
	
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

	return STACK_OK;
}


//----------------------------------------------------------------------------//
stack_error pop( stack s )
//----------------------------------------------------------------------------//
{
	if(s==NULL)
		return STACK_INVALID_INPUT;
    the_stack *t = (the_stack *)s;

	if(t->top == NULL)
		return STACK_STACK_EMPTY;

	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	t->destroy_element( 
	t->top->data );// CALL CALLBACK FUN TO CLEANUP THE MEMORY ASSIGNED TO THE ELEMENT
	next_top_element( s );
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

	return STACK_OK;
}

//----------------------------------------------------------------------------//
stack_error top( const stack s, element * element )
//----------------------------------------------------------------------------//
{
	if(s==NULL || element ==NULL)
		return STACK_INVALID_INPUT;
    the_stack *t = (the_stack *)s;

	if ( t->top == NULL )
		return STACK_STACK_EMPTY;

	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!	///	
	*element = t->top->data; 
	stack_element* old = t->top;
	t->top = t->top->next;
	free(old);
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!		
		
	return STACK_OK;
}

//----------------------------------------------------------------------------//
stack_error peek( const stack s, element * stack_element )
//----------------------------------------------------------------------------//
{
	if(s==NULL || stack_element ==NULL)
		return STACK_INVALID_INPUT;

    the_stack *t = (the_stack *)s;

	if ( t->top == NULL )
		return STACK_STACK_EMPTY;

	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!	///	
	t->copy_element( t->top->data, stack_element ); // RETURN DEEP COPY
	
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!		
		
	return STACK_OK;
}

//----------------------------------------------------------------------------//
unsigned int stack_size( const stack s )
//----------------------------------------------------------------------------//
{
	if(s == NULL)
		return 0;
	the_stack *t = (the_stack *)s;
	unsigned int stack_size = 0;
	
	if(t->top == NULL)
		return stack_size;
	
	stack_element* current = t->top;
	stack_size ++;

	while( current->next != NULL){
		current = current->next;
		stack_size++;
	}
	
	return stack_size;
}

//----------------------------------------------------------------------------//
static stack_error next_top_element( const stack s )
//----------------------------------------------------------------------------//
{
	if(s==NULL)
		return STACK_INVALID_INPUT;

	the_stack *t = (the_stack *)s;
	
	stack_element* old = t->top;
	t->top = t->top->next;
	if(old->data != NULL)
		(t->destroy_element)(old->data);
	free( old );
	return STACK_OK;
}

//----------------------------------------------------------------------------//
char* stack_get_error_message( const stack_error error )
//----------------------------------------------------------------------------//
{
	switch(error){
		case STACK_OK:
			return "No error was detected";
		case STACK_STACK_EMPTY:
			return "Tried to get an element from an empty stack";
		case STACK_MEMORY_ERROR:
			return "When trying to allocate memory, an error occured";
		case STACK_INIT_ERROR:
			return "Unable to initialize the stack";
		case STACK_UNDEFINED_ERROR:
		default:
			return "The error that occured is not further specified, \n   please check your error handling.";
	}
}
