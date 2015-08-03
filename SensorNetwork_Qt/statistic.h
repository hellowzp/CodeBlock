#ifndef STATISTIC
#define STATISTIC

#include "config.h"
#include "gateway.h"
#include "linkedlist.h"

#define BUFSIZE 1024
#define CHAR_IP_ADDR_LENGTH 16     // 4 numbers of 3 digits, 3 dots and \0
#define checkInterval 200
#define Sensor_Number 4096

extern char buffer0[BUFSIZE];
extern  data_t sensor_data;
extern  packet_ptr_t sensor_packet_temp;

extern  list_ptr_t  list_buff;
extern  list_ptr_t *all_sensor_list;

extern int *IDFlagCheck;    //check this sensor is active and just starts to send data
extern int *list_index;   //index in the array of time interval
extern int *tempAver;

extern int myIndex;
extern int data_buffer;

extern sensor_id_t * idptr;
extern sensor_value_t * valueptr;
extern sensor_ts_t * tsptr;

extern void sensor_malloc();
extern void data_manipulation(int sensorid);
extern void sensor_free();
extern int *get_tempAver();
extern int *get_IDFlagCheck();

#endif
