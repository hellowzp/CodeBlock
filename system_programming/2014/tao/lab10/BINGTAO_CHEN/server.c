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
#include <sys/wait.h>
#include <assert.h>
#include "gateway.h"
#include "D_list.h"
#include "server.h"
#include <pthread.h>

/*-----------------------------------------------------------------------
 * 								define
 * ----------------------------------------------------------------------*/
#define BUFSIZE 1024
#define PORT 1234
#define TRUE 1
#define FALSE 0



/*-----------------------------------------------------------------------
 * 							global variable
 * ----------------------------------------------------------------------*/
char buffer[BUFSIZE];
static CThread_pool *pool = NULL; 



/*-----------------------------------------------------------------------
 * 							main
 * ----------------------------------------------------------------------*/

int main (int argc, char **argv) 
{ 
	 int opt = TRUE;
	 int bytes=0;
	 int j=0;
	 int master_socket , addrlen , new_socket , client_socket[30] , max_clients = 30 , activity, i , valread , s,app_socket;
	 struct sockaddr_in address;
	 int *workingnum = NULL;


	 //set of socket descriptors
	 fd_set readfds;
	 //a message
	 char *message = "Hello new client!THis is Server.\n";
	 Socket server,client,server_app;
	
	
	 pool_init (5);/*set five threads that can active*/
	 server = tcp_passive_open( PORT1);
	 server_app = tcp_passive_open(PORT2);
	 master_socket = get_socket_descriptor(server);
	 app_socket = get_socket_descriptor(server_app);
	 
	 //initialise all client_socket[] to 0 so not checked
	 for (i = 0; i < max_clients; i++) 
	 {
		client_socket[i] = 0;
	 }
      
     addrlen = sizeof(address);
	 
	 while(1){
	    
	    //clear the socket set
		FD_ZERO(&readfds);
		//add master socket to set
		FD_SET(master_socket, &readfds);
		FD_SET(app_socket,&readfds);
		//add child sockets to set
		for ( i = 0 ; i < max_clients ; i++) 
		{
			s = client_socket[i];
			if(s > 0)
			{
				FD_SET( s , &readfds);
			}
		}  


		activity = select( max_clients + 1 , &readfds , NULL , NULL , NULL);
		if ((activity < 0) && (errno!=EINTR)) 
		{
			printf("select error");
		}
		//If something happened on the master socket , then its an incoming connection
		if (FD_ISSET(master_socket, &readfds)) 
		{
            
			client = tcp_wait_for_connection( server );
			printf("incoming client connection\n");
			new_socket=get_socket_descriptor(client);
			
		    //tcp_close(&server);
			//inform user of socket number - used in send and receive commands
			printf("New connection , socket fd is %d , ip is : %s , port : %d \n" , new_socket , get_ip_addr(client ) , get_port( client));
			
			
			//send new connection greeting message
			tcp_send( client, message, BUFSIZE);
			pool_add_worker (myprocess, client); 

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

		if (FD_ISSET(app_socket,&readfds)){
		
			//one client_app is coming display function
				
		}
		}
   
      /*wait all threads finished*/ 
      sleep(5);
      //pthread_join(tid, NULL);  
      printf("All done -- exiting/n"); 
     
      /*destroy the thread*/ 
      pool_destroy (); 

    
      return 0; 
}


/*-----------------------------------------------------------------------
 * 							pool init
 * ----------------------------------------------------------------------*/
void pool_init (int max_thread_num) 
{ 
     pool = (CThread_pool *) malloc (sizeof (CThread_pool)); 


     pthread_mutex_init (&(pool->queue_lock), NULL); 
     pthread_cond_init (&(pool->queue_ready), NULL); 


     pool->queue_head = NULL; 


     pool->max_thread_num = max_thread_num; 
     pool->cur_queue_size = 0; 


     pool->shutdown = 0; 


     pool->threadid = (pthread_t *) malloc (max_thread_num * sizeof (pthread_t)); 
    int i = 0; 
    for (i = 0; i < max_thread_num; i++) 
    { 
         pthread_create (&(pool->threadid[i]), NULL, thread_routine, 
                 NULL); 
    } 
} 


/*-----------------------------------------------------------------------
 * 							add worker
 * ----------------------------------------------------------------------*/
 
int pool_add_worker (void *(*process) (void *arg), void *arg) 
{ 
     /*new worker*/ 
     CThread_worker *newworker = (CThread_worker *) malloc (sizeof (CThread_worker)); 
     newworker->process = process; 
     newworker->arg = arg; 
     newworker->next = NULL;


     pthread_mutex_lock (&(pool->queue_lock)); 
     /*add the work into the waiting list*/ 
     CThread_worker *member = pool->queue_head; 
     if (member != NULL) 
     { 
        while (member->next != NULL) 
             member = member->next; 
         member->next = newworker; 
     } 
     else 
     { 
         pool->queue_head = newworker; 
     } 


     assert (pool->queue_head != NULL); 


     pool->cur_queue_size++; 
     pthread_mutex_unlock (&(pool->queue_lock)); 
     /*wake up one thread*/ 
     pthread_cond_signal (&(pool->queue_ready)); 
     return 0; 
} 


/*-----------------------------------------------------------------------
 * 							pool_destroy
 * ----------------------------------------------------------------------*/
 
int pool_destroy () 
{ 
    if (pool->shutdown) 
        return -1;/* avoid of using second time*/ 
     pool->shutdown = 1; 


    /*wake up all the waiting thread,then to deatroy*/ 
     pthread_cond_broadcast (&(pool->queue_ready)); 


     /*waiting thread exit*/
     int i; 
     for (i = 0; i < pool->max_thread_num; i++) 
         pthread_join (pool->threadid[i], NULL); 
     free (pool->threadid); 


     /*destroy the waiting list*/ 
     CThread_worker *head = NULL; 
     while (pool->queue_head != NULL) 
     { 
         head = pool->queue_head; 
         pool->queue_head = pool->queue_head->next; 
         free (head); 
     } 
     /*destroy the mutex and condition variable*/ 
     pthread_mutex_destroy(&(pool->queue_lock)); 
     pthread_cond_destroy(&(pool->queue_ready)); 
     
     free (pool); 
     /*make pool point to null*/ 
     pool=NULL; 
     return 0; 
} 

/*-----------------------------------------------------------------------
 * 							thread_routine
 * ----------------------------------------------------------------------*/
void * thread_routine (void *arg) 
{ 
     printf ("starting thread 0x%x\n", pthread_self ()); 
	 while (1) 
     { 
         pthread_mutex_lock (&(pool->queue_lock)); 
         /*if the waiting list is 0 and the pool is not destroyed,then it is 
          * in the block model;pthread_cond_wait is a kind of enter for the mutex*/ 
         
         while (pool->cur_queue_size == 0 && !pool->shutdown) 
         { 
             printf ("thread 0x%x is waiting\n", pthread_self ()); 
             pthread_cond_wait (&(pool->queue_ready), &(pool->queue_lock)); 
         } 


          /*the thread pool going to be destroyed*/ 
          if (pool->shutdown) 
          { 
            /*unlock*/ 
             pthread_mutex_unlock (&(pool->queue_lock)); 
             printf ("thread 0x%x will exit\n", pthread_self ()); 
             pthread_exit (NULL); 
          } 


         printf ("thread 0x%x is starting to work\n", pthread_self ()); 


         /*assert*/ 
         assert (pool->cur_queue_size != 0); 
         assert (pool->queue_head != NULL); 
         
         /*get the function method,and begin the task*/ 
         pool->cur_queue_size--; 
         CThread_worker *worker = pool->queue_head; 
         pool->queue_head = worker->next; 
         pthread_mutex_unlock (&(pool->queue_lock)); 


         /*free the worker and set to null*/ 
         (*(worker->process)) (worker->arg); 
         free (worker); 
         worker = NULL; 
     } 
     /*exit the thread(it will not get here)*/ 
     pthread_exit (NULL); 
} 

/*-----------------------------------------------------------------------
 * 							thread's funciton
 * ----------------------------------------------------------------------*/
void * myprocess (void *arg) 
{ 
    int number=0;
	int ones_check=0;
	packet_ptr_t temp_packet_ptr;
	list_ptr_t  *sensor_array;
	int parityNum=0;               
	data_t data;
	time_t base_T = time(NULL);	//keep to track time
	time_t variable_T = time(NULL);	
	char pac[1024];
	int bytes=0;
	Socket client;

	init_array(&sensor_array); //initialization
	
	client = arg;
	while ( 1 ) 
    {
		bytes = tcp_receive( client, buffer, BUFSIZE ); 
		printf("received message of %d bytes: %s\n", bytes, buffer );

		// echo msg back to client
		tcp_send( client, (void*)buffer, bytes );
		
		strcpy(pac,buffer);
		number = atoi(pac);
		printf ("the gateway is %d\n", number);
        memset(buffer,0,BUFSIZE);
		temp_packet_ptr = (packet_ptr_t)(&number);  //number's value to temp_packet_ptr

		// to count the number of '1' in the data packet
		for (ones_check = 0; *(int *)temp_packet_ptr; (*(int *)temp_packet_ptr) >>= 1)
	    ones_check += (*(int *)temp_packet_ptr & 1);
		parityNum=ones_check;
		
		if (parityNum % 2) 
			continue;

		//if check pass go on process
		data.timestamp = time(NULL);				
		data.signbit = temp_packet_ptr->signbit;	
		data.value = temp_packet_ptr->value;

		

		//if the linked list for this sensor does not exsit, then creat one
		if (sensor_array[(int)temp_packet_ptr->id] == NULL) 
			list_alloc(&data_destory,&data_compare,&data_copy, (data_ptr_t)&data);//(function's return value)

		else    //add the data at beginning of list,position->0
			list_insert_at_index(sensor_array[(int)temp_packet_ptr->id], (data_ptr_t)&data, 0);
		    error_handler(err);	

		variable_T = time(NULL);
		
		if ((variable_T - base_T) < day_time)
			continue;
		else
		{
			variable_T = base_T = time(NULL);      //refresh timestamp
			data_compression(&sensor_array);
		}
	}
	
	//if sensor is off line then distory the list array
	destory_array(&sensor_array); 
    
} 
