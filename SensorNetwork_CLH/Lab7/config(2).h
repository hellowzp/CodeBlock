#ifndef _CONFIG_H_
#define _CONFIG_H_


#include <stdint.h>
#include <time.h>

/*------------------------------------------------------------------------------
		definitions (defines, typedefs, ...)
------------------------------------------------------------------------------*/

typedef uint16_t sensor_id_t;
typedef double sensor_value_t;    
typedef time_t sensor_ts_t;         // UTC timestamp as returned by time()

typedef struct{
	sensor_id_t id;
	sensor_value_t value;
	sensor_ts_t ts;
}sensor_data_t, * sensor_data_ptr_t;
			

#endif /* _CONFIG_H_ */

