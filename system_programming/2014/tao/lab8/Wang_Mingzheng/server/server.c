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
#include "tcpsocket.h"
#include "double_list.h"

/*------------------------------------------------------------------------------
		definitions (defines, typedefs, ...)
------------------------------------------------------------------------------*/
#define BUFSIZE 1024
#define PORT 1234
typedef int data_t;

/*------------------------------------------------------------------------------
		function prototypes
------------------------------------------------------------------------------*/
void child_server(Socket client, int fd);
void destory_data(data_ptr_t data);
int compare_data(data_ptr_t data_1, data_ptr_t data_2);
data_ptr_t deep_copy(data_ptr_t data);

unsigned char buffer[BUFSIZE];

//----------------------------------------------------------------------------//
int main( void )
//----------------------------------------------------------------------------//
{
 	Socket server, client;
	int child_pid;
	int wpid;
	int status;
	int fd;
	int state;
	int i;
	char sub_dir;
	char *file_loc = NULL;
	list_ptr_t list = NULL;
 

  	// open server socket
 	 server = tcp_passive_open( PORT );

	printf("\n*******Choose the subdirectory your want to store you data then hit ENTER ******\n");
	printf("*******    '1' for 'parent dir/subdir_1/'      *******\n");
	printf("*******    '2' for 'parent dir/subdir_2/'      *******\n");
	printf("*******    '3' for 'parent dir/subdir_3/'      *******\n");
	printf("*******     :     :     :     :     :          *******\n");
	printf("*******     :     :     :     :     :          *******\n");
	printf("*******    '9' for 'parent dir/subdir_9/'      *******\n");
	printf("*******    '0' for storing the data just in parent directory ******\n");

	scanf("%c", &sub_dir);
	
	// switch among different subdirectories in the parent dirctory
	switch(sub_dir)
	{
		case '1': file_loc = "./subdir_1/sensor.data";
		break;
		case '2': file_loc = "./subdir_2/sensor.data";
		break;
		case '3': file_loc = "./subdir_3/sensor.data";
		break;
		case '4': file_loc = "./subdir_4/sensor.data";
		break;
		case '5': file_loc = "./subdir_5/sensor.data";
		break;
		case '6': file_loc = "./subdir_6/sensor.data";
		break;
		case '7': file_loc = "./subdir_7/sensor.data";
		break;
		case '8': file_loc = "./subdir_8/sensor.data";
		break;
		case '9': file_loc = "./subdir_9/sensor.data";
		break;
		case '0': file_loc = "./sensor.data";
		break;
		default: file_loc = "./sensor.data";
	}

	// open file
	fd = open( file_loc, O_CREAT | O_WRONLY | O_TRUNC, S_IRWXU );
		if ( fd == -1 ) {
			#ifdef DEBUG
			perror("File open failed: ");
			#endif
	}
	
 	 while( 1 ) {	
	
		printf("\nWaiting for a connection from a client.....\n");
  	 	client = tcp_wait_for_connection( server );
		
  	 	printf("Incoming client connection\n");
	 	child_pid = fork();
	 	if (child_pid == 0)  // if the this is the child pid run the function for child
	 	{
			child_server(client, fd);
			break;
	 	}
	 	else if (child_pid > 0) // run the following code if this is the parent process
	 	{
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
		else if (child_pid == -1) // if the fork is a failure, break the while loop in parent dirctory
		{
			perror("fork is a failure because: ");
			break;
		}

   	 	sleep(1);	// to allow socket to drain 
  	}

	//after the while loop is exited
	if (child_pid == 0)
	{
		printf("child process pid = %d is terminating, the transmission is over\n", getpid());
	}
	else if (child_pid > 0) // the parent process should wait until its children before exiting
	{
		
  		tcp_close( &server );
		while ((wpid = wait(&status)) > 0)
   		{
        		printf("Exit status of %d was %d (%s)\n", (int)wpid, status,
               		(status > 0) ? "accept" : "reject");
   		}
		list_free(list);
		
	}


  	return 0;
}

//----------------------------------------------------------------------------//
void child_server(Socket client, int fd)
//----------------------------------------------------------------------------//
{
	int bytes;
	int result;
	printf("Child process pid = %d has been created\n", getpid());
	while (1)
	{
		bytes = tcp_receive( client, buffer, BUFSIZE );
		printf("received message of %d bytes\n", bytes);

		result = write(fd, buffer, bytes);
		if ( result == -1 )
		{
			perror("Write failed");
			exit(1);
		}
		if (bytes == 0 ) //when there is no bytes coming 
		{
			break;
		}
	}
   	tcp_close( &client );
}

//----------------------------------------------------------------------------//
void destory_data(data_ptr_t data)
//----------------------------------------------------------------------------//
{
	if (data != NULL) free((data_t *)data);  	//free the data if it is not null
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








