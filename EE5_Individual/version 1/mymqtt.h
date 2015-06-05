#ifndef MYMQTT_H
#define MYMQTT_H

typedef struct {
	void* msg;   //mqtt message
	char* addr;
} MQTT, *mqtt_ptr_t;

#endif