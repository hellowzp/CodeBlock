#include <stdio.h>
#include <stdlib.h>
       
/*
typedef struct {
	int * ptr1;
        int * ptr2;
} Address;
Address add;
Address* swap(int *p1 , int *p2) {
	printf("THe address of p1: %p, p2: %p\n",p1,p2);
	int *t = p1;
	p1 = p2;
	p2 = t;
	printf("THe address of p1: %p, p2: %p\n",p1,p2);
	add.ptr1 = p1;
	add.ptr2 = p2;
	return &add;
}
*/

void swap(int **p1, int **p2) {
	printf("THe address of p1: %p, p2: %p\n",p1,p2);
	printf("THe content of p1: %p, p2: %p\n",*p1,*p2);

	int **t1 = p1, **t2 = p2;
	printf("THe address of t1: %p, t2: %p\n",t1,t2);
	*p1 = *t2;
	printf("THe address of p1: %p, p2: %p\n",p1,p2);	
	*p2 = *t1;
	printf("THe address of p1: %p, p2: %p\n",p1,p2);
	printf("THe content of p1: %p, p2: %p\n",*p1,*p2);
}

int main(void) {
	int a=1, b=2, *p=&a, *q=&b;
	printf("THe address of a: %p, b: %p\n",&a,&b);
	printf("THe address of p: %p, q: %p\n",&p,&q);
	//printf("THe content of p: %p, p: %p\n",*p,*q);
	
	swap(&p,&q);
	printf("THe address of a: %p, b: %p\n",&a,&b);
	printf("THe address of p: %p, q: %p\n",p,q);
	return 0;
}

