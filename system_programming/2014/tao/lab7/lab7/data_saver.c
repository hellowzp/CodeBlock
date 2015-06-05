/****************************************
*      Database saver
****************************************/


/*-----------------------------------------------------------------------------
		include files
------------------------------------------------------------------------------*/
#include <fcntl.h>
#include <sys/types.h>
#include "config.h"
#include <unistd.h>
#include <stdio.h>
#include <string.h>

#include <my_global.h>
#include <mysql.h>

//----------------------------------------------------------------------------//
int main(int argc, char **argv)
//----------------------------------------------------------------------------//
{
	int fd;
	int result;
	int countElements;
	int i;
	sensor_data_t my_sensor_data;
	MYSQL *conn;
	char id_buffer[32];
	char value_buffer[32];
	char ts_buffer[32];
	char container[200];

	// open file
	fd = open( "./sensor.data", O_RDONLY );
	if ( fd == -1 ) {
		#ifdef DEBUG
			perror("File open failed: ");
		#endif
	}

	// check if file size corresponds with stack size
	result  = lseek( fd, 0, SEEK_END );
	if ( result == -1 ) 
	{
		#ifdef DEBUG
			perror("File lseek failed: ");
		#endif
	}
	
	//calculate the number of elements in the database and print the result
	countElements = result / sizeof(sensor_data_t);
	printf("The number of data element in the file is %d\n", countElements);
	
	//reset the fd to the beginning of the file
	result  = lseek( fd, 0, SEEK_SET );
	if ( result == -1 ) 
	{
		#ifdef DEBUG
			perror("File lseek failed: ");
		#endif
	}
	
	//print out the version of the mySQL
	printf("MySQL client version: %s\n", mysql_get_client_info());

	//initialize the database and confirm the initialization
	conn = mysql_init(NULL);
 	if (conn == NULL) 
	{
      		printf("Error %u: %s\n", mysql_errno(conn), mysql_error(conn));
      		exit(1);
 	}

	//connect to the database and confirm the result of connection
	if (mysql_real_connect(conn, "studev.groept.be", "a12_syssoft", "a12_syssoft", "a12_syssoft", 0, NULL, 0) == NULL) 
	{
      		printf("Error %u: %s\n", mysql_errno(conn), mysql_error(conn));
      		exit(1);
  	}
	
	//creat a table in DB
	mysql_query(conn, "CREATE TABLE mingzheng_wang(id INT NOT NULL PRIMARY KEY AUTO_INCREMENT, sensor_id INT, sensor_value INT, timestamp INT)");
	
	printf("Id\tValue\tTimestamp\n");  //print out the coloum names
	// save data into DB
	// and print out the information in "sensor.data"
	for(i = 0; i < countElements; i++)
	{
		//read out information from "sensor.data" element by element
		result = read( fd, &my_sensor_data, sizeof(sensor_data_t) );

		//print out the information read from sensor.data
		printf("%ld\t%ld\t%ld\n", (long int)my_sensor_data.id, (long int)my_sensor_data.value, (long int)my_sensor_data.ts);
		
		//save what is read out into database
		//convert the integer to string type
		sprintf(id_buffer, "%ld", (long int)my_sensor_data.id);
		sprintf(value_buffer, "%ld", (long int)my_sensor_data.value);
		sprintf(ts_buffer, "%ld", (long int)my_sensor_data.ts);

		//make up the query string
		strcpy(container, "INSERT  INTO mingzheng_wang (sensor_id, sensor_value, timestamp) VALUES(");
		strcat(container, id_buffer);
		strcat(container, ",");
		strcat(container, value_buffer);
		strcat(container, ",");
		strcat(container, ts_buffer);
		strcat(container, ")");
		//excute the query
		mysql_query(conn, container);
		//reset the query container after each query
		memset(container, 0, 200);
	}	
	
	//close the database after everything is done
	mysql_close(conn);
	return 0;
}








