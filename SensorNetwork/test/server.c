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


unsigned char buffer[BUFSIZE];

int main( void )
{
  Socket server, client;
  int bytes;

  // open server socket
  server = tcp_passive_open( PORT );

  while( 1 ) {	
    client = tcp_wait_for_connection( server );
    printf("incoming client connection\n");

    bytes = tcp_receive( client, (void *)buffer, BUFSIZE );
    printf("received message of %d bytes: %s\n", bytes, buffer );

    // echo msg back to client
    tcp_send( client, (void*)buffer, bytes );

    //sleep(1);	/* to allow socket to drain */
    tcp_close( &client );
  }

  tcp_close( &server );
  return 0;
}

