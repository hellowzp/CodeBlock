#ifndef GATEWAY
#define GATEWAY


#include"double_list.h"

/*------------------------------------------------------------------------------
		definitions (defines, typedefs, ...)
------------------------------------------------------------------------------*/
typedef struct packet_t{
	unsigned sensor_id: 12;
	unsigned sensor_sequence: 4;
	unsigned sensor_flag: 2;
	unsigned sensor_sign: 1;
	unsigned sensor_value: 12;
	unsigned sensor_parity: 1;
} packet_t, *packet_ptr_t;

typedef struct data_t{
	unsigned sensor_sign: 1;
	unsigned sensor_value: 12;
	time_t timestamp;
	} data_t;

#define period_weekly 604800
#define period_daily 86400

#define number_of_sensors 4096

/*------------------------------------------------------------------------------
		function prototypes
------------------------------------------------------------------------------*/
void init_array(list_ptr_t **);
void destory_array(list_ptr_t **);
void data_compression_tool(list_ptr_t **);
void error_handler(err_code);
data_ptr_t deep_copy(data_ptr_t data);
int compare_data(data_ptr_t data_1, data_ptr_t data_2);
void destory_data(data_ptr_t data);

#endif
