#ifndef MYMQTT_H
#define MYMQTT_H

#include <stdint.h>
#include <time.h>

typedef struct {
	uint64_t addr;
	unsigned char len;
	void* msg;   //mqtt message
	time_t ts;
} MQTT, *mqtt_ptr_t;

#endif