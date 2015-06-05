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
#include <sys/stat.h>
#include <sys/wait.h>
#include <assert.h>
#include "D_list.h"
#include "gateway.h"
#include <pthread.h>
#include "DataServer.h"
#include <netdb.h>
#include <signal.h>
/*-----------------------------------------------------------------------
 * 								define
 * ----------------------------------------------------------------------*/

#define POOlSIZE  5
#define BUFSIZE 1024
#define LOG_FILE "./logfile.txt"
#define CONNMAX 1000
#define BYTES 1024

/*-----------------------------------------------------------------------
 * 							global variable
 * ----------------------------------------------------------------------*/
char buffer[BUFSIZE];
static CThread_pool *pool = NULL; 
pthread_mutex_t log_m;
pthread_mutex_t db_m;
pthread_mutex_t para_m;
char *ROOT;
int listenfd, clients[CONNMAX];
/*-----------------------------------------------------------------------
 * 							main
 * ----------------------------------------------------------------------*/

int main (int argc, char **argv) 
{ 
	 int opt = TRUE;
	 int bytes=0;
	 int j=0,size=0;
	 int master_socket , addrlen , new_socket , client_socket[30] , max_clients = 30 , activity, i,ret , valread , s,app_socket,new_app_socket,para_socket,new_para_socket,http_socket,new_http_socket;
	 struct sockaddr_in address;
	 int *workingnum = NULL;
	 char msg[1024];
	 FILE *fp;
	 pthread_mutex_init(&log_m,NULL);
     pthread_mutex_init(&db_m,NULL);
     pthread_mutex_init(&para_m,NULL);
     argument_ptr arge;
     arge = (argument_ptr)malloc(sizeof(argument_1));
	 //set of socket descriptors
	 fd_set readfds;
	 //a message
	 char *message = "Hello new client!THis is Server.\n";
	 Socket server,client,server_app,client_app,server_para,client_para,server_http,client_http;
	
	 pool_init (POOlSIZE);/*set five threads that can active*/
	 server = tcp_passive_open( PORT1);
	 server_app = tcp_passive_open(PORT2);
	 server_para = tcp_passive_open(PORT3);
	 server_http = tcp_passive_open(PORT4);
	 
	 master_socket = get_socket_descriptor(server);
	 app_socket = get_socket_descriptor(server_app);
	 para_socket = get_socket_descriptor(server_para);
	 http_socket = get_socket_descriptor(server_http);
	 
	 //initialise all client_socket[] to 0 so not checked
	 for (i = 0; i < max_clients; i++) 
	 {
		client_socket[i] = 0;
	 }
      
     addrlen = sizeof(address);
	 //LOG -GATEWAY-OPEN
	 
	 fp = openFile(LOG_FILE,"a");
	 logkeep = LOG_GATEOPEN;
	 log_handler(fp,logkeep,server);
	 closeFile(fp);
	 
	 while(1){
	    
		//clear the socket set
		FD_ZERO(&readfds);
		//add master socket to set
		FD_SET(master_socket, &readfds);
		FD_SET(app_socket,&readfds);
		FD_SET(para_socket,&readfds);
		FD_SET(http_socket,&readfds);
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
			
			//LOG new sensor log in
			pthread_mutex_lock(&log_m);
			fp = openFile(LOG_FILE,"a");
			logkeep = LOGIN_IP_PORT;
			log_handler(fp,logkeep,client);
			closeFile(fp);
			pthread_mutex_unlock(&log_m);
			
			//send new connection greeting message
			arge->client = client;
			arge->log_m = log_m;
			arge->db_m = db_m;
			tcp_send( client, message, BUFSIZE);
			pool_add_worker (myprocess, (void*)arge); 



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
				client_app = tcp_wait_for_connection( server_app );
				new_app_socket = get_socket_descriptor(client_app);
				
				printf("Client APP , socket fd is %d , ip is : %s , port : %d \n" ,new_app_socket , get_ip_addr(client_app ) , get_port( client_app));
			
				//client app login
				pthread_mutex_lock(&log_m);
				fp = openFile(LOG_FILE,"a");
				logkeep = LOG_CLIENT_APP;
				log_handler(fp,logkeep,client_app);
				closeFile(fp);
				pthread_mutex_unlock(&log_m);
			
				//send welcome message
				tcp_send( client_app, message, BUFSIZE);
			
				fp = fopen("logfile.txt","r");
			 
				if(fp==NULL){
			  
				printf("failed open!");
			  
				}
				else{
			
				printf("file open success! \n");
				} 
				//if client_app is ready
				if((ret = tcp_receive (client_app, buffer, BUFSIZE))>0){

					fflush(stdout);
  
					//begin to send file to client_app
					do{
				
					size=fread(msg,sizeof(char),1024,fp);	   
					printf("reading...");
					// send msg to client_app
					tcp_send( client_app, (void *)msg, size);

					memset(msg,0,BUFSIZE);
					}while((size=fread(msg,sizeof(char),1024,fp))>0 && (ret = tcp_receive (client_app, buffer, BUFSIZE))>0);
				
				}

				printf("\n");  
			  
				fclose(fp);
		
		}
		if (FD_ISSET(http_socket,&readfds)){

				
				struct sockaddr_in clientaddr;
				socklen_t addrlen;
				char c;    
				
				//Default Values PATH = ~/ and PORT=10000
				char PORT[6];
				ROOT = getenv("PWD");
				strcpy(PORT,"10000");

				int slot=0;

				//Parsing the command line arguments
				while ((c = getopt (argc, argv, "p:r:")) != -1)
					switch (c)
					{
						case 'r':
							ROOT = malloc(strlen(optarg));
							strcpy(ROOT,optarg);
							break;
						case 'p':
							strcpy(PORT,optarg);
							break;
						case '?':
							fprintf(stderr,"Wrong arguments given!!!\n");
							exit(1);
						default:
							exit(1);
					}
				
				printf("Server started at port no. %s%s%s with root directory as %s%s%s\n","\033[92m",PORT,"\033[0m","\033[92m",ROOT,"\033[0m");
				// Setting all elements to -1: signifies there is no client connected
				int i;
				for (i=0; i<CONNMAX; i++)
					clients[i]=-1;
				startServer(PORT);

				// ACCEPT connections
				while (1)
				{
					addrlen = sizeof(clientaddr);
					clients[slot] = accept (listenfd, (struct sockaddr *) &clientaddr, &addrlen);

					if (clients[slot]<0)
						error ("accept() error");
					else
					{
						if ( fork()==0 )
						{
							respond(slot);
							exit(0);
						}
					}

					while (clients[slot]!=-1) slot = (slot+1)%CONNMAX;
				}
		
		}
		if (FD_ISSET(para_socket,&readfds)){
		
						
				//one client_app is coming display function
				client_para = tcp_wait_for_connection( server_para );
				new_para_socket = get_socket_descriptor(client_para);
				
				printf("Client APP , socket fd is %d , ip is : %s , port : %d \n" ,new_para_socket , get_ip_addr(client_para ) , get_port( client_para));
			
				//client app login
				pthread_mutex_lock(&log_m);
				fp = openFile(LOG_FILE,"a");
				logkeep = LOG_CLIENT_APP;
				log_handler(fp,logkeep,client_para);
				closeFile(fp);
				pthread_mutex_unlock(&log_m);
			
				//send welcome message
				tcp_send( client_para, message, BUFSIZE);
			
				fp = fopen("parameter.txt","r");
			 
				if(fp==NULL){
			  
				printf("failed open!");
			  
				}
				else{
			
				printf("file open success! \n");
				} 
				//if client_app is ready
				if((ret = tcp_receive (client_para, buffer, BUFSIZE))>0){

					fflush(stdout);
  
					//begin to send file to client_app
					do{
				
					size=fread(msg,sizeof(char),1024,fp);	   
					printf("reading...");
					// send msg to client_app
					tcp_send( client_para, (void *)msg, size);

					memset(msg,0,BUFSIZE);
					}while((size=fread(msg,sizeof(char),1024,fp))>0 && (ret = tcp_receive (client_para, buffer, BUFSIZE))>0);
				
				}

				printf("\n");  
			  
				fclose(fp);
		
		}
		
		//detect the offline
		for (i = 0; i < max_clients; i++) 
		{
		
			s = client_socket[i];
			if (FD_ISSET( s , &readfds)) 
			{
				//Check if it was for closing , and also read the incoming message
				if ((valread = read( s , buffer, BUFSIZE)) == 0)
				{
					//Somebody disconnected , get his details and print
					getpeername(s , (struct sockaddr*)&address , (socklen_t*)&addrlen);
					printf("Host disconnected , ip %s , port %d \n" , inet_ntoa(address.sin_addr) , ntohs(address.sin_port));
					//Close the socket and mark as 0 in list for reuse
					close( s );
					client_socket[i] = 0;
					
					//log sensor log off
					pthread_mutex_lock(&log_m);
					fp = openFile(LOG_FILE,"a");
					logkeep = LOGOFF_IP_PORT;
					log_off(fp,inet_ntoa(address.sin_addr) , ntohs(address.sin_port));
					closeFile(fp);
					pthread_mutex_unlock(&log_m);
					//if sensor is off line then distory the list array
					//if()
					//destory_array(&sensor_array); 
					
				}	
				//Echo back the message that came in
				else
				{
					//set the terminating NULL byte on the end of the data read
					buffer[valread] = '\0';
					tcp_send( client, buffer , BUFSIZE );
				}
			}
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
 
int pool_add_worker (void *(*process)(void *arg), void *arg) 
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
     printf ("starting thread 0x%x\n", (unsigned int)pthread_self ()); 
	 while (1) 
     { 
         pthread_mutex_lock (&(pool->queue_lock)); 
         /*if the waiting list is 0 and the pool is not destroyed,then it is 
         in the block model;pthread_cond_wait is a kind of enter for the mutex*/ 
         
         while (pool->cur_queue_size == 0 && !pool->shutdown) 
         { 
             printf ("thread 0x%x is waiting\n", (unsigned int)pthread_self ()); 
             pthread_cond_wait (&(pool->queue_ready), &(pool->queue_lock)); 
         } 


          /*the thread pool going to be destroyed*/ 
          if (pool->shutdown) 
          { 
            /*unlock*/ 
             pthread_mutex_unlock (&(pool->queue_lock)); 
             printf ("thread 0x%x will exit\n", (unsigned int)pthread_self ()); 
             pthread_exit (NULL); 
          } 


         printf ("thread 0x%x is starting to work\n",(unsigned int) pthread_self ()); 


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
void startServer(char *port)
{
    struct addrinfo hints, *res, *p;

    // getaddrinfo for host
    memset (&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    if (getaddrinfo( NULL, port, &hints, &res) != 0)
    {
        perror ("getaddrinfo() error");
        exit(1);
    }
    // socket and bind
    for (p = res; p!=NULL; p=p->ai_next)
    {
        listenfd = socket (p->ai_family, p->ai_socktype, 0);
        if (listenfd == -1) continue;
        if (bind(listenfd, p->ai_addr, p->ai_addrlen) == 0) break;
    }
    if (p==NULL)
    {
        perror ("socket() or bind()");
        exit(1);
    }

    freeaddrinfo(res);

    // listen for incoming connections
    if ( listen (listenfd, 1000000) != 0 )
    {
        perror("listen() error");
        exit(1);
    }
}

//client connection
void respond(int n)
{
    char mesg[99999], *reqline[3], data_to_send[BYTES], path[99999];
    int rcvd, fd, bytes_read;

    memset( (void*)mesg, (int)'\0', 99999 );

    rcvd=recv(clients[n], mesg, 99999, 0);

    if (rcvd<0)    // receive error
        fprintf(stderr,("recv() error\n"));
    else if (rcvd==0)    // receive socket closed
        fprintf(stderr,"Client disconnected upexpectedly.\n");
    else    // message received
    {
        printf("%s", mesg);
        reqline[0] = strtok (mesg, " \t\n");
        if ( strncmp(reqline[0], "GET\0", 4)==0 )
        {
            reqline[1] = strtok (NULL, " \t");
            reqline[2] = strtok (NULL, " \t\n");
            if ( strncmp( reqline[2], "HTTP/1.0", 8)!=0 && strncmp( reqline[2], "HTTP/1.1", 8)!=0 )
            {
                write(clients[n], "HTTP/1.0 400 Bad Request\n", 25);
            }
            else
            {
                if ( strncmp(reqline[1], "/\0", 2)==0 )
                    reqline[1] = "/index.html";        //Because if no file is specified, index.html will be opened by default (like it happens in APACHE...

                strcpy(path, ROOT);
                strcpy(&path[strlen(ROOT)], reqline[1]);
                printf("file: %s\n", path);

                if ( (fd=open(path, O_RDONLY))!=-1 )    //FILE FOUND
                {
                    send(clients[n], "HTTP/1.0 200 OK\n\n", 17, 0);
                    while ( (bytes_read=read(fd, data_to_send, BYTES))>0 )
                        write (clients[n], data_to_send, bytes_read);
                }
                else    write(clients[n], "HTTP/1.0 404 Not Found\n", 23); //FILE NOT FOUND
            }
        }
    }

    //Closing SOCKET
    shutdown (clients[n], SHUT_RDWR);         //All further send and recieve operations are DISABLED...
    close(clients[n]);
    clients[n]=-1;
}
