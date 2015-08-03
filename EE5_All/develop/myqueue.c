#include <stdio.h>
#include <stdlib.h>
#include "myqueue.h"

#include <unistd.h>
#include <pthread.h>

#define DEBUG
#define QUEUE_INITIAL_CAPACITY 10

#define POINTER_CHECK(ptrcheck,fun) do{ if(ptrcheck) printf("%s\n","fun() error: invalid parameter..."); } while(0) 

typedef unsigned int unint;

struct queue {
	unint front;
	unint rear;
	unint capacity;
	pthread_mutex_t mutex;
	DATATYPE* data;
};


Queue* QueueCreate() {
	Queue *q = NULL;
	q = malloc(sizeof(Queue));
	q->front = 0;
	q->rear = 0;
	q->capacity = QUEUE_INITIAL_CAPACITY;
	q->mutex = (pthread_mutex_t)PTHREAD_MUTEX_INITIALIZER;
	q->data = malloc(QUEUE_INITIAL_CAPACITY * sizeof(DATATYPE));
	return q;
}

void Enqueue(Queue* queue, DATATYPE element) {
	pthread_mutex_lock(&(queue->mutex));
	POINTER_CHECK(!queue,Enqueue);
	if((queue->rear-queue->front)>=queue->capacity) {
		DATATYPE* d = realloc(queue->data,(queue->capacity+QUEUE_INITIAL_CAPACITY)*sizeof(DATATYPE));
		if(d != NULL) {
			queue->data = d;
			queue->capacity += QUEUE_INITIAL_CAPACITY;
		} else {
			printf("%s\n","the queue is full now and unable to reallocate larger memory");
		}
	}
	queue->data[queue->rear] = element;
	queue->rear++;	
	pthread_mutex_unlock(&(queue->mutex));
}

void QueueDestroy(Queue** queue) {
	pthread_mutex_lock(&((*queue)->mutex));
	if((*queue)->front == (*queue)->rear) return;
	POINTER_CHECK(!queue||!(*queue),QueueDestroy);
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
	POINTER_CHECK(!queue,QueueSize);
	int size =  queue->rear - queue->front;
	pthread_mutex_unlock(&(queue->mutex));
	return size;
}

DATATYPE* QueueTop(Queue* queue) {
	pthread_mutex_lock(&(queue->mutex));
	POINTER_CHECK(!queue,QueueTop);
	DATATYPE* data = &(queue->data[queue->front]);
	pthread_mutex_unlock(&(queue->mutex));
	return data;
}

void Dequeue(Queue* queue) {
	pthread_mutex_lock(&(queue->mutex));
	POINTER_CHECK(!queue,Dequeue);	
	if(queue->rear > queue->front) {
	    queue->front++;
		pthread_mutex_unlock(&(queue->mutex));
		return ;
	}
	printf("The queue is already empty..\n");
}

int main() {
	Queue* q = QueueCreate();
	//Enqueue(q,12);
	printf("queue size:%d top:%p\n",QueueSize(q), QueueTop(q));
	QueueDestroy(&q);
	return 0;
}



