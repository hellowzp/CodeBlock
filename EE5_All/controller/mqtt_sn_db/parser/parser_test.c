#include "parser.h"
#include "stdio.h"
#include "string.h"

int main( int argc, char** argv){
//	char str[1000];
//	scanf("%s", str);
	char* str = "This/is*/a/test?? ////to/see.!!/if\\/the\\\\/function/\\/works/ /:)";
	printf("the test string: %s\n", str);
	parser p;
	uint32_t number_of_wildcards;
	
	if(parser_string_to_pad(strlen(str), str, &p, &number_of_wildcards))
		printf("An error occurred parsing the pad");
	printf("number of wildcard: %d\n", number_of_wildcards);

	uint32_t elements = 0;
	if(parser_number_of_elements(p, &elements))
		printf("An error occured getting the number of elements\n");
	printf("number of elements: %d\n", elements);

	printf("The pad looks like this:\n");
	for(; elements >0; elements --){
		parser_get_top(p, &str);
		printf("%s\n", str);
	}

	parser_destroy(&p);
}
