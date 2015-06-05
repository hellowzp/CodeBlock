/*******************************************************************************
*  FILENAME: server.c							       
*
* Version V1.0		
* Author: Luc Vandeurzen
*
* A server example to illustrate the usage of the tcp_comm.h/.c socket API
* 
* Compile this code and start the executable in a terminal. The server must 
* run before the client is started. 
*									    
*******************************************************************************/



#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include "tcpsocket.h"

#define BUFSIZE 1024
#define PORT 1234
#define FILE_NAME_MAX_SIZE 512

unsigned char buffer[BUFSIZE];
void fork_Server(Socket client);


int main( void )
{
  Socket server, client;

  int P_PID;
  int WAIT_PID;
  int status;
  // open server socket
  server = tcp_passive_open( PORT );
  

  while(1 ) {	
    client = tcp_wait_for_connection( server );
    printf("incoming client connection\n");
    P_PID = fork();
    
    if(P_PID == 0){
	
		fork_Server(client);
		printf("This is a child process! PID is %d",getpid());
		break;
	}
	if(P_PID < 0){
	    
	    printf("Fork failed! PID is");
	    
		//wait(&status);
	
	}
 
    if (P_PID == 0){
		
		printf("child process pid = %d is terminating, the transmission is over\n", getpid());
	}
	else{  // the parent process should wait until its children 
	
		
  		//tcp_close( &server );
		while ((WAIT_PID = wait(&status)) > 0)
   		{
        		printf("STATE of %d was %d (%s)\n", (int)WAIT_PID, status,
               		(status > 0) ? "EXIT" : "STILL RUNNING");
   		}
		
		
	}
	
    // echo msg back to client
    //tcp_send( client, (void*)buffer, bytes );

    //sleep(1);	/* to allow socket to drain */
   
    tcp_close( &client );
  }
  
  tcp_close( &server );
  return 0;
}

void fork_Server(Socket client){
	
	int bytes;
	FILE *fp;
	do{
		
    bytes = tcp_receive( client, buffer, BUFSIZE ); 
    printf("received message of %d bytes: %s\n", bytes, buffer );
    printf("I am beginning to write！");
    fp = fopen("./recieve.txt","a");
      
	if (fp==NULL){
		printf("File:\t Can Not Open To Write \n");
        exit(1);
	}
  
    fwrite(buffer,15,sizeof(char),fp);
	fclose(fp);
	tcp_send( client, (void*)buffer, bytes );
	}while(bytes>0);
	
	
	
	
}
