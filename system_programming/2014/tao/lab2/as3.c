#include<stdlib.h>
#include<stdio.h>
#include<time.h>
#define MAX 10
#define MIN 2
int main(void)
{
int number;
srand((unsigned)time(NULL));
number=rand()%(MAX-MIN)+MIN;
//printf("%d\n",number);
int i;
for(i=number;i>=0;i--)
{

printf("\a");
printf("%d\n",i);
}
return 0;
}
