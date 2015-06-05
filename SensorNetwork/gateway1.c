#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <pthread.h>

#include "config.h"
#include "tcpsocket.h"
#include "list.h"
#include "myqueue.h"

#define INIT_ARRAY_SIZE 1024

#define POINTER_CHECK(ptrcheck,fun) do{ if(ptrcheck) printf("%s() error: invalid parameter...\n",fun); } while(0)


struct list_item {
	data_ptr_t data;
	list_ptr_t prev, next;
}; 
typedef tcp_packet_t LIST_DATA_TYPE; //used for void pointer type cast of the data field in list

struct queue {
	unsigned int front;
	unsigned int rear;
	unsigned int capacity;
	pthread_mutex_t mutex;
	void* data;
};
typedef sensor_data_t QUEUE_DATA_TYPE;  //used for void pointer type cast of the data field in queue   

int port;
int qsize;
float min_temp;
float max_temp;
Queue* cirque;
list_ptr_t sensor_array[INIT_ARRAY_SIZE];

int sensor_check_parity(packet_ptr_t ppt) {
    packet_union_t punion;
    punion.pkt_seg = *ppt;
	//printf("%d\n",punion.pkt_whole);

    int cnt = 0;
    int bits = 32;
    while(bits) {
        cnt += punion.pkt_whole%2;
        punion.pkt_whole /= 2;
        bits--;
    }
    
    return (cnt%2);    
} 

void client_cleanup_if_stopped(void* arg) {
	printf("%s\n","client clean up...");
}

void* client_response(void* arg) {
	printf("%s\n","new client thread created, ");
	//pthread_cleanup_push(client_cleanup_if_stopped,NULL);
	
	Socket client = (Socket)arg;
	int sid;
	while(1) {
		packet_ptr_t ppt = malloc(sizeof(*ppt));
		int bytes = tcp_receive( arg, (void*)ppt, sizeof(*ppt) );  //will only block if connected		
		if(bytes==0) break;   //client has stopped 
		
		time_t tm = time(NULL);
		printf("received message of %d bytes, id: %d temp: %d %s", bytes, ppt->id, ppt->tem, ctime(&tm)); //ctime() string will end with new line
		if( bytes==4 && sensor_check_parity(ppt)==0) {
			//printf("%s\n","parity ok...");
			tcp_pkt_ptr_t tpt = malloc(sizeof(*tpt));
			tpt->pkt = *ppt;
			tpt->tms = tm;
			
			sid = ppt->id;
			int index = list_size(sensor_array[sid]) + 1;
			sensor_array[sid] = list_insert_at_index(sensor_array[sid],(void*)(tpt),index+1);
			//don't free ppt and tpt, the content is also pointed to by sensor_array now
		}	
		sleep(5);
	}
	
	tcp_close( &client );
	list_free_all(sensor_array[sid]);
	sensor_array[sid] = NULL;
	printf("client with sensor ID %d thread finished...\n",sid);
	pthread_exit((void*)("client closed...")); //better to describe client 
}

void* tcp_connect(void* arg) {
	Socket server = tcp_passive_open(port);		
    while( 1 ) {	
		pthread_t resp;
		Socket client = tcp_wait_for_connection(server);
		if(client) {
			pthread_create(&resp,NULL,client_response,(void*)client);
			//void* cltStatus; //returned client status if client connection is closed
			//pthread_join(resp,&cltStatus);	//will block here..
			//printf("%s\n",(char*)cltStatus);
		}
    }

    tcp_close( &server );
    pthread_exit((void*)"tcp connection thread return successfully...");
}

int main(int argv, char* args[]) {
	if(argv!=2) {  
		printf("usage: %s %s\n",args[0],"port");
		exit(EXIT_FAILURE);
	} 

	#ifdef SET_MIN_TEMP
	min_temp = (float)SET_MIN_TEMP;
	
	#ifdef SET_MAX_TEMP
	max_temp = (float)SET_MAX_TEMP;
	#else 
	#error "SET_MIN_TEMP and SET_MAX_TEMP should be given as preprocessor directives."
	#endif
	
	#else 
	#error "SET_MIN_TEMP and SET_MAX_TEMP should be given as preprocessor directives."
	#endif
	
	/* already defined in myqueue.h
	#ifndef SET_QUEUE_SIZE
	#define SET_QUEUE_SIZE 20
	#endif */
	
	qsize = (int)SET_QUEUE_SIZE;
	port = (int)strtol(args[1],NULL,10);  //server port
	cirque = QueueCreate();
	int i;
	for(i=0; i<INIT_ARRAY_SIZE; i++) {
		sensor_array[i] = list_alloc();
	}
	
	pthread_t pth_tcp_con, pth_data_man, pth_strg_man;
	int err;
	void* retStatus;
	
	if((err=pthread_create(&pth_tcp_con,NULL,tcp_connect,NULL))!=0) {
		printf("can't create tcp connection thread: %s\n",strerror(err));
		exit(EXIT_FAILURE);
	}	
	pthread_join(pth_tcp_con,&retStatus);	
	printf("%s\n",(char*)retStatus);
	
	return 0;
}
