#ifndef _CONFIG_H_
#define _CONFIG_H_

#include <stdint.h>
#include <time.h>

#include "list.h"

/*------------------------------------------------------------------------------
 definitions for sensor data
 ------------------------------------------------------------------------------*/

typedef uint16_t sensor_id_t;
typedef int16_t sensor_value_t;     // temp in 0.1C, e.g. 18.3C is stored as 183
typedef time_t sensor_ts_t;         // UTC timestamp as returned by time()

typedef struct{
	sensor_id_t id;
	sensor_value_t value;
	sensor_ts_t ts;
}sensor_data_t, * sensor_data_ptr_t;   //for log file and queue data

typedef struct {
    unsigned id:12;
	unsigned seq:6;
	unsigned sign:1;
	unsigned tem:12;
	unsigned parity:1;       //use even parity: total number of 1 is even
} packet_t, *packet_ptr_t;   //for tcp packet

typedef union {
	packet_t pkt_seg;
	unsigned int pkt_whole;  //align 
} packet_union_t;

typedef struct {
	packet_t pkt;
	time_t tms;
} tcp_packet_t, *tcp_pkt_ptr_t;    //final real list data type

typedef struct {
	uint32_t id;
	uint32_t size;
	list_ptr_t lptr;
} extended_list_t, *xlist_ptr_t;

typedef struct {
	xlist_ptr_t data;
	list_ptr_t prev; 
	list_ptr_t next;
} list_array_t, *list_array_ptr_t;

typedef unsigned char Byte;
typedef Byte* String;	

#endif /* _CONFIG_H_ */

