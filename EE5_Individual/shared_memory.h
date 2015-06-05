#ifndef  SHARED_MEMORY_H
#define SHARED_MEMORY_H

#define QUEUE_INITIAL_CAPACITY 20
#define BUSY 1
#define IDLE 0

typedef char* String;
typedef struct {
	String name;
	String value;
} Map, *map_ptr_t;

typedef struct {
	char* address;
	int len;
	map_ptr_t payload;
} Data, *data_ptr_t;

typedef struct{
	int head;
	int tail;
	data_ptr_t queue;
} Queue, *queue_ptr_t;

typedef void (*dsrfp)(queue_ptr_t q,char** buf); //down stream read function pointer, read from queue to fill in the buf
typedef void (*usrfp)(queue_ptr_t,void*); //up stream read function pointer, read from queue to ...


char down_stream_busy;
char up_stream_busy;
 
Queue down_stream_queue, up_stream_queue;


#endif
