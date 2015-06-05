/****************************************
*      sensor data simulator
****************************************/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

int main (void) 
{
	int number;
	
	srand( time( NULL ) );
	int i;
	for(i=1;i<50000;i++)
	{
		number = rand();
		printf ("%d\n", number);
		fflush(stdout);
		usleep( 100 );
	}
	
	return 0;
}
