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

/*-----------------------------------------------------------------------------
		include files
------------------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <time.h>
#include <sys/select.h>
#include "tcpsocket.h"
#include "double_list.h"

/*------------------------------------------------------------------------------
		definitions (defines, typedefs, ...)
------------------------------------------------------------------------------*/
#define BUFSIZE 1024
#define PORT_1 1234
#define PORT_2 1268
#define PORT_3 1354

#undef max
#define max(x,y) ((x) > (y) ? (x) : (y))


typedef int data_t;

/*------------------------------------------------------------------------------
		function prototypes
------------------------------------------------------------------------------*/
void server_service_1(Socket client);
void server_service_2(Socket client);
void server_service_3(Socket client);
void destory_data(data_ptr_t data);
int compare_data(data_ptr_t data_1, data_ptr_t data_2);
data_ptr_t deep_copy(data_ptr_t data);

unsigned char buffer[BUFSIZE];

//----------------------------------------------------------------------------//
int main( void )
//----------------------------------------------------------------------------//
{
 	Socket server_1, server_2, server_3, client; //declear 3 different servers listenning to 3 different ports
	int child_pid;
	int wpid;
	int status;
	int sd_1, sd_2, sd_3;  //declear 3 socket descripters and the maximum socket descriptor
	int max_sd = 0;
	int state;
	int i;
	fd_set rfds;
        struct timeval tv;
	int retval;
	list_ptr_t list = NULL;
 

  	// open server sockets on 3 different ports
 	server_1 = tcp_passive_open( PORT_1 );
	sd_1 = get_socket_descriptor(server_1);
	max_sd = max(max_sd, sd_1);

	server_2 = tcp_passive_open( PORT_2 );
	sd_2 = get_socket_descriptor(server_2);
	max_sd = max(max_sd, sd_2);

	server_3 = tcp_passive_open( PORT_3 );
	sd_3 = get_socket_descriptor(server_3);
	max_sd = max(max_sd, sd_3);


	
 	 while( 1 ) {	

		FD_ZERO(&rfds);  //clear the socket descriptors in the "rfd" set

		//set the three socket decriptors into set 'rfds'
		FD_SET(sd_1, &rfds); 
		FD_SET(sd_2, &rfds); 
		FD_SET(sd_3, &rfds); 

		// define the maximum time that the select function will wait
		tv.tv_sec = 5;
        	tv.tv_usec = 0;

		//do the select
		retval = select(max_sd + 1, &rfds, NULL, NULL, &tv);

		//select() returns -1 if something go wrong
		if (retval == -1) perror("select()");

		//if there is status change the following code will be excuted
		else if (retval) {
			/*chech the socket descriptors one by one to see which one changed its status*/
			if (FD_ISSET(sd_1, &rfds)) {
				client = tcp_wait_for_connection( server_1 );
				child_pid = fork();
				if (child_pid == 0) {
					server_service_1(client);
					break;
				}
			
			}

			else if (FD_ISSET(sd_2, &rfds)) {
				client = tcp_wait_for_connection( server_2 );
				child_pid = fork();
				if (child_pid == 0) {
					server_service_2(client);
					break;
				}
			
			}

			else if (FD_ISSET(sd_3, &rfds)) {
				client = tcp_wait_for_connection( server_3 );
				child_pid = fork();
				if (child_pid == 0) {
					server_service_3(client);
					break;
				}
			
			}
			
			else {
				//code shoud never come here
				exit(0);
			}
			
		}

		if (child_pid > 0) {
			// store each children pid into a double linked list
			
			if (list == NULL)
			{
				list = list_alloc(&destory_data, &compare_data, &deep_copy, (void *)&child_pid);
			}
			else
			{
				list = list_insert_at_index( list, (void *)&child_pid, list_size(list));
			}

			// each time one connnection is set up, check each child to see if it is dead
			for (i = 0; i < list_size(list); i++)
			{
				waitpid(*(data_t *)list_get_data_at_index(list,i), &state, WNOHANG);
			}
		}
	}

	if (child_pid == 0)
	{
		printf("child process pid = %d is terminated, the transmission is over\n", getpid());
	}
	else if (child_pid > 0) // the parent process should wait until its children before exiting
	{
		
  		tcp_close( &server_1 );
		tcp_close( &server_2 );
		tcp_close( &server_3 );
		while ((wpid = wait(&status)) > 0)
   		{
        		printf("Exit  status of %d was %d (%s)\n", (int)wpid, status,
               		(status > 0) ? "accept" : "reject");
   		}
		list_free(list);  //free the list after finishing using it
		
	}
	
  	return 0;
			
		
	

}

//----------------------------------------------------------------------------//
void server_service_1(Socket client)
//----------------------------------------------------------------------------//
{

/*this method simulate the service 1 of the server who is listenning to port '1234'*/
	int bytes;

	bytes = tcp_receive( client, buffer, BUFSIZE );
	tcp_send(client, "echo back from the service 1 on port 1234", 50 );
	printf("service 1 listenning on port 1234 received message of %d bytes :%s\n", bytes, buffer);
	return;
}

//----------------------------------------------------------------------------//
void server_service_2(Socket client)
//----------------------------------------------------------------------------//
{
/*this method simulate the service 2 of the server who is listenning to port '1268'*/
	int bytes;

	bytes = tcp_receive( client, buffer, BUFSIZE );
	tcp_send(client, "echo back from the service 2 on port 1268", 50 );
	printf("service 2 listenning on port 1268 received message of %d bytes :%s\n", bytes, buffer);
	return;
}

//----------------------------------------------------------------------------//
void server_service_3(Socket client)
//----------------------------------------------------------------------------//
{
/*this method simulate the service 3 of the server who is listenning to port '1354'*/
	int bytes;

	bytes = tcp_receive( client, buffer, BUFSIZE );
	tcp_send(client, "echo back from the service 3 on port 1354", 50 );
	printf("service 3 listenning on port 1354 received message of %d bytes :%s\n", bytes, buffer);
	return;
}

//----------------------------------------------------------------------------//
void destory_data(data_ptr_t data)
//----------------------------------------------------------------------------//
{
	if (data != NULL) free((data_t *)data);  	//free the data if it is not null
	return;
}

//----------------------------------------------------------------------------//
int compare_data(data_ptr_t data_1, data_ptr_t data_2)
//----------------------------------------------------------------------------//
{
	
	if ((*(data_t *)data_1) > (*(data_t *)data_2)) return 1;
	else if (*(data_t *)data_1 == *(data_t *)data_2) return 0;
	else return -1;
}

//----------------------------------------------------------------------------//
data_ptr_t deep_copy(data_ptr_t data)
//----------------------------------------------------------------------------//
{
	data_t * t = (data_t *)malloc(sizeof(data_t));
	 *t = *(data_t *)data;

	return (data_ptr_t)t; 
}

//----------------------------------------------------------------------------//
void error_handler(err_code err)
//----------------------------------------------------------------------------//
{
	switch (err)
	{
		case NONE:
			printf("+++++++++++ result ++++++++++++\n This function runs proporiately\n--------------------------------\n\n");
			break;
		case NULL_POINTER_ERR:
			printf("+++++++++++ result ++++++++++++\n no pointer error (your list doesn't exist)\n--------------------------------\n\n");
			break;
		case NONE_EXIST_REF_ERR:
			printf("+++++++++++ result ++++++++++++\n no such reference (your reference is not valid)\n--------------------------------\n\n");
			break;
		case NON_NEXT_REF_ERR:
			printf("+++++++++++ result ++++++++++++\n no next reference (your reference is already the last one)	\n--------------------------------\n\n");
			break;
		case NON_PREVIOUS_REF_ERR:
			printf("+++++++++++ result ++++++++++++\n no previous reference (your reference is already the first one)\n--------------------------------\n\n");
			break;
		case NO_DATA_ERR:
			printf("+++++++++++ result ++++++++++++\n no data error (your data is not stored in the list)\n--------------------------------\n\n");
			break;
		default:
			printf("+++++++++++ result ++++++++++++\n unknow error \n--------------------------------\n\n");
		
	}
}








