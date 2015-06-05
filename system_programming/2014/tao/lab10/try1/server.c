#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <sys/types.h> 
#include <pthread.h> 
#include <assert.h>

#define BUFSIZE 1024
#define PORT 1234
#define TRUE 1
#define FALSE 0


unsigned char buffer[BUFSIZE];
/* 
*�̳߳����������к͵ȴ���������һ��CThread_worker 
*�������������������������һ������ṹ 
*/ 
typedef struct worker 
{ 
    /*�ص���������������ʱ����ô˺�����ע��Ҳ��������������ʽ*/ 
    void *(*process) (void *arg); 
    void *arg;/*�ص������Ĳ���*/ 
    struct worker *next; 


} CThread_worker; 




/*�̳߳ؽṹ*/ 
typedef struct 
{ 
     pthread_mutex_t queue_lock; 
     pthread_cond_t queue_ready; 


    /*����ṹ���̳߳������еȴ�����*/ 
     CThread_worker *queue_head; 


    /*�Ƿ������̳߳�*/ 
    int shutdown; 
     pthread_t *threadid; 
    /*�̳߳�������Ļ�߳���Ŀ*/ 
    int max_thread_num; 
    /*��ǰ�ȴ����е�������Ŀ*/ 
    int cur_queue_size; 


} CThread_pool; 




int pool_add_worker (void *(*process) (void *arg), void *arg); 
void *thread_routine (void *arg); 




static CThread_pool *pool = NULL; 
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




/*���̳߳��м�������*/ 
int pool_add_worker (void *(*process) (void *arg), void *arg) 
{ 
    /*����һ��������*/ 
     CThread_worker *newworker = (CThread_worker *) malloc (sizeof (CThread_worker)); 
     newworker->process = process; 
     newworker->arg = arg; 
     newworker->next = NULL;/*�����ÿ�*/ 


     pthread_mutex_lock (&(pool->queue_lock)); 
    /*��������뵽�ȴ�������*/ 
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
    /*���ˣ��ȴ��������������ˣ�����һ���ȴ��̣߳� 
     ע����������̶߳���æµ�����û���κ�����*/ 
     pthread_cond_signal (&(pool->queue_ready)); 
    return 0; 
} 




/*�����̳߳أ��ȴ������е����񲻻��ٱ�ִ�У������������е��̻߳�һֱ 
����������������˳�*/ 
int pool_destroy () 
{ 
    if (pool->shutdown) 
        return -1;/*��ֹ���ε���*/ 
     pool->shutdown = 1; 


    /*�������еȴ��̣߳��̳߳�Ҫ������*/ 
     pthread_cond_broadcast (&(pool->queue_ready)); 


    /*�����ȴ��߳��˳�������ͳɽ�ʬ��*/ 
    int i; 
    for (i = 0; i < pool->max_thread_num; i++) 
         pthread_join (pool->threadid[i], NULL); 
     free (pool->threadid); 


    /*���ٵȴ�����*/ 
     CThread_worker *head = NULL; 
    while (pool->queue_head != NULL) 
     { 
         head = pool->queue_head; 
         pool->queue_head = pool->queue_head->next; 
         free (head); 
     } 
    /*���������ͻ�����Ҳ����������*/ 
     pthread_mutex_destroy(&(pool->queue_lock)); 
     pthread_cond_destroy(&(pool->queue_ready)); 
     
     free (pool); 
    /*���ٺ�ָ���ÿ��Ǹ���ϰ��*/ 
     pool=NULL; 
    return 0; 
} 




void * thread_routine (void *arg) 
{ 
     printf ("starting thread 0x%x\n", pthread_self ()); 
    while (1) 
     { 
         pthread_mutex_lock (&(pool->queue_lock)); 
        /*����ȴ�����Ϊ0���Ҳ������̳߳أ���������״̬; ע�� 
         pthread_cond_wait��һ��ԭ�Ӳ������ȴ�ǰ����������Ѻ�����*/ 
        while (pool->cur_queue_size == 0 && !pool->shutdown) 
         { 
             printf ("thread 0x%x is waiting\n", pthread_self ()); 
             pthread_cond_wait (&(pool->queue_ready), &(pool->queue_lock)); 
         } 


        /*�̳߳�Ҫ������*/ 
        if (pool->shutdown) 
         { 
            /*����break,continue,return����ת��䣬ǧ��Ҫ�����Ƚ���*/ 
             pthread_mutex_unlock (&(pool->queue_lock)); 
             printf ("thread 0x%x will exit\n", pthread_self ()); 
             pthread_exit (NULL); 
         } 


         printf ("thread 0x%x is starting to work\n", pthread_self ()); 


        /*assert�ǵ��Եĺð���*/ 
         assert (pool->cur_queue_size != 0); 
         assert (pool->queue_head != NULL); 
         
        /*�ȴ����г��ȼ�ȥ1����ȡ�������е�ͷԪ��*/ 
         pool->cur_queue_size--; 
         CThread_worker *worker = pool->queue_head; 
         pool->queue_head = worker->next; 
         pthread_mutex_unlock (&(pool->queue_lock)); 


        /*���ûص�������ִ������*/ 
         (*(worker->process)) (worker->arg); 
         free (worker); 
         worker = NULL; 
     } 
    /*��һ��Ӧ���ǲ��ɴ��*/ 
     pthread_exit (NULL); 
} 




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

	init_array(&sensor_array); //initialization
	
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
			list_alloc(&data_destory, &data_compare, &data_copy, (data_ptr_t)&data);//(function's return value)

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

	destory_array(&sensor_array); 
    return NULL; 
} 


int main (int argc, char **argv) 
{ 
	 int opt = TRUE;
	 int j=0;
	 int master_socket , addrlen , new_socket , client_socket[30] , max_clients = 30 , activity, i , valread , s;
	 struct sockaddr_in address;
//	 char buffer[1024]; //data buffer of 1K
	 //set of socket descriptors
	 fd_set readfds;
	 //a message
	 char *message = "ECHO Daemon v1.0 \r\n";
	 Socket server;
	
	
	 pool_init (5);/*�̳߳������5����߳�*/
	 server = tcp_passive_open( PORT);
	 master_socket = get_socket_descriptor(server);
	
	
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
			
			//tcp_close(&serv1);
			//inform user of socket number - used in send and receive commands
			printf("New connection , socket fd is %d , ip is : %s , port : %d \n" , sd_1 , get_ip_addr(client ) , get_port( client));
			
			
			//send new connection greeting message
			tcp_send( client, &message, bytes );
			/*���������Ͷ��10������*/ 
			int *workingnum = (int *) malloc (sizeof (int) * 10); 

		
			workingnum[j] = j; 
			pool_add_worker (myprocess, &workingnum[j]); 
			j++; 
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
	    
/*	    
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
				getpeername(s , (struct sockaddr*)&(get_ip_addr(client )) , (socklen_t*)&addrlen);
				printf("Host disconnected , ip %s , port %d \n" , get_ip_addr(client ) , get_port( client));
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
	}                                      ----------------*/
     
     
   
    /*�ȴ������������*/ 
     sleep(5);
  //   pthread_join(tid, NULL);  
     printf("All done -- exiting/n");  

    /*�����̳߳�*/ 
     pool_destroy (); 


     free (workingnum); 
 }
    return 0; 
}
