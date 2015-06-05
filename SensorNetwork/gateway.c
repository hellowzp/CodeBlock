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

#define LIST_ARRAY_SIZE 100

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
list_ptr_t snptr; //sensor node ptr, each list data points to an array of list with size LIST_ARRAY_SIZE

pthread_t pth_tcp_con, pth_data_man, pth_strg_man;
pthread_mutex_t list_mtx, queue_mtx;

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
	int list_array_index;    //which list array block,
	int list_array_rindex;   //list array row index
	int list_array_cindex;   //column index to insert

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
			list_array_index  = sid/100; //which list array block,
			list_array_rindex = sid%100;     //array row index

			//first check the number of allocated list array in case of SEGERROR
			//lock it since it's reading a shared list
			pthread_mutex_lock(&list_mtx);
			int nr_list_array = list_size(snptr);  //min size is 1 after initialization
			list_ptr_t it = snptr;
			while(list_array_index >= nr_list_array) {
                list_ptr_t nlist = list_alloc();
                nlist->prev = it;
                nlist->data = (list_ptr_t*)malloc(LIST_ARRAY_SIZE * sizeof(list_ptr_t));
                it->next = nlist;
                int i;
                for(i=0; i<LIST_ARRAY_SIZE; i++) {
                    nlist->data[i] = list_alloc();
                }
                it = it->next;
                nr_list_array++;
			}
			pthread_mutex_unlock(&list_mtx);
			//unlock here since the following list is owned by only this thread

			list_ptr_t rlist = (snptr+list_array_index)->data[list_array_rindex];
			list_array_cindex = list_size(rlist) + 1;   //column index to insert
			rlist = list_insert_at_index(rlist,(void*)(tpt),list_array_cindex);
			//since append will not change the beginning address of list,
			//so not necessary to assign rlist to ..;

			//don't free ppt and tpt, the content is also pointed to by the sensor array now
		}
		sleep(5);
	}

	tcp_close( &client );
	list_free_all((snptr+list_array_index)->data[list_array_rindex]);
	(snptr+list_array_index)->data[list_array_rindex] = NULL;
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

	snptr = list_alloc();
	snptr->data = (list_ptr_t*)malloc(LIST_ARRAY_SIZE * sizeof(list_ptr_t));
	int i;
	for(i=0; i<LIST_ARRAY_SIZE; i++) {
		snptr->data[i] = list_alloc();
	}

	int err;
	void* retStatus;

	pthread_mutex_init(&list_mtx,NULL);
	pthread_mutex_init(&queue_mtx,NULL);

	if((err=pthread_create(&pth_tcp_con,NULL,tcp_connect,NULL))!=0) {
		printf("can't create tcp connection thread: %s\n",strerror(err));
		exit(EXIT_FAILURE);
	}
	pthread_join(pth_tcp_con,&retStatus);
	printf("%s\n",(char*)retStatus);

	return 0;
}
