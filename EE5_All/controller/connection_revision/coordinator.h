#ifndef COORDINATOR_H
#define COORDINATOR_H

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>
#include <stdint.h>

typedef unsigned char Byte;
typedef char* String;

uint32_t error coordinator_start(int* child_pid, int* file_descriptor_write, int* file_descriptor_read);

typedef struct {
	uint64_t addr;
	unsigned char len;
	void* msg;   //mqtt message
	time_t ts;
} MQTT, *mqtt_ptr_t;



#endif
