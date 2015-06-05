/****************************************
*      sensor data simulator
****************************************/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>




int main () 
{
	int number;
	
	srand( time( NULL ) );
	
	int i;
	for(i=0; i<30; i++) {
		number = rand()%4294967296;
		printf ("%d\n", number);
		fflush(stdout);
		usleep( 100000 );
	}
	
	return 0;
}
