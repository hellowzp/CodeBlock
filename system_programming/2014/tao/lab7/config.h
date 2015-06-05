#ifndef _CONFIG_H_
#define _CONFIG_H_


#include <stdint.h>
#include <time.h>

/*------------------------------------------------------------------------------
		definitions (defines, typedefs, ...)
------------------------------------------------------------------------------*/

/*-- --*/

typedef enum{NONE = 0, NULL_POINTER_ERR,  NONE_EXIST_REF_ERR, NON_NEXT_REF_ERR,NON_PREVIOUS_REF_ERR,NO_DATA_ERR} err_code;
typedef uint16_t sensor_id_t;
typedef int16_t sensor_value_t;	// temp in 0.1C, e.g. 18.3C is stored as 183
typedef time_t sensor_ts_t; // UTC timestamp as returned by time()


typedef struct sensor_data_t{
	sensor_id_t id;
	sensor_value_t value;
	sensor_ts_t ts;
} sensor_data_t, *sensor_data_ptr_t;


err_code err;			
#define day_time 864000

#define number_of_sensors 1024   //max number of sensors
/*----------------------------------------------------
 * function prototype
 * --------------------------------------------------*/
sensor_data_ptr_t deep_copy(sensor_data_ptr_t data);
void error_handler(err_code);
#endif /* _CONFIG_H_ */

