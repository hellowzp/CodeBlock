#ifndef GATEWAY
#define GATEWAY


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
#include <pthread.h>
/*-------------------------------------------------
 *                definitions
---------------------------------------------------*/



#define BUFSIZE 1024
#define PORT1 1234
#define PORT2 2345
#define PORT3 3456
#define PORT4 4567
#define TRUE 1
#define FALSE 0


 char buffer[BUFSIZE];
 /* 
in the thread pool,every task is a  CThread_work;
*/ 
typedef struct worker 
{ 
    /*echo function*/ 
    void *(*process) (void *arg); 
    void *arg;/*arguments of echo function*/ 
    struct worker *next; 

} CThread_worker; 



/*struct of Thread pool*/ 
typedef struct 
{ 
     pthread_mutex_t queue_lock; 
     pthread_cond_t queue_ready; 


    /*all the task which is waiting*/ 
     CThread_worker *queue_head; 


    /*if destroy the thread pool*/ 
    int shutdown; 
    pthread_t *threadid; 
    /*max number allowed for the thread*/ 
    int max_thread_num; 
    /*the number of waiting task*/ 
    int cur_queue_size; 


} CThread_pool; 
/*----------------------------------------------------
 * function prototype
 * --------------------------------------------------*/
int pool_add_worker (void *(*process) (void *arg), void *arg); 
void *thread_routine (void *arg); 
void pool_init (int max_thread_num);
int pool_destroy () ;
void error(char *);
void startServer(char *);
void respond(int);
#endif
