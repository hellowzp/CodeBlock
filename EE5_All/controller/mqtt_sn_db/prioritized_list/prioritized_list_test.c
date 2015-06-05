#include "prioritized_list.h"

pl_data copy (pl_data);
void destroy (pl_data* data);
void test_1(void);
prioritized_list list;

int main(){

	printf("error: %d\n", create_prioritized_list(&list, 20000, &destroy, &copy));
	test_1();
	exit(0);
}

void test_1(void){
	srand(time(NULL));

	int i = 0;
	for(; i<8; i++){
		uint32_t priority = i%2;
		uint32_t weight = rand()%10;
		int* tmp =(int*) malloc(sizeof(int));
		*tmp = i;
		add_data_blocking(list, tmp, i/4, priority, weight);
		print_prioritized_list(list);
	}
}

pl_data copy (pl_data data){
	int* tmp =(int*) malloc(sizeof(int));
	*tmp = *((int*) data);
	return (pl_data) tmp;
}
void destroy(pl_data* to_destroy){
	free((void*) to_destroy);
	*to_destroy = NULL;
}
