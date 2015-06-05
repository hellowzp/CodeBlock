#ifndef THREAD
#define THREAD

#include <pthread.h>
#include <assert.h>
#include "tcpsocket.h"

//a struct to store information about server,client,socket descriptor 
typedef struct clientechoarg    
{
  Socket myserver;
  Socket myclient;
  int mysd;		  
}clientEchoArg;

//structure for task in thread
typedef struct worker
{
  void *(*process) (void *arg);  //task
  void *arg;                    //argument for function
  struct worker *next;          
} CThread_worker;


//thread pool
typedef struct
{
  pthread_mutex_t queue_lock;
  pthread_cond_t queue_ready;
  
  CThread_worker *queue_head;

//flag to cancell thread
  int shutdown;
  pthread_t *threadid;
//maxim thread number in pool
  int max_thread_num;
//current waiting threads
  int cur_queue_size;
} CThread_pool;


extern CThread_pool *pool;


void pool_init (int max_thread_num);                                     //initialize thread pool
int pool_add_worker(void *(*process) (void *arg), void *arg);            //add thread tasks in pool
int pool_destroy();  													//destroy pool
void * thread_routine(void *arg);                                                      


#endif
