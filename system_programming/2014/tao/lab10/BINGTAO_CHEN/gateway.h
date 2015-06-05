#ifndef GATEWAY
#define GATEWAY


#include "D_list.h"
#include <time.h>
/*-------------------------------------------------
 *                definitions
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
    unsigned value:12;
    unsigned signbit:1;
    time_t timestamp;
}data_t;

#define day_time 600   //10 minutes

#define number_of_sensors 1024   //max number of sensors
/*----------------------------------------------------
 * function prototype
 * --------------------------------------------------*/
void init_array(list_ptr_t **);
void destory_array(list_ptr_t **);
void data_compression(list_ptr_t **);
void error_handler(err_code);
data_ptr_t data_copy(data_ptr_t data);
int data_compare(data_ptr_t d1, data_ptr_t d2);
void data_destory(data_ptr_t data);

#endif
