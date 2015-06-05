/****************************************
*      sensor gateway
****************************************/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include "gateway.h"
#include "D_list.h"

int main (void) 
{
	int number=0;
	int ones_check=0;
	packet_ptr_t temp_packet_ptr;
	list_ptr_t  *sensor_array;
	int parityNum=0;               
	data_t data;
	time_t base_T = time(NULL);	//keep to track time
	time_t variable_T = time(NULL);	

	init_array(&sensor_array); //initialization
	
	while ( 1 ) 
    {
		scanf("%d", &number);
		printf ("the gateway is %d\n", number);

		temp_packet_ptr = (packet_ptr_t)(&number);  //number's value to temp_packet_ptr

		// to count the number of '1' in the data packet
		for (ones_check = 0; *(int *)temp_packet_ptr; (*(int *)temp_packet_ptr) >>= 1)
	    ones_check += (*(int *)temp_packet_ptr & 1);
		parityNum=ones_check;
		
		if (parityNum % 2) 
			continue;

		//if check pass go on process
		data.timestamp = time(NULL);				
		data.signbit = temp_packet_ptr->signbit;	
		data.value = temp_packet_ptr->value;

		

		//if the linked list for this sensor does not exsit, then creat one
		if (sensor_array[(int)temp_packet_ptr->id] == NULL) 
			list_alloc(&data_destory, &data_compare, &data_copy, (data_ptr_t)&data);//(function's return value)

		else    //add the data at beginning of list,position->0
			list_insert_at_index(sensor_array[(int)temp_packet_ptr->id], (data_ptr_t)&data, 0);
		    error_handler(err);	

		variable_T = time(NULL);
		if ((variable_T - base_T) < day_time)
			continue;
		else
		{
			variable_T = base_T = time(NULL);      //refresh timestamp
			data_compression(&sensor_array);
		}
	}

	destory_array(&sensor_array);
	
	return 0;
}

/*int check_parity(sensor_packet (*temp_packet_ptr))
{
	int ones_check;
	for (ones_check = 0; *(int *)temp_packet_ptr; (*(int *)temp_packet_ptr) >>= 1)
	ones_check += (*(int *)temp_packet_ptr & 1);
	return ones_check;
}*/

void init_array(list_ptr_t **ptr)
{
	*ptr = (list_ptr_t *)malloc(number_of_sensors * sizeof(list_ptr_t));
	if (*ptr == NULL)
	{
		printf("Wrong memory");
		assert(1==0);
	}

}

void destory_array(list_ptr_t **ptr)

{
	int i;	
	// free each parameter list of sensor array
	for (i = 0; i < number_of_sensors; i++)
		list_free((*ptr)[i]);
	// free the sensor array 
	free(*ptr);
}

void data_compression(list_ptr_t **ptr)
{
	int i, j;
	int v1, v2, v3;
	for (i = 0; i < number_of_sensors; i++)
	{
		if (((*ptr)[i] != NULL) && (list_size((*ptr)[i]) >= 3))  
		{	
			for (j = 0; j < (list_size((*ptr)[i]) - 3) / 3; j++)
			{
				if (((data_t *)list_get_data_at_index((*ptr)[i], j * 3))->signbit)      //get first value
					v1= -(int)(((data_t *)list_get_data_at_index((*ptr)[i], j * 3))->value);
				else
					v1 = (int)(((data_t *)list_get_data_at_index((*ptr)[i], j * 3))->value);
				
				error_handler(err);

				if (((data_t *)list_get_data_at_index((*ptr)[i], j * 3))->signbit)      //get second value
					v2 = -(int)(((data_t *)list_get_data_at_index((*ptr)[i], j * 3 + 1))->value);
				else
					v2 = (int)(((data_t *)list_get_data_at_index((*ptr)[i], j * 3 + 1))->value);
				error_handler(err);

				if (((data_t *)list_get_data_at_index((*ptr)[i], j * 3))->signbit)     //get third value
					v3 = -(int)(((data_t *)list_get_data_at_index((*ptr)[i], j * 3 + 2))->value);
				else
					v3 = (int)(((data_t *)list_get_data_at_index((*ptr)[i], j * 3 + 2))->value);
				error_handler(err);

				v1 = (v1 + v2 + v3) / 3;                                               //get the average value and copy to v1,then free the second and third list
				if (v1 >= 0)                                                           //set the signbit
				{
					((data_t *)list_get_data_at_index((*ptr)[i], j * 3))->signbit = 0;
					error_handler(err);
					((data_t *)list_get_data_at_index((*ptr)[i], j * 3))->value = v1;
					error_handler(err);
				}
				else if (v1 < 0)
				{
					((data_t *)list_get_data_at_index((*ptr)[i], j * 3))->signbit = 1;
					error_handler(err);
					((data_t *)list_get_data_at_index((*ptr)[i], j * 3))->value = abs(v1);
					error_handler(err);
				}
			}
			for (j = 0; j < (list_size((*ptr)[i]) - 3) / 3; j++)
			{
				(*ptr)[i] = list_free_at_index((*ptr)[i], j +1);
				error_handler(err);
				(*ptr)[i] = list_free_at_index((*ptr)[i], j +1);
				error_handler(err);
			}		
		}							
	}
}


data_ptr_t data_copy(data_ptr_t data)
{
	data_t *t = (data_t *)malloc(sizeof(data_t));
	*t = *(data_t *)data;
	return (data_ptr_t)t; 
}

int data_compare(data_ptr_t d1, data_ptr_t d2)
{
	if (((data_t *)d1)->signbit != ((data_t *)2)->signbit)
	{
		if (((data_t *)d1)->value != ((data_t *)d2)->value)
		{
			if (((data_t *)d1)->signbit)
				return 1;
			else
				return -1;
		}
		else
			return 0;
	}
	else
	{
		if (((data_t *)d1)->signbit)
			return (int)(((data_t *)d2)->value - ((data_t *)d1)->value);
		else
			return (int)(((data_t *)d1)->value - ((data_t *)d2)->value);
	}
}


void data_destory(data_ptr_t data)
{
	free((data_t *)data);
}


void error_handler(err_code err)
{
	switch (err)
	{
		case NONE: 
			break;
		case NULL_POINTER_ERR:
			printf("error:no pointer \n");
			break;
		case NONE_EXIST_REF_ERR:
			printf("no such reference \n");
			break;
		case NON_NEXT_REF_ERR:
			printf(" no next reference \n");
			break;
		case NON_PREVIOUS_REF_ERR:
			printf(" no previous reference\n");
			break;
		case NO_DATA_ERR:
			printf(" no data  \n");
			break;
		default:
			printf(" unknow error \n");
			
		
	}			
}
