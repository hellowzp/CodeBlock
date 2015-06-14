#ifndef GATEWAY
#define GATEWAY

#include "linkedlist.h"
#include <time.h>


/*-------------------------------------------------
 *          data    definitions
---------------------------------------------------*/

typedef struct sensor_packet{
	unsigned id:12;
	unsigned seq:4;
	unsigned flag:2;
	unsigned signbit:1;
	unsigned value:12;
	unsigned parity:1;
}sensor_packet,*packet_ptr_t; 

typedef struct data_t{  
	unsigned id:12;                         
    unsigned value:12;
    unsigned signbit:1;
    time_t timestamp;
}data_t;


#define sensor_number 4096

/*----------------------------------------------------
 * function prototype
 * --------------------------------------------------*/
int parityCheck(packet_ptr_t  dataPacket);
/*
void init_array(list_ptr_t **);
void destory_array(list_ptr_t **);
void data_compression(list_ptr_t **);
* */
//void error_handler(err_code);
data_ptr_t data_copy(data_ptr_t data);
int data_compare(data_ptr_t d1, data_ptr_t d2);
void data_destory(data_ptr_t data);

#endif
