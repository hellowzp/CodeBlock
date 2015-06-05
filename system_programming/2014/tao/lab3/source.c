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
	int top;
	};

typedef struct queuen Queuen;

/*------------------------------------------------------------------------------
		function prototypes
------------------------------------------------------------------------------*/

ErrCode Init( Stack ** ); /* cf. constructor: initialise the new stack */
ErrCode Destroy( Stack ** ); /* cf. destructor: destroy the stack: free memory, etc. */
ErrCode Push( Stack *, const Element ); /* push element on the stack */
ErrCode Pop( Stack * ); /* delete element from stack  */
ErrCode Top( const Stack *, Element * ); /* returns top of stack */
unsigned int StackSize( const Stack * ); /* return number of elements on the stack */

#endif /*QUEUEN_H_*/
