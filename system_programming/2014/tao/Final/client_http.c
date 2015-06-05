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
#define FILENAME "./LogRecord.txt"

void Doc_Server(Socket client,char file_name[]);
char buffer[BUFSIZE];

int main( void ) {
	 
	  Socket client;
	  int ret;
	  FILE *fp;

	 

	  // open TCP connection to the server; server is listening to SERVER_IP and PORT
	  client = tcp_active_open( PORT, SERVER_IP );

	  ret = tcp_receive (client, buffer, BUFSIZE);
	  printf("Recieve %s\n",buffer);
	  
	  tcp_send( client, (void *)buffer, strlen(buffer)+1 );
	  
	  memset(buffer,0,BUFSIZE);
	  
	  fp = fopen(FILENAME,"w");
		    
	  if (fp == NULL)
	  {
		printf("File:\t%s Can Not Open To Write!!!!!\n", buffer);
		exit(1);
	  }
	  fclose(fp);
	  Doc_Server(client,FILENAME);
	  return 0;
}

void Doc_Server(Socket client,char file_name[]){
	
	int bytes;
	FILE *fp;
    fflush(stdout);
    printf("%s",file_name);
	
	bzero(buffer, sizeof(buffer));
	fp = fopen(file_name,"a");
	do{
 
    //open a file
    
    
     bytes = tcp_receive( client, buffer, BUFSIZE ); 


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

     
    fwrite(buffer,sizeof(char),bytes,fp);
    //echo back to server
    tcp_send( client, (void *)buffer, strlen(buffer)+1 );
    //clear buffer
    memset(buffer,0,BUFSIZE);

	}while(bytes>0);
	fclose(fp);
	tcp_close(&client);
	
}		  
	  

		
	
