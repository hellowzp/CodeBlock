/****************************************
*      sensor data simulator
****************************************/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main (void) 
{
	int number;
	
	srand( time( NULL ) );
	
	while ( 1 ) {
		number = rand();
		printf ("%d\n", number);
		fflush(stdout);
		usleep( 1000000 );
	}
	
	return 0;
}
