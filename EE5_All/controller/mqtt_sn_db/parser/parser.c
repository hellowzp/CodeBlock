#include "parser.h"

uint32_t parser_error_var = 0;

#define TRY(parser_function)																		\
    do{																									\
        parser_error_var = parser_function;														\
        if(parser_error_var){																			\
			  PRINT_ERROR("Error while using the parser");									\
			  return parser_error_var; 																	\
		  }																								\
    }																										\
    while(0)

#ifdef DEBUG
	#define PRINT_ERROR(error_message)															\
		do{																								\
			printf("ERROR line %d of file %s by function %s:\n\t %s\n", 				\
			__LINE__, __FILE__,__func__,error_message);										\
			}																								\
		while(0)
#else
	#define PRINT_ERROR(s)																			\
		do{}																								\
		while(0)
#endif

/*********************************************************************************
**		Prototypes
**********************************************************************************/

static void parser_destroy_element(element data);
static void parser_copy_element(element data, element* copy);
static uint32_t parser_get_fragment_length(uint32_t* length_remaining, char **input_ptr, uint32_t* fragment_length);

/*********************************************************************************
**		Implementations
**********************************************************************************/

static void parser_destroy_element(element data){
	free( data );
}

static void parser_copy_element(element data, element* copy){
	if(data == NULL)
		return ;

	char* string = (char*) data;

	size_t size = strlen(string) + 1;

	*copy = (element) malloc(size);
	char* string_copy = (char*) *copy;

	uint32_t i = 0;

	for( i=0; i<size; i++){
		*string_copy = *string;
		string_copy++;
		string++;
	} 
}

uint32_t parser_string_to_pad(uint32_t length, char *input, parser* pad, uint32_t* number_of_wildcards){
	stack s;

	/* initializing the stack */
	stack_init(pad, &parser_destroy_element, &parser_copy_element);
	stack_init(&s, &parser_destroy_element, &parser_copy_element);

	/* checking the input */
	if(length == 0)
		return PARSER_NO_INPUT;

	if(number_of_wildcards == NULL)
		return PARSER_NO_INPUT;
	
	/* counting the number of wildcards */
	*number_of_wildcards = 0;
	uint32_t i = 0;
	for(i = 0; i<length; i++){
		if(*(input + i) == SINGLE_WILDCARD || *(input + i) == MULTIPLE_WILDCARD)
			(*number_of_wildcards)++;
	}
	
	/* starting parsing */
	uint32_t length_remaining = length;

	while(length_remaining>0){
		/* get the length of the next fragment */
		uint32_t fragment_length = 0;
		TRY(parser_get_fragment_length(&length_remaining, &input, &fragment_length));
		
		/* allocate memory for this fragment (+1 for termination) */
		char* fragment = (char*) malloc(fragment_length + 1);
		char* fragment_start = fragment;
		
		/* copy the fragment */
		for(;fragment_length>0; fragment_length--){
			*fragment = *input;
			input++;
			fragment++;
		}
		
		/* terminate the fragment */
		*fragment = '\0';
		TRY(push(s, (element) fragment_start));
	}

	uint32_t size_stack = stack_size( s );
	for(;size_stack>0; size_stack--){
		element temp_string;
		TRY(top(s, &temp_string));
		TRY(push(*pad, temp_string));
	}

   return PARSER_OK;
}

uint32_t parser_get_top(parser p, char** return_string){
	element temp_string;
	TRY(top((stack) p, &temp_string));
	*return_string = (char*) temp_string;
	return PARSER_OK;
}

uint32_t parser_number_of_elements(parser p, uint32_t* number_of_elements){
	*number_of_elements = stack_size( p );
	return PARSER_OK;
}

uint32_t parser_destroy(parser* p){
 TRY(destroy( p ));
 return PARSER_OK;
}

static uint32_t parser_get_fragment_length(uint32_t* length_remaining, char **input_ptr, uint32_t* fragment_length){
	
	char* input = *input_ptr;
	
	if(*length_remaining == 0)
		return PARSER_NO_INPUT;
	
	*fragment_length = 0;

	uint8_t empty = 1;
	
	while(*length_remaining > 0){
		
		//Check if the end is reached
		if((*input == '/' || *length_remaining == 0) && !empty){
			return PARSER_OK;
		}
		
		//Skip the heading slashes, return error if no other input found
		while(*input == '/' && *length_remaining > 0){
			(*length_remaining)--;
			input++;
			(*input_ptr)++;
		}
		if(*length_remaining == 0)
			return PARSER_NO_INPUT;

		//check if the next char is escaped
		if(*input == '\\'){
			if(*length_remaining == 1)
				return PARSER_ILLEGAL_INPUT;
			
			empty = 0;
			*length_remaining	-= 2;
			input += 2;
			*fragment_length +=2;
		} else {
			empty = 0;
			(*length_remaining) --;
			input++;
			(*fragment_length) ++;
		}
	}

	if(empty)
		return PARSER_NO_INPUT;
	
	return PARSER_OK;	
}
