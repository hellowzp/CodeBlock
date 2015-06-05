#ifndef _DATASERVER_H_
#define _DATASERVER_H_

#include <my_global.h>
#include <mysql.h>
#include <stdint.h>
#include <time.h>
#include "D_list.h"
#include "tcpsocket.h"
#include <pthread.h>

/*------------------------------------------------------------------------------
		definitions (defines, typedefs, ...)
------------------------------------------------------------------------------*/
typedef enum{LOGIN_IP_PORT,LOGOFF_IP_PORT,LOG_CLIENT_APP,LOG_GATEOPEN,LOG_CHECK_OK,LOG_CHECK_NO} log_info;

typedef struct time{
	
	int tm_year;
	int tm_mon;
	int tm_mday;
	int tm_hour;
	int tm_min;
	int tm_sec;

	} tm_1,*tm_2;
	
typedef struct argu {
		
	Socket client;
	pthread_mutex_t log_m;
	pthread_mutex_t db_m;
	pthread_mutex_t para_m;
		
}argument_1,*argument_ptr;

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

/*-- --*/


typedef uint16_t sensor_id_t;
typedef int16_t sensor_value_t;	// temp in 0.1C, e.g. 18.3C is stored as 183
typedef time_t sensor_ts_t; // UTC timestamp as returned by time()


typedef struct sensor_data_t{
	sensor_id_t id;
	sensor_value_t value;
	sensor_ts_t ts;
} sensor_data_t, *sensor_data_ptr_t;

log_info logkeep;


#define day_time 600   //10 minutes		
#define BUFSIZE 1024
#define number_of_sensors 4096   //max number of sensors
/*----------------------------------------------------
 * function prototype
 * --------------------------------------------------*/
sensor_packet deep_copy(packet_ptr_t data);
void copy_fun(MYSQL *conn,sensor_data_t data);
void * myprocess (void *arg);
void init_array(list_ptr_t **);
void destory_array(list_ptr_t **);
void data_compression(list_ptr_t **);
void error_handler(err_code);
data_ptr_t data_copy(data_ptr_t data);
int data_compare(data_ptr_t d1, data_ptr_t d2);
void data_destory(data_ptr_t data);
void save_in_list (sensor_data_ptr_t temp_packet_ptr,pthread_mutex_t para_m,FILE* fp);
list_ptr_t free_sensor_list(list_ptr_t *ptr,int index);
int getTime(char *out, int fmt);
FILE* openFile(const char *fileName, const char *mode);
int writeFile(FILE *fp,char *str);
int closeFile(FILE *fp);
char* log_handler(FILE *fp,log_info logkeep,Socket client);
char* log_off(FILE *fp,char* ip,int port);
sensor_data_t copydata(sensor_data_t data);
void parameter_save(FILE *fp,int value,int t,int id);
#endif /* _DATASERVER_H_ */

