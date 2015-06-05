/****************************************
*      sensor data simulator
****************************************/








int main (void) 
{
	
	
	
	
	while ( 1 ) {
		
	}
	
	return 0;
}
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "tcpsocket.h"

#define BUFSIZE 1024
#define SERVER_IP "127.0.0.1"
#define PORT 2345

unsigned char buffer[BUFSIZE];

int main( void ) {
  
    Socket client;
    int ret;
    char msg[1000];
    FILE *fp;
    int number;
 
    srand( time( NULL ) );
   
   while(1){
		
		// open TCP connection to the server; server is listening to SERVER_IP and PORT
		client = tcp_active_open( PORT, SERVER_IP );
		printf("Type in what you want to Send to Server: \n");
		
		
		number = rand();
		printf ("%d\n", number);
		
		//type cast int to char
		sprintf(msg,"%d",number);
		fflush(stdout);
		usleep( 10000);
		

		// send msg to server
		tcp_send( client, (void *)msg, strlen(msg)+1 );
  
        // get reply from server
        printf("\nanswer from server: ");
	
		while( (ret = tcp_receive (client, buffer, BUFSIZE)) > 0 ) {
			printf("%s", buffer);
			fflush(stdout);
		}
		printf("\n");  
	}

    // exit
    tcp_close( &client );
    return 1;
}
