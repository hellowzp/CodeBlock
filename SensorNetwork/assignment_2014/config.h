#ifndef _CONFIG_H_
#define _CONFIG_H_

#include <stdint.h>
#include <time.h>

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
}sensor_data_t, * sensor_data_ptr_t;			

#endif /* _CONFIG_H_ */

