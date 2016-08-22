#include <stdlib.h>
#include <pthread.h>
#include <stdio.h>
#include <errno.h>
#include <time.h>
#include "config.h"
#include "sbuffer.h"

/*
 * I got this idea of Two lock sbuffer data structure and operations from a paper
 * writen by Maged M. Michael and  Michael L. Scott "Simple, Fast, and Practical
 * Non-Blocking and Blocking Concurrent Queue Algorithms".
 *
 *	http://www.research.ibm.com/people/m/michael/podc-1996.pdf
 */

typedef struct sbuffer_node {
  struct sbuffer_node * next;
  sbuffer_data_t data;
} sbuffer_node_t;

struct sbuffer {
  sbuffer_node_t * head;
  sbuffer_node_t * read_head;
  sbuffer_node_t * tail;
	//pthread_mutex_t buffer_mutex;
	pthread_mutex_t head_mutex;
	pthread_mutex_t tail_mutex;
	pthread_cond_t readable_cond;
	pthread_cond_t removable_cond;
	int read_only;
};	


int sbuffer_init(sbuffer_t ** buffer)
{
  *buffer = malloc(sizeof(sbuffer_t));
  if (*buffer == NULL) return SBUFFER_FAILURE;

	sbuffer_node_t * dummy_node = NULL; 
	dummy_node = malloc(sizeof(*dummy_node));
	if (dummy_node == NULL) return SBUFFER_FAILURE;
	dummy_node->next = NULL;
	
  (*buffer)->head = dummy_node; //Both head and tail point to the dummy_node,
  (*buffer)->read_head = dummy_node; //Both head and tail point to the dummy_node,
  (*buffer)->tail = dummy_node;
	pthread_mutex_init(&((*buffer)->head_mutex) , NULL);
	pthread_mutex_init(&((*buffer)->tail_mutex) , NULL);
	pthread_cond_init(&((*buffer)->readable_cond), NULL);
	pthread_cond_init(&((*buffer)->removable_cond), NULL);
	//(*buffer)->readable_cond_flag = 0;
  return SBUFFER_SUCCESS; 
}


int sbuffer_free(sbuffer_t ** buffer)
{
  if ((buffer==NULL) || (*buffer==NULL)) 
  {
    return SBUFFER_FAILURE;
  } 
  while ( (*buffer)->head )  // free all the nodes in sbuffer including the dummy_node.
  {
  	sbuffer_node_t * dummy;
    dummy = (*buffer)->head;
    (*buffer)->head = (*buffer)->head->next;
    free(dummy);
  }
	pthread_mutex_destroy(&((*buffer)->head_mutex));
	pthread_mutex_destroy(&((*buffer)->tail_mutex));
	pthread_cond_destroy(&((*buffer)->readable_cond));
  free(*buffer);
  *buffer = NULL;
  return SBUFFER_SUCCESS;		
}


int sbuffer_remove(sbuffer_t * buffer,sbuffer_data_t * data, int timeout, int read_only)
{
  sbuffer_node_t * dummy;
	int retval;
	struct timespec abstime;

  if (buffer == NULL) return SBUFFER_FAILURE;
	//printf("buffer->readable_cond_flag = %d\n", buffer->readable_cond_flag);
	
	if (read_only == 0)  //free the node
	{
		pthread_mutex_lock(&(buffer->head_mutex));
		//while(buffer->readable_cond_flag != 1)
		dummy = buffer->head;
		while(dummy->next == buffer->read_head->next) // sbuffer is empty.
		{
			abstime.tv_sec = time(NULL) + (time_t)timeout;
			abstime.tv_nsec = 0;
			retval = pthread_cond_timedwait(&(buffer->removable_cond), &(buffer->head_mutex), &abstime);
			if(retval == ETIMEDOUT)
			{
				pthread_mutex_unlock(&(buffer->head_mutex));
				return SBUFFER_NO_DATA;
			}
		}
		
		*data = dummy->next->data;
		buffer->head = dummy->next;
		pthread_mutex_unlock(&(buffer->head_mutex));
		free(dummy);
	}
	else       //read the node
	{
		pthread_mutex_lock(&(buffer->head_mutex));
		//while(buffer->readable_cond_flag != 1)
		dummy = buffer->read_head;
		while(dummy->next == NULL) // sbuffer is empty.
		{
			abstime.tv_sec = time(NULL) + (time_t)timeout;
			abstime.tv_nsec = 0;
			retval = pthread_cond_timedwait(&(buffer->readable_cond), &(buffer->head_mutex), &abstime);
			if(retval == ETIMEDOUT)
			{
				pthread_mutex_unlock(&(buffer->head_mutex));
				return SBUFFER_NO_DATA;
			}
		}

		*data = dummy->next->data;
		buffer->read_head = dummy->next;

		pthread_cond_broadcast(&(buffer->removable_cond));
		pthread_mutex_unlock(&(buffer->head_mutex));
	}
/*
  if (buffer->head == buffer->tail) // buffer has only one node
  {
    buffer->head = buffer->tail = NULL; 
		
  }
*/	
/*
  else  // buffer has many nodes empty
  {
    buffer->head = buffer->head->next;
  }
  if (buffer->head == NULL) 
	{
		buffer->readable_cond_flag = 0;
	}
	pthread_mutex_unlock(&(buffer->buffer_mutex));
  free(dummy);
*/
	
  return SBUFFER_SUCCESS;
}

int sbuffer_insert(sbuffer_t * buffer, sbuffer_data_t * data)
{
  sbuffer_node_t * dummy;
  if (buffer == NULL) return SBUFFER_FAILURE;
  dummy = malloc(sizeof(sbuffer_node_t));
  if (dummy == NULL) return SBUFFER_FAILURE;
  dummy->data = *data;
  dummy->next = NULL;
	pthread_mutex_lock(&(buffer->tail_mutex)); // acquire tail_mutex
	buffer->tail->next = dummy;
	buffer->tail = dummy;
/*	
  if (buffer->tail == NULL) // buffer empty (buffer->head should also be NULL
  {
    buffer->head = buffer->tail = dummy;
  } 
  else // buffer not empty
  {
    buffer->tail->next = dummy;
    buffer->tail = buffer->tail->next; 
  }
*/	
	//buffer->readable_cond_flag = 1;
	pthread_cond_broadcast(&(buffer->readable_cond));
//	puts("broadcast\n");
	pthread_mutex_unlock(&(buffer->tail_mutex));
  return SBUFFER_SUCCESS;
}



