#ifndef _CONFIG_H_
#define _CONFIG_H_


#include <stdint.h>
#include <time.h>

/*------------------------------------------------------------------------------
		definitions (defines, typedefs, ...)
------------------------------------------------------------------------------*/

/*-- --*/


typedef uint16_t sensor_id_t;
typedef int16_t sensor_value_t;	// temp in 0.1C, e.g. 18.3C is stored as 183
typedef time_t sensor_ts_t; // UTC timestamp as returned by time()

<<<<<<< HEAD
typedef struct mysensor 
			{
				sensor_id_t id;
				sensor_value_t value;
				sensor_ts_t ts;
			} sensor_data_t, * sensor_data_ptr_t;
=======
typedef struct {
    sensor_id_t id;
    sensor_value_t value;
    sensor_ts_t ts;
} sensor_data_t, * sensor_data_ptr_t;
>>>>>>> 72ab7c9359283513d60aec90b8ff12ff7c918271


#endif /* _CONFIG_H_ */

