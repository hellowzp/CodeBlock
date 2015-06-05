#include <stdio.h>
#include <stdlib.h>
#include "myqueue.h"

int main() {
	Queue* q = QueueCreate();
	Enqueue(q,12);
	printf("queue size:%d top:%d\n",QueueSize(q), *QueueTop(q));
	QueueDestroy(&q);
	return 0;
}
