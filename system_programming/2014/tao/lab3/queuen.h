#ifndef QUEUEN_H_
#define QUEUEN_H_

/*------------------------------------------------------------------------------
		definitions (defines, typedefs, ...)
------------------------------------------------------------------------------*/

#define MAX_STACK_SIZE	10

/*-- stack error codes --*/
enum errCode { ERR_NONE = 0, ERR_EMPTY, ERR_FULL, ERR_MEM, ERR_INIT, ERR_UNDEFINED };

typedef enum errCode ErrCode;

typedef long int Element;

struct queuen {
	Element data[MAX_QUEUEN_SIZE];
	int rear;
	};

typedef struct queuen Queuen;

/*------------------------------------------------------------------------------
		function prototypes
------------------------------------------------------------------------------*/

ErrCode Init( Queuen ** ); /* cf. constructor: initialise the new stack */
ErrCode Destroy( Queuen ** ); /* cf. destructor: destroy the stack: free memory, etc. */
ErrCode Push( Queuen *, const Element ); /* push element on the stack */
ErrCode Pop( Queuen * ); /* delete element from stack  */
ErrCode Rear( const Queuen *, Element * ); /* returns top of stack */
unsigned int StackSize( const Queuen * ); /* return number of elements on the stack */

#endif /*QUEUEN_H_*/
