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

char buffer[BUFSIZE];
void fork_Server(Socket client,char file_name[]);


int main( void )
{
  Socket server, client;

  int P_PID;
  int WAIT_PID;
  int status,ret;
  char file_name[FILE_NAME_MAX_SIZE+1];
  
  // open server socket
  server = tcp_passive_open( PORT );
  

  while(1 ) {	
    client = tcp_wait_for_connection( server );
    printf("incoming client connection\n");
    P_PID = fork();
    
    if(P_PID == 0){
		
		bzero(file_name,sizeof(file_name));
		//
		while(buffer == NULL){
  
		ret = tcp_receive (client, buffer, BUFSIZE);
		}
		strncpy(file_name,buffer,strlen(buffer) > FILE_NAME_MAX_SIZE ? FILE_NAME_MAX_SIZE : strlen(buffer));
		
		fork_Server(client,file_name);
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

void fork_Server(Socket client,char file_name[]){
	
	int bytes;
	FILE *fp;
		
	
	bzero(buffer, sizeof(buffer));
	do{
		
    bytes = tcp_receive( client, buffer, BUFSIZE ); 
    fp = fopen(file_name,"a");
    if (fp == NULL)
	{
		printf("File:\t%s Can Not Open To Write!\n", file_name);
		exit(1);
	}
    if (bytes < 0)
	{
		printf("Recieve Data From Client Failed!\n");
		break;
	}
    printf("received message of %d bytes: %s\n", bytes, buffer );
    printf("I am beginning to write！");
     
    fwrite(buffer,15,sizeof(char),fp);
	//close file
	fclose(fp);
	tcp_send( client, (void*)buffer, bytes );
	}while(bytes>0);
	
	
}
