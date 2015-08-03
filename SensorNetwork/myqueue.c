#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "myqueue.h"
//#include "config.h"

#include <unistd.h>
#include <pthread.h>

#define POINTER_CHECK(ptr_to_check,fun) do{ if(ptr_to_check) { printf("%s() error: invalid parameter...\n",fun); return; } } while(0) 

#ifndef SET_QUEUE_SIZE
#define SET_QUEUE_SIZE 100
#endif

#ifndef MAIN
extern const unsigned int QUEUE_DATA_SIZE;
//extern pthread_mutex_t queue_mutex;
#else	
const unsigned int QUEUE_DATA_SIZE = 4;
//pthread_mutex_t queue_mutex;
#endif

struct queue {
	unsigned int front;
	unsigned int rear;
	unsigned int capacity;
	pthread_mutex_t mutex;
	void* data;
};

Queue* QueueCreate() {
	Queue *q = NULL;
	q = malloc(sizeof(Queue));
	if(q == NULL) { 
		printf("%s\n","allocate memory error...");
		exit(EXIT_FAILURE);
	} 
	q->front = 0;
	q->rear = 0;
	q->capacity = SET_QUEUE_SIZE;
	q->data = calloc(SET_QUEUE_SIZE,QUEUE_DATA_SIZE);
	if(q->data == NULL) { 
		printf("%s\n","allocate memory error...");
		exit(EXIT_FAILURE);
	}
	q->mutex = (pthread_mutex_t)PTHREAD_MUTEX_INITIALIZER;
	return q;
}

void Enqueue(Queue* que, void* element) {
	POINTER_CHECK(!que || !element,"Enqueue");
	//printf("%s\n","lock mutex...");
	//pthread_mutex_lock(&(que->mutex));
	//printf("%s\n","lock mutex succeed...");
	if((que->rear-que->front)>=que->capacity) {
		#ifdef DYNAMIC_QUEUE
		void* newblock = realloc(que->data,(que->capacity+SET_QUEUE_SIZE)*QUEUE_DATA_SIZE);
		if(newblock != NULL) {
			que->data = newblock;
			que->capacity += SET_QUEUE_SIZE;
		} else {
			printf("%s\n","the que is full now and unable to reallocate larger memory");
			pthread_mutex_unlock(&(que->mutex));
			exit(EXIT_FAILURE);
		}
		
		memcpy(que->data + que->rear * QUEUE_DATA_SIZE, element, QUEUE_DATA_SIZE);
		que->rear++;	
		printf("enqueue:%p %p\n",que->data,(que->data)+que->rear*QUEUE_DATA_SIZE);
		
		#else
		printf("%s\n","the que is full now and new data discarded...");		
		#endif
	} else {
		memcpy(que->data + que->rear * QUEUE_DATA_SIZE, element, QUEUE_DATA_SIZE);
		que->rear++;	
		printf("enqueue:%p %p\n",que->data,(que->data)+que->rear*QUEUE_DATA_SIZE);
	}
	
	//pthread_mutex_unlock(&(que->mutex));
}

void QueueDestroy(Queue** que) {
	POINTER_CHECK(!que||!(*que),"QueueDestroy");
	pthread_mutex_lock(&((*que)->mutex));	
	free((*que)->data);  
	pthread_mutex_destroy(&((*que)->mutex));
	free(*que);  //will this unlock the mutex??
	*que=NULL;
	//pthread_mutex_unlock(&((*que)->mutex)); //do this before free the que...
}

int QueueSize(Queue* que) {
	POINTER_CHECK(!que,"QueueSize");
	pthread_mutex_lock(&(que->mutex));
	int size =  que->rear - que->front;
	pthread_mutex_unlock(&(que->mutex));
	return size;
}

void* QueueTop(Queue* que) {
	POINTER_CHECK(!que,"QueueTop");	
	pthread_mutex_lock(&(que->mutex));
	if(que->rear <= que->front) return NULL;
	
	void* data = malloc(QUEUE_DATA_SIZE);
	if(!data) {
		perror("QueueTop malloc");
		pthread_mutex_unlock(&(que->mutex));
		exit(EXIT_FAILURE);
	}
	
	memcpy(data,que->data + que->front*QUEUE_DATA_SIZE, QUEUE_DATA_SIZE);	
	pthread_mutex_unlock(&(que->mutex));
	
	return data;
}

void Dequeue(Queue* que) {
	POINTER_CHECK(!que,"Dequeue");	
	pthread_mutex_lock(&(que->mutex));
	if(que->rear > que->front) {
	    que->front++;
		pthread_mutex_unlock(&(que->mutex));
		return ;
	} else {
		pthread_mutex_unlock(&(que->mutex));
	}
}

#ifdef MAIN
int main() {
	Queue* q = QueueCreate();
	int *p = malloc(sizeof(int));
	*p = 12;
	Enqueue(q,p);
	Enqueue(q,p);
	int i = QueueSize(q);
	int* a = (int*)QueueTop(q);
	printf("queue size:%d top:%d\n",i, *a);
	QueueDestroy(&q);
	free(p);
	return 0;
}  
#endif






