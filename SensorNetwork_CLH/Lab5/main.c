#include <stdio.h>
#include <stdlib.h>
#include "list.h"

void copy(void* *dest, void *src){}
void free(void *element){}
int compare(void *x, void *y){}
void print(void *element){}

int main(){
	list_t *my_list;
	
	my_list=list_create(&copy,&free,&compare,&print);
	
	return 0;
}
