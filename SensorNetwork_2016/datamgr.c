#include <stdio.h>
#include <inttypes.h>
#include <stdint.h>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "errutil.h"
#include "config.h"
#include "sbuffer.h"
#include "datamgr.h"
#include "lib/dplist.h"

struct sensor_node{
	sensor_id_t sensor_id;
	room_id_t room_id;
	sensor_value_t buffer[RUN_AVG_LENGTH];
	sensor_ts_t last_modified;
	int index;
	int buffer_size;
};

extern FILE * fifo_wr;

void * datamgr_element_copy(void *element)
{
	sensor_node_t * copy;
	copy = malloc(sizeof(sensor_node_t));
	MALLOC_ERROR(copy);
	
	copy->sensor_id = ((sensor_node_t *)element)->sensor_id;
	copy->room_id = ((sensor_node_t *)element)->room_id;
	copy->last_modified = ((sensor_node_t *)element)->last_modified;
	copy->index = ((sensor_node_t *)element)->index;
	copy->buffer_size = ((sensor_node_t *)element)->buffer_size;
	memcpy(copy->buffer, ((sensor_node_t *)element)->buffer, sizeof(copy->buffer));

	return copy;
}

void datamgr_element_free(void **element)
{
	free(*element);
	*element = NULL;
}

int datamgr_element_compare(void *x, void *y)
{
	if (((sensor_node_t *)x)->sensor_id == ((sensor_node_t *)y)->sensor_id)
		return 0;
	else if (((sensor_node_t *)x)->sensor_id > ((sensor_node_t *)y)->sensor_id)
		return 1;
	return -1;
}

dplist_t *list =NULL;
int dplist_errno;
 
void datamgr_parse_sensor_data(FILE * fp_sensor_map, sbuffer_t **sbuffer)
{
	sensor_data_t * sensor_data = NULL;
	sensor_data = malloc(sizeof(sensor_data_t));
	MALLOC_ERROR(sensor_data);

	sensor_node_t * sensor_node = NULL;
	sensor_node = malloc(sizeof(sensor_node_t));
	MALLOC_ERROR(sensor_node);

	sensor_node->index = 0;
	sensor_node->buffer_size = 0;
//	sensor_node->room_id = 0;
//	sensor_node->sensor_id = 0;
//	sensor_node->last_modified = 0;
	memset(sensor_node->buffer, 0, sizeof(sensor_node->buffer));


	list = dpl_create(datamgr_element_copy, datamgr_element_free, datamgr_element_compare);
	assert(dplist_errno == DPLIST_NO_ERROR);

	while(fscanf(fp_sensor_map,"%" SCNu16 " %" SCNu16 "\n", &sensor_node->room_id, &sensor_node->sensor_id) != EOF)
	{
		//printf("room_id %" PRIu16",  sensor_id %"PRIu16"\n", sensor_node->room_id, sensor_node->sensor_id);
		sensor_node_t *temp = datamgr_get_sensnor_node(sensor_node->room_id);
		if(temp != NULL)
		{
			fprintf(stderr, "sensor with sensor_id = %"PRIu16" is already mapped\n", sensor_node->sensor_id);
			continue;
		}

		dpl_insert_at_index(list, sensor_node, 0, true);
  		assert(dplist_errno == DPLIST_NO_ERROR);
	}
	free(sensor_node);
	sensor_node = NULL;
	
/*
// print out all the sensor_node stored in the list.
	j = dpl_size(list);
	for (i = 0; i<j; i++)
	{
		sensor_node_t * element = dpl_get_element_at_index(list, i);
		printf("element at index %d = %"PRIu16" %" PRIu16" %d   %d\n", i, element->room_id, element->sensor_id, element->index, element->buffer_size);
	}
*/
	while (1)
	{
		sbuffer_data_t sbuffer_data;
		int retval = sbuffer_remove(*sbuffer, &sbuffer_data, TIMEOUT, 1);
		if(retval == SBUFFER_NO_DATA)
			break;
	//	printf("sensor_id %" PRIu16" - sensor_value %lf - sensor_ts %ld\n", sensor_data->id, sensor_data->value, sensor_data->ts);
		*sensor_data = sbuffer_data.sensor_data;
		sensor_node_t *temp = datamgr_get_sensnor_node(sensor_data->id);
		if(temp == NULL)
		{
			//flockfile(fp_FIFO_w);
			fprintf(fp_FIFO_w, "Received sensor data with invalid sensor node ID %"PRIu16"\n", sensor_data->id);
			fflush(fp_FIFO_w);
			//funlockfile(fp_FIFO_w);
#ifdef DEBUG
			fprintf(stderr, "DEBUG log_mssge, Received sensor data with invalid sensor node ID %"PRIu16"\n", sensor_data->id);
#endif			
			continue;
		}
		temp->buffer[temp->index++] = sensor_data->value; 
		temp->last_modified = sensor_data->ts;
		if(temp->buffer_size < RUN_AVG_LENGTH)
			temp->buffer_size++;

		if(temp->index >= RUN_AVG_LENGTH)
			temp->index = 0;

		sensor_value_t running_avg = 0;
		running_avg =  datamgr_running_avg(temp->sensor_id);
		if(running_avg == 0)
		{
			continue;
		}
		if(running_avg > SET_MAX_TEMP)
		{
			//flockfile(fp_FIFO_w);
			fprintf(fp_FIFO_w, "The sensor node with %"PRIu16" reports it’s too hot (running avg temperature = %f)\n", temp->sensor_id, running_avg);
			fflush(fp_FIFO_w);
#ifdef DEBUG
			fprintf(stderr, "DEBUG log_mssge, The sensor node with %"PRIu16" reports it’s too hot (running avg temperature = %f)\n", temp->sensor_id, running_avg);
#endif			
			//funlockfile(fp_FIFO_w);

		}
		if(running_avg < SET_MIN_TEMP)
		{
			//flockfile(fp_FIFO_w);
			fprintf(fp_FIFO_w, "The sensor node with %"PRIu16" reports it’s too cold (running avg temperature = %f)\n", temp->sensor_id, running_avg);
			fflush(fp_FIFO_w);
#ifdef DEBUG			
			fprintf(stderr, "DEBUG log_mssge, The sensor node with %"PRIu16" reports it’s too cold (running avg temperature = %f)\n", temp->sensor_id, running_avg);
#endif			
			//funlockfile(fp_FIFO_w);
		}
/*
#ifdef DEBUG			
		fprintf(stderr, "DEBUG Datamgr_mssge: sensor_id = %"PRIu16"  room_id = %"PRIu16"  last_modified = %ld  index = %d  buffer_size = %d , running_avg %f ", 
				temp->sensor_id, temp->room_id, temp->last_modified, temp->index, temp->buffer_size, running_avg);
		fprintf(stderr, "buffer [");
		for(int i = 0; i < RUN_AVG_LENGTH; i++)
		{
			fprintf(stderr, "%f, ",temp->buffer[i]);
		}
		fprintf(stderr, "]\n");
#endif
*/
	}
	free(sensor_data);
	
}
void datamgr_free()
{
	dpl_free(&list);
  assert(dplist_errno == DPLIST_NO_ERROR);
}

uint16_t datamgr_get_room_id(sensor_id_t sensor_id)
{
	sensor_node_t *sensor_node = NULL;
	sensor_node = datamgr_get_sensnor_node(sensor_id);
	if (sensor_node == NULL)
	{
		//fprintf(stderr, "sensor_id %"PRIu16" is not mapped with any room_id\n", sensor_id);
		return 0;
	}

	return sensor_node->room_id;
}

sensor_value_t datamgr_running_avg(sensor_id_t sensor_id)
{
	sensor_node_t *sensor_node = NULL;
	sensor_node = datamgr_get_sensnor_node(sensor_id);
	if (sensor_node == NULL)
	{
		//fprintf(stderr, "sensor_id %"PRIu16" is not mapped with any room_id\n", sensor_id);
		return -1;
	}

	sensor_value_t running_sum = 0;
	if (sensor_node->buffer_size < 5)
	{
		//fprintf(stderr, "sensor_id = %"PRIu16", less than RUN_AVG_LENGTH = %d measurements are recorded, the avg is 0\n",sensor_node->sensor_id, RUN_AVG_LENGTH);
		return 0;
	}
	for(int i =0; i < RUN_AVG_LENGTH; i++)
	{
		running_sum += sensor_node->buffer[i];
	}

	return running_sum/RUN_AVG_LENGTH;
}

sensor_value_t datamgr_get_avg(sensor_id_t sensor_id)
{
	sensor_value_t ret_val = datamgr_running_avg(sensor_id);
	if (ret_val == 0)
	{
		//fprintf(stderr, "sensor_id = %"PRIu16", less than RUN_AVG_LENGTH = %d measurements are recorded, the avg is 0\n", sensor_id, RUN_AVG_LENGTH);
	}
	return ret_val;
}
time_t datamgr_get_last_modified(sensor_id_t sensor_id)
{
	sensor_node_t *sensor_node;
	sensor_node = datamgr_get_sensnor_node(sensor_id);
	if (sensor_node == NULL)
	{
		//fprintf(stderr, "sensor_id %"PRIu16" is not mapped with any room_id\n", sensor_id);
		return 0;
	}

	return sensor_node->last_modified;
}

int datamgr_get_total_sensors()
{
	int ret_val = 0; 
	ret_val = dpl_size(list);
  assert(dplist_errno == DPLIST_NO_ERROR);

	return ret_val;
}

sensor_node_t *datamgr_get_sensnor_node(sensor_id_t sensor_id)
{
	sensor_node_t *sensor_node=NULL;
	dplist_node_t *list_node=NULL;
	sensor_node = malloc(sizeof(sensor_node_t));
	MALLOC_ERROR(sensor_node);

	sensor_node->sensor_id = sensor_id;
	if((list_node = dpl_get_reference_of_element(list, sensor_node)) == NULL)
	{
  	assert(dplist_errno == DPLIST_NO_ERROR);
		free(sensor_node);  
		return NULL;
	}
	free(sensor_node);
	sensor_node = dpl_get_element_at_reference(list, list_node);
  assert(dplist_errno == DPLIST_NO_ERROR);
	return sensor_node;
}

