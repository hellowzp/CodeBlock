#ifndef PARSER_H_
#define PARSER_H_

/*********************************************************************************
**		dependencies
**********************************************************************************/
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../stack/stack.h"

/*********************************************************************************
**		Enums
**********************************************************************************/

enum Parser_error_enum{ PARSER_OK = 0, PARSER_NO_INPUT, PARSER_ILLEGAL_INPUT, PARSER_UNSPECIFIED_ERROR};

/*********************************************************************************
**		Defines
**********************************************************************************/

#ifndef MULTIPLE_WILDCARD
#define MULTIPLE_WILDCARD '*'
#endif

#ifndef SINGLE_WILDCARD
#define SINGLE_WILDCARD '?'
#endif


/*********************************************************************************
**		Typedefs
**********************************************************************************/

typedef void* parser;

/*********************************************************************************
**		Prototypes
**********************************************************************************/

extern uint32_t parser_string_to_pad(uint32_t length, char *input, parser* pad, uint32_t* number_of_wildcards);
extern uint32_t parser_get_top(parser p, char** return_string);
extern uint32_t parser_number_of_elements(parser p, uint32_t* number_of_elements);
extern uint32_t parser_destroy(parser* p);

#endif /*PARSER_H_*/
