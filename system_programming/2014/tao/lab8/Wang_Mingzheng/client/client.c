/*******************************************************************************
*  FILENAME: tcp_comm.c							       
*
* Version V1.0		
* Author: Luc Vandeurzen
*
* A client example to illustrate the usage of the tcp_comm.h/.c socket API
* 
* Compile this code and start the executable in a terminal. The server must 
* run before the client is started. 
*									    
*******************************************************************************/

/*-----------------------------------------------------------------------------
		include files
------------------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "tcpsocket.h"

/*------------------------------------------------------------------------------
		definitions (defines, typedefs, ...)
------------------------------------------------------------------------------*/
#define BUFSIZE 1024
#define SERVER_IP "127.0.0.1"
#define PORT 1234

unsigned char buffer[BUFSIZE];

//----------------------------------------------------------------------------//
int main( void ) {
//----------------------------------------------------------------------------//

  	Socket client;
	int fd;
	int result;
 	char msg[100];

	// open file
	fd = open( "./sensor.data", O_RDONLY );
	if ( fd == -1 ) {
		#ifdef DEBUG
			perror("File open failed: ");
		#endif
	}


  	// open TCP connection to the server; server is listening to SERVER_IP and PORT
  	client = tcp_active_open( PORT, SERVER_IP );

	while (1)
	{
		result = read( fd, &msg, sizeof(msg) );
		printf("The size of data has been read is %d\n", result);
  		// send msg to server
  		tcp_send( client, (void *)msg, result );
		if (result != sizeof(msg))
		{
			//if the result read is small then ask, meaning the file has been read up
			//then I can exit the program
  			tcp_close( &client );
			exit(1);
		}
		printf("data read has been send.....\n");
  
	}
  	// exit
	tcp_close( &client );
  	return 1;
}
