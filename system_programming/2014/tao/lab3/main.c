#include <stdio.h>
#include <assert.h>

#include "queuen.h"


/*------------------------------------------------------------------------------
		definitions (defines, typedefs, ...)
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
		function prototypes
------------------------------------------------------------------------------*/

void ErrHandler( const ErrCode err );

/*------------------------------------------------------------------------------
		global variable declarations
------------------------------------------------------------------------------*/


/*------------------------------------------------------------------------------
		implementation code
------------------------------------------------------------------------------*/

//----------------------------------------------------------------------------//
int main( void )
//----------------------------------------------------------------------------//
{

    Stack *s; // 2nd stack: Stack stack2; etc.
	Element value;
	ErrCode err;

	//initialize the stack before using it
	Init(&s);

	// read values from the command prompt and push them on the stack
	do
    {
		printf( "Give a value to push on the stack (negative value to quit): " );
		scanf( "%ld", &value );
		if ( value >= 0 )
		{
			err = Push( s, value );
			ErrHandler( err );
		}
	} while ( value >= 0 );

#ifdef DEBUG
	printf( "\nThe stack size is %d\n", StackSize( s ) );
#endif

	printf( "\nThe stack values are:\n" );
	while ( StackSize( s ) > 0)
	{
		err = Top( s, &value);
		ErrHandler( err );
		printf( "%ld\n", value );
		ErrHandler( Pop( s ) );
	}

	//destroy the stack
	Destroy( &s );

	return 0;
}


//----------------------------------------------------------------------------//
void ErrHandler( const ErrCode err )
//----------------------------------------------------------------------------//
{
	switch( err ) {
		case ERR_NONE:
			break;
		case  ERR_EMPTY:
			printf( "\nCan't execute this operation while the stack is empty.\n" );
			break;
		case ERR_FULL:
			printf( "\nCan't execute this operation while the stack is full.\n" );
			break;
		case ERR_MEM:
			printf( "\nMemory problem occured while executing this operation on the stack.\n" );
			break;
		case ERR_INIT:
			printf( "\nStack initialization problem.\n" );
			break;
		case ERR_UNDEFINED:
			printf( "\nUndefined problem occured while executing this operation on the stack.\n" );
			break;
		default: // should never come here
			assert( 1==0 );
	}
}
