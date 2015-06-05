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
*线程池里所有运行和等待的任务都是一个CThread_worker 
*由于所有任务都在链表里，所以是一个链表结构 
*/ 
typedef struct worker 
{ 
    /*回调函数，任务运行时会调用此函数，注意也可声明成其它形式*/ 
    void *(*process) (void *arg); 
    void *arg;/*回调函数的参数*/ 
    struct worker *next; 


} CThread_worker; 




/*线程池结构*/ 
typedef struct 
{ 
     pthread_mutex_t queue_lock; 
     pthread_cond_t queue_ready; 


    /*链表结构，线程池中所有等待任务*/ 
     CThread_worker *queue_head; 


    /*是否销毁线程池*/ 
    int shutdown; 
     pthread_t *threadid; 
    /*线程池中允许的活动线程数目*/ 
    int max_thread_num; 
    /*当前等待队列的任务数目*/ 
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




/*向线程池中加入任务*/ 
int pool_add_worker (void *(*process) (void *arg), void *arg) 
{ 
    /*构造一个新任务*/ 
     CThread_worker *newworker = (CThread_worker *) malloc (sizeof (CThread_worker)); 
     newworker->process = process; 
     newworker->arg = arg; 
     newworker->next = NULL;/*别忘置空*/ 


     pthread_mutex_lock (&(pool->queue_lock)); 
    /*将任务加入到等待队列中*/ 
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
    /*好了，等待队列中有任务了，唤醒一个等待线程； 
     注意如果所有线程都在忙碌，这句没有任何作用*/ 
     pthread_cond_signal (&(pool->queue_ready)); 
    return 0; 
} 




/*销毁线程池，等待队列中的任务不会再被执行，但是正在运行的线程会一直 
把任务运行完后再退出*/ 
int pool_destroy () 
{ 
    if (pool->shutdown) 
        return -1;/*防止两次调用*/ 
     pool->shutdown = 1; 


    /*唤醒所有等待线程，线程池要销毁了*/ 
     pthread_cond_broadcast (&(pool->queue_ready)); 


    /*阻塞等待线程退出，否则就成僵尸了*/ 
    int i; 
    for (i = 0; i < pool->max_thread_num; i++) 
         pthread_join (pool->threadid[i], NULL); 
     free (pool->threadid); 


    /*销毁等待队列*/ 
     CThread_worker *head = NULL; 
    while (pool->queue_head != NULL) 
     { 
         head = pool->queue_head; 
         pool->queue_head = pool->queue_head->next; 
         free (head); 
     } 
    /*条件变量和互斥量也别忘了销毁*/ 
     pthread_mutex_destroy(&(pool->queue_lock)); 
     pthread_cond_destroy(&(pool->queue_ready)); 
     
     free (pool); 
    /*销毁后指针置空是个好习惯*/ 
     pool=NULL; 
    return 0; 
} 




void * thread_routine (void *arg) 
{ 
     printf ("starting thread 0x%x\n", pthread_self ()); 
    while (1) 
     { 
         pthread_mutex_lock (&(pool->queue_lock)); 
        /*如果等待队列为0并且不销毁线程池，则处于阻塞状态; 注意 
         pthread_cond_wait是一个原子操作，等待前会解锁，唤醒后会加锁*/ 
        while (pool->cur_queue_size == 0 && !pool->shutdown) 
         { 
             printf ("thread 0x%x is waiting\n", pthread_self ()); 
             pthread_cond_wait (&(pool->queue_ready), &(pool->queue_lock)); 
         } 


        /*线程池要销毁了*/ 
        if (pool->shutdown) 
         { 
            /*遇到break,continue,return等跳转语句，千万不要忘记先解锁*/ 
             pthread_mutex_unlock (&(pool->queue_lock)); 
             printf ("thread 0x%x will exit\n", pthread_self ()); 
             pthread_exit (NULL); 
         } 


         printf ("thread 0x%x is starting to work\n", pthread_self ()); 


        /*assert是调试的好帮手*/ 
         assert (pool->cur_queue_size != 0); 
         assert (pool->queue_head != NULL); 
         
        /*等待队列长度减去1，并取出链表中的头元素*/ 
         pool->cur_queue_size--; 
         CThread_worker *worker = pool->queue_head; 
         pool->queue_head = worker->next; 
         pthread_mutex_unlock (&(pool->queue_lock)); 


        /*调用回调函数，执行任务*/ 
         (*(worker->process)) (worker->arg); 
         free (worker); 
         worker = NULL; 
     } 
    /*这一句应该是不可达的*/ 
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
	
	
	 pool_init (5);/*线程池中最多5个活动线程*/
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
			/*连续向池中投入10个任务*/ 
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
     
     
   
    /*等待所有任务完成*/ 
     sleep(5);
  //   pthread_join(tid, NULL);  
     printf("All done -- exiting/n");  

    /*销毁线程池*/ 
     pool_destroy (); 


     free (workingnum); 
 }
    return 0; 
}
