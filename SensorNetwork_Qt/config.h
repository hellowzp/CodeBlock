#ifndef _CONFIG_H_
#define _CONFIG_H_

#include <stdint.h>
#include <time.h>

/*------------------------------------------------------------------------------
 definitions for sensor data
 ------------------------------------------------------------------------------*/

typedef uint16_t sensor_id_t;
typedef uint16_t room_id_t;
typedef int16_t sensor_value_t;     // temp in 0.1C, e.g. 18.3C is stored as 183
typedef time_t sensor_ts_t;         // UTC timestamp as returned by time()
typedef uint8_t sensor_data_statu_t;
typedef struct {   // lab 6
	sensor_id_t id;
    sensor_value_t tmp;
	sensor_ts_t ts;
    sensor_data_statu_t state;
} sensor_data_t, *sensor_data_ptr_t;  // for queue element

typedef struct {   // lab 6
    sensor_id_t sensor_id;
    room_id_t room_id;
    float avg_tmp;
    sensor_ts_t ts;
} list_element_t, *list_element_ptr_t;

#endif /* _CONFIG_H_ */

