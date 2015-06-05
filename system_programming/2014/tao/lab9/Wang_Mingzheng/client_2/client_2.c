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
#define PORT 1268

unsigned char buffer[BUFSIZE];

int main( void ) {
  Socket client;
  int ret;
  char msg[] = "Hello there! This is client 2 connecting to port 1268";

  // open TCP connection to the server; server is listening to SERVER_IP and PORT
  client = tcp_active_open( PORT, SERVER_IP );

  // send msg to server
  tcp_send( client, (void *)msg, strlen(msg)+1 );
  
  // get reply from server
  printf("\nanswer from server: ");
  if ( (ret = tcp_receive (client, buffer, BUFSIZE)) > 0 ) {
     printf("%s", buffer);
  }
  printf("\n");  

  // exit
  tcp_close( &client );
  return 1;
}
