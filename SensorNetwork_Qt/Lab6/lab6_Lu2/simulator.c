/****************************************
*      sensor data simulator
****************************************/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include "simulator.h"


int main (void) 
{        
    outpacket output;
    output.id_b1=0x01;
	int number;
	srand( time( NULL ) );
	while ( 1 ) {
		number = rand();
        output.value_b1=(0xff)&number;
        int highno=number>>8;
        output.value_b2=(0x1f)&highno;
		printf ("%p\n", output);
		fflush(stdout);
		sleep( 1);
	}
	return 0;
}



