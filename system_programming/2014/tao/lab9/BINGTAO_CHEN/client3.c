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
#define PORT 4567
#define FILE_NAME_MAX_SIZE 512


char buffer[BUFSIZE];

int main( void ) {
  Socket client;
  int ret,size;
  char msg[1024] ;
  FILE *fp;
  char file_name[FILE_NAME_MAX_SIZE+1];
 

  // open TCP connection to the server; server is listening to SERVER_IP and PORT
  client = tcp_active_open( PORT, SERVER_IP );
  printf("Please Input File Name On the Client.\t");
  scanf("%s",buffer);//type in the name of file you want to get from client
 
  tcp_send( client, (void *)buffer, strlen(buffer)+1 );

  
  
 
  
  //ret = tcp_receive (client, buffer, BUFSIZE);

    
  fp = fopen(buffer,"r");
  if(fp==NULL){
  
  printf("failed open!");
  
  }
  else{
	  printf("file open success! \n");
  } 
  
  // used to recieve the feedback from server
  if(ret = tcp_receive (client, buffer, BUFSIZE)>0){
  
  
  		//begin to send file to server
 		 while((size=fread(msg,sizeof(char),1024,fp))>0){
	   
			printf("writing %s",msg);
			// send msg to server
			tcp_send( client, (void *)msg, size);
         }
	
   }

  printf("\n");  
  
  fclose(fp);
  // exit
  tcp_close( &client );
  return 1;
}
