/****************************************
*      sensor data simulator
****************************************/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "config.h"
#include <sys/types.h>
#include <fcntl.h>
#include "gateway.h"



int main (void) 
{
	int number;
	int fd;
	int result;
	packet_ptr_t my_packet_ptr;
	sensor_data_t my_sensor_data;

	// open file
	fd = open( "./sensor.data", O_CREAT | O_WRONLY | O_TRUNC, S_IRWXU );
		if ( fd == -1 ) {
			#ifdef DEBUG
			perror("File open failed: ");
			#endif
	}

	srand( time( NULL ) );
	
	while ( 1 ) {
		number = rand();
//		printf ("%d\n", number);
//		fflush(stdout);
		usleep( 100000 );
		
		my_packet_ptr = (packet_ptr_t)&number;
		my_sensor_data.id = (sensor_id_t)my_packet_ptr->sensor_id;
		if (my_packet_ptr->sensor_sign == 1)
		{
			my_sensor_data.value = -(sensor_value_t)my_packet_ptr->sensor_value;
		}
		else
		{
			my_sensor_data.value = (sensor_value_t)my_packet_ptr->sensor_value;
		}
		my_sensor_data.ts = (sensor_ts_t)time(NULL);

		result = write(fd, &my_sensor_data, sizeof(my_sensor_data));
		if ( result == -1 ) 
		{
			#ifdef DEBUG
			perror("File write failed: ");
			#endif
		}
		if (result != sizeof(my_sensor_data))
		{
			#ifdef DEBUG
			fprintf(stderr, "File write failed: less bytes written than requested: \n");
			#endif
		}
	}

	if ( close(fd) == -1 ) {
		#ifdef DEBUG
			perror("File close failed: ");
		#endif
	}
	
	return 0;
}











