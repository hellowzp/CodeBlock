#include<stdio.h>
#include<stdlib.h>
void swap(int **x,int **y);
int main(){
	int a=1;
	int b=2;
	int *p=&a;
	int *q=&b;
	printf("The address of a= %p and b= %p \n",p,q);
	swap(&p,&q);
	printf("The address of a= %p and b= %p \n",p,q);
	printf("the value of a is %d and b is %d\n",*p,*q);
	return 0;
	}
void swap(int **x,int **y){
		int *z;
  		//*z=x; 
		//*c=y;
		z=*x;
        *x=*y;
        *y=z;
		//return 0;
		}
