/****************************************
*      sensor data simulator
****************************************/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "tcpsocket.h"
#include <time.h>

#define BUFSIZE 1024
#define SERVER_IP "127.0.0.1"
#define PORT 1234

unsigned char buffer[BUFSIZE];

int main( void ) {
  
    Socket client;
    int ret;
    char msg[1000];
    int number;
    int i;
    
    
    // open TCP connection to the server; server is listening to SERVER_IP and PORT
	client = tcp_active_open( PORT, SERVER_IP );
	printf("Client begin to send Sensor_package to Server: \n");	
   
    do{
					
		srand( time( NULL ) );
		number = rand();
		printf ("Package Number is :%d\n", number);
		
		//type cast int to char
		sprintf(msg,"%d",number);
		fflush(stdout);
		sleep(4);
		
        
		// send msg to server
		tcp_send( client, (void *)msg, strlen(msg)+1 );
  
       
        // get reply from server
		printf("\n");  
		memset(buffer,0,BUFSIZE);
		ret = tcp_receive (client, buffer, BUFSIZE);
		printf("\nanswer from server: ");
		printf("%s\n", buffer);
		fflush(stdout);
	}while(ret > 0);

    // exit
    tcp_close( &client );
    return 1;
}
