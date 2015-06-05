#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include "tcpsocket.h"
#include <arpa/inet.h> //close
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h> 
#include <time.h>


#define BUFSIZE 1024
#define PORT 1234
#define TRUE 1
#define FALSE 0
#define FILE_NAME_MAX_SIZE 512

unsigned char buffer[BUFSIZE];

void Doc_Server(int new_socket,char file_name[]);

int main(void)
{
	int opt = TRUE;
	int flag=0;
	int bytes;
	int master_socket , addrlen , new_socket , client_socket[30] , max_clients = 30 , activity, i , valread , s;
	struct sockaddr_in address;
	time_t rawtime;
    struct tm * timeinfo;
	char buffer[1025]; //data buffer of 1K
	char file_name[FILE_NAME_MAX_SIZE+1];
    FILE *fp;
	//set of socket descriptors
	fd_set readfds;
	//a message
	char *message = "Hello New Client!!! \r\n";
	//initialise all client_socket[] to 0 so not checked
	for (i = 0; i < max_clients; i++) 
	{
		client_socket[i] = 0;
	}
	//create a master socket
	if( (master_socket = socket(AF_INET , SOCK_STREAM , 0)) == 0) 
	{
		perror("socket failed");
		exit(EXIT_FAILURE);
	}
	//set master socket to allow multiple connections 
	if( setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)) < 0 )
	{
		perror("setsockopt");
		exit(EXIT_FAILURE);
	}
	//type of socket created
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons( 1234 );
	//bind the socket to localhost port 1234
	if (bind(master_socket, (struct sockaddr *)&address, sizeof(address))<0) 
	{
		perror("bind failed");
		exit(EXIT_FAILURE);
	}
	//try to specify maximum pending connections for the master socket
	if (listen(master_socket, MAX_PENDING) < 0)
	{
		perror("listen");
		exit(EXIT_FAILURE);
	}
	//accept the incoming connection
	addrlen = sizeof(address);
	puts("Waiting for connections...");
	while(TRUE) 
	{
		//clear the socket set
		FD_ZERO(&readfds);
		//add master socket to set
		FD_SET(master_socket, &readfds);
		//add child sockets to set
		for ( i = 0 ; i < max_clients ; i++) 
		{
			s = client_socket[i];
			if(s > 0)
		{
			FD_SET( s , &readfds);
		}
	}
	//wait for an activity on one of the sockets , timeout is NULL 
	activity = select( max_clients + 3 , &readfds , NULL , NULL , NULL);
	if ((activity < 0) && (errno!=EINTR)) 
	{
		printf("select error");
	}
	//If something happened on the master socket , then its an incoming connection
	if (FD_ISSET(master_socket, &readfds)) 
	{
		if ((new_socket = accept(master_socket, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0)
		{
			perror("accept");
			exit(EXIT_FAILURE);
		}
		//inform user of socket number - used in send and receive commands
		printf("New connection , socket fd is %d , ip is : %s , port : %d \n" , new_socket , inet_ntoa(address.sin_addr) , ntohs(address.sin_port));
		//send new connection greeting message
		if( send(new_socket, message, strlen(message), 0) != strlen(message) ) 
		{
			perror("send");
		}
		//insert the function from here
		
		 
	switch(new_socket){
	
	case 4:
	    bytes = recv(new_socket, buffer, BUFSIZE, 0);
		printf("received message of %d bytes: %s\n", bytes, buffer );
		// echo msg back to client
		send( new_socket , buffer , strlen(buffer) , 0 );
		break;
	case 5:
	    time ( &rawtime );
		timeinfo = localtime ( &rawtime );
		printf ( "\007The current date/time is: %s", asctime (timeinfo) );
		strcpy(buffer,(asctime(timeinfo)));
	    send( new_socket , buffer , strlen(buffer) , 0 );
	    break;
	case 6:
		//catch the name of file you want to store
		while(file_name == NULL){
        bzero(file_name,sizeof(file_name));
		bytes = recv(new_socket, buffer, BUFSIZE, 0);
		strncpy(file_name,buffer,strlen(buffer) > FILE_NAME_MAX_SIZE ? FILE_NAME_MAX_SIZE : strlen(buffer));
		}
		
		printf("You want to save %s",buffer);
		
		
		//just create a file
		fp = fopen(buffer,"w");
		
		 if (fp == NULL)
		{
			printf("File:\t%s Can Not Open To Write!!!!!\n", file_name);
			exit(1);
		}
		fclose(fp); 
		Doc_Server(new_socket,buffer);
		break;
	default :
	break;
	}
		
		
		send( s , buffer , strlen(buffer) , 0 );
		puts("Welcome message sent successfully");
		//add new socket to array of sockets
		for (i = 0; i < max_clients; i++) 
		{
			s = client_socket[i];
			if (s == 0)
		{	
			client_socket[i] = new_socket;
			printf("Adding to list of sockets as %d\n" , i);
			i = max_clients;
		}
		
		}
	}
	//else its some IO operation on some other socket :)
	for (i = 0; i < max_clients; i++) 
	{
		s = client_socket[i];
		if (FD_ISSET( s , &readfds)) 
		{
			//Check if it was for closing , and also read the incoming message
			if ((valread = read( s , buffer, 1024)) == 0)
			{
			//Somebody disconnected , get his details and print
			getpeername(s , (struct sockaddr*)&address , (socklen_t*)&addrlen);
			printf("Host disconnected , ip %s , port %d \n" , inet_ntoa(address.sin_addr) , ntohs(address.sin_port));
			//Close the socket and mark as 0 in list for reuse
			close( s );
			client_socket[i] = 0;
			}
		//Echo back the message that came in
		else
		{
			//set the terminating NULL byte on the end of the data read
			buffer[valread] = '\0';
			send( s , buffer , strlen(buffer) , 0 );
		}
		}
	}
	}
	return 0;
}

void Doc_Server(int new_socket,char file_name[]){
	
	int bytes;
	FILE *fp;
		
	
	bzero(buffer, sizeof(buffer));
	do{
		
    bytes = recv(new_socket, buffer, BUFSIZE, 0); 
    
    //open a file
    fp = fopen(file_name,"w");
    
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
	send( new_socket , buffer , BUFSIZE , 0 );
	//sleep(1);
	}while(bytes>0);
	
}	
