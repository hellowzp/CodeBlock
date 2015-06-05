#include <stdio.h>
#include <stdlib.h>
#include "myqueue.h"
#include "config.h"

#include <unistd.h>
#include <pthread.h>

#define POINTER_CHECK(ptr_to_check,fun) do{ if(ptr_to_check) printf("%s() error: invalid parameter...\n",fun); } while(0) 

typedef int DATATYPE;

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
	} else printf("%s\n","allocate memory successfully...");
	q->front = 0;
	q->rear = 0;
	q->capacity = SET_QUEUE_SIZE;
	q->mutex = (pthread_mutex_t)PTHREAD_MUTEX_INITIALIZER;
	q->data = calloc(SET_QUEUE_SIZE,sizeof(DATATYPE));
	if(q->data == NULL) { 
		printf("%s\n","allocate memory error...");
		exit(EXIT_FAILURE);
	}
	return q;
}

void Enqueue(Queue* queue, void* element) {
	pthread_mutex_lock(&(queue->mutex));
	POINTER_CHECK(!queue || !element,"Enqueue");
	if((queue->rear-queue->front)>=queue->capacity) {
		DATATYPE* d = realloc(queue->data,(queue->capacity+SET_QUEUE_SIZE)*sizeof(DATATYPE));
		if(d != NULL) {
			queue->data = d;
			queue->capacity += SET_QUEUE_SIZE;
		} else {
			printf("%s\n","the queue is full now and unable to reallocate larger memory");
			exit(EXIT_FAILURE);
		}
	}
	*(DATATYPE*)((queue->data)+queue->rear) = *(DATATYPE*)element;
	printf("enqueue:%p %p\n",queue->data,(queue->data)+queue->rear);
	queue->rear++;	
	pthread_mutex_unlock(&(queue->mutex));
}

void QueueDestroy(Queue** queue) {
	pthread_mutex_lock(&((*queue)->mutex));
	if((*queue)->front == (*queue)->rear) return;
	POINTER_CHECK(!queue||!(*queue),"QueueDestroy");
	(*queue)->front = 0;
	(*queue)->rear = 0;
	free((*queue)->data);
	(*queue)->data = NULL;
	free(*queue);
	*queue=NULL;
	pthread_mutex_unlock(&((*queue)->mutex));
}

int QueueSize(Queue* queue) {
	pthread_mutex_lock(&(queue->mutex));
	POINTER_CHECK(!queue,"QueueSize");
	int size =  queue->rear - queue->front;
	pthread_mutex_unlock(&(queue->mutex));
	return size;
}

void* QueueTop(Queue* queue) {
	pthread_mutex_lock(&(queue->mutex));
	POINTER_CHECK(!queue,"QueueTop");
	void* data = queue->data + queue->front;
	pthread_mutex_unlock(&(queue->mutex));
	return data;
}

void Dequeue(Queue* queue) {
	pthread_mutex_lock(&(queue->mutex));
	POINTER_CHECK(!queue,"Dequeue");	
	if(queue->rear > queue->front) {
	    queue->front++;
		pthread_mutex_unlock(&(queue->mutex));
		return ;
	}
	//printf("The queue is already empty..\n");
}

int main() {
	Queue* q = QueueCreate();
	Enqueue(q,&12);
	Enqueue(q,&12);
	printf("queue size:%d top:%p\n",QueueSize(q), QueueTop(q));
	QueueDestroy(&q);
	return 0;
}  



