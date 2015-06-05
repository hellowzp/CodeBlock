#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <limits.h>

int main(void)
{
   char *p;   //local variable can only be declared in the first row
   time_t t;
   int i , n;
   const int frq = 5;
   
   char ch = 'A';
   //printf("%d\n",ch); //the output is negative!! -->signed value
   printf("char is stored at %s type!\n\n", (ch>0)?"unsigned":"signed");
   
   printf("The memory size of int is %ld \n", sizeof(int) );
   printf("The memory size of long int is %ld \n", sizeof(long int) );
   printf("The memory size of short int is %ld \n", sizeof(short int) );
   printf("The memory size of float is %ld \n", sizeof(float) );
   printf("The memory size of double is %ld \n", sizeof(double) );
   printf("The memory size of char is %ld \n", sizeof(char) );
   printf("The memory size of void is %ld \n", sizeof(void) );   
   printf("The memory size of pointer is %ld \n\n", sizeof(p) );
   
   //temperature test   
   srand((unsigned) time(&t));
   //srand((unsigned)(time(NULL)));   
   n = 10;
   for( i = 0 ; i < n ; i++ ) {

      float r = (rand()%3000)/100.0;
      printf("Temperature = %3.2f ", r );

      time_t curtime;

      time(&curtime);

      printf(" @ %s\n", ctime(&curtime));

 
      fflush(stdout);
      sleep(frq);
   }
           
   return 0;
}

