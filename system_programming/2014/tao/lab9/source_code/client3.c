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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "tcpsocket.h"

#define BUFSIZE 1024
#define SERVER_IP "127.0.0.1"
#define PORT 1234
#define FILE_NAME_MAX_SIZE 512


char buffer[BUFSIZE];

int main( void ) {
  Socket client;
  int ret;
  char msg[1000] ;
  FILE *fp;
  char file_name[FILE_NAME_MAX_SIZE+1];
 

  // open TCP connection to the server; server is listening to SERVER_IP and PORT
  client = tcp_active_open( PORT, SERVER_IP );
  printf("Please Input File Name On the Client.\t");
  scanf("%s",file_name);//type in the name of file you want to get from client
 
 
  strncpy(buffer, file_name, strlen(file_name) > BUFSIZE ? BUFSIZE : strlen(file_name));
  tcp_send( client, (void *)buffer, strlen(buffer)+1 );
  bzero(buffer,sizeof(buffer));
  
  //a loop used to recieve the feedback from server
  while(buffer == NULL){
  
  ret = tcp_receive (client, buffer, BUFSIZE);
  }
    
  fp = fopen(file_name,"r");
  if(fp==NULL){
  
  printf("failed open!");
  
  }
  else{
	  printf("file open success! \n");
  } 
  
  //begin to send file to server
  while(fread(msg,10,sizeof(char),fp)==1&&( (ret = tcp_receive (client, buffer, BUFSIZE)) > 0 )){
	   
	printf("writing %s",msg);
	// send msg to server
	tcp_send( client, (void *)msg, strlen(msg)+1 );
	sleep(1);
	// get reply from server
	printf("\nanswer from server: %s",buffer);
	
  }

  printf("\n");  
  
  fclose(fp);
  // exit
  tcp_close( &client );
  return 1;
}
