/****************************************
*      sensor gateway
****************************************/


/*-----------------------------------------------------------------------------
		include files
------------------------------------------------------------------------------*/
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include "double_list.h"
#include  "gateway.h"



//----------------------------------------------------------------------------//
int main (void) 
//----------------------------------------------------------------------------//
{
	int number;
	list_ptr_t *my_sensor_array;
	packet_ptr_t my_packet_ptr;
	int one_bit_count;
	data_t my_data;
	time_t t_1 = time(NULL);	//use two time references to keeping track of time to compress data 
	time_t t_2 = time(NULL);	

	init_array(&my_sensor_array); //initialize the sensor array
	
	while ( 1 ) {
		scanf("%d", &number);
		printf ("the gateway is %d\n", number);

		my_packet_ptr = (packet_ptr_t)(&number);  //take the int as a packet

		//count the number of ones in the packet
		for (one_bit_count = 0; *(int *)my_packet_ptr; (*(int *)my_packet_ptr) >>= 1)
			one_bit_count += (*(int *)my_packet_ptr & 1);
			
		// if the prity check fails, then skip the following code in while loop and start a new loop
		if (one_bit_count % 2) 
			continue;

		// add a timetamp and take the data from the packet if prity check succeeds
		my_data.timestamp = time(NULL);				//timestamp
		my_data.sensor_sign = my_packet_ptr->sensor_sign;	//take the data
		my_data.sensor_value = my_packet_ptr->sensor_value;

		

		//if the linked list for this sensor does not exsit, then creat one
		if (my_sensor_array[(int)my_packet_ptr->sensor_id] == NULL) 
			list_alloc(&destory_data, &compare_data, &deep_copy, (data_ptr_t)&my_data);

		else    //add the data in the beginning of the linked list
			list_insert_at_index(my_sensor_array[(int)my_packet_ptr->sensor_id], (data_ptr_t)&my_data, 0);
		error_handler(err);	//call the error handler each time we use the linked array to see it the array works weel

		t_2 = time(NULL);
		if ((t_2 - t_1) < period_daily)
			continue;
		else
		{
			t_2 = t_1 = time(NULL);
			data_compression_tool(&my_sensor_array);
		}
	}

	destory_array(&my_sensor_array);
	
	return 0;
}

//----------------------------------------------------------------------------//
void init_array(list_ptr_t **ptr)
//----------------------------------------------------------------------------//
{
	*ptr = (list_ptr_t *)malloc(number_of_sensors * sizeof(list_ptr_t));
	if (*ptr == NULL)
	{
		printf("Fail to malloc memory");
		exit(1);
	}

}

//----------------------------------------------------------------------------//
void destory_array(list_ptr_t **ptr)
//----------------------------------------------------------------------------//
{
	int i;	
	// free each array in the sensor array
	for (i = 0; i < number_of_sensors; i++)
		list_free((*ptr)[i]);
	// free the sensor array itself
	free(*ptr);
}

//----------------------------------------------------------------------------//
void data_compression_tool(list_ptr_t **ptr)
//----------------------------------------------------------------------------//
{
	int i, j;
	int d_1, d_2, d_3;
	for (i = 0; i < number_of_sensors; i++)
	{
		if (((*ptr)[i] != NULL) && (list_size((*ptr)[i]) >= 3))  //when the list exist and contains more than 2 data
		{	
			for (j = 0; j < (list_size((*ptr)[i]) - 3) / 3; j++)
			{
				if (((data_t *)list_get_data_at_index((*ptr)[i], j * 3))->sensor_sign)
					d_1 = -(int)(((data_t *)list_get_data_at_index((*ptr)[i], j * 3))->sensor_value);
				else
					d_1 = (int)(((data_t *)list_get_data_at_index((*ptr)[i], j * 3))->sensor_value);
				//call the error handler each time we use the linked array to see it the array works weel
				error_handler(err);

				if (((data_t *)list_get_data_at_index((*ptr)[i], j * 3))->sensor_sign)
					d_2 = -(int)(((data_t *)list_get_data_at_index((*ptr)[i], j * 3 + 1))->sensor_value);
				else
					d_2 = (int)(((data_t *)list_get_data_at_index((*ptr)[i], j * 3 + 1))->sensor_value);
				error_handler(err);

				if (((data_t *)list_get_data_at_index((*ptr)[i], j * 3))->sensor_sign)
					d_3 = -(int)(((data_t *)list_get_data_at_index((*ptr)[i], j * 3 + 2))->sensor_value);
				else
					d_3 = (int)(((data_t *)list_get_data_at_index((*ptr)[i], j * 3 + 2))->sensor_value);
				error_handler(err);

				d_1 = (d_1 + d_2 + d_3) / 3;
				if (d_1 >= 0)
				{
					((data_t *)list_get_data_at_index((*ptr)[i], j * 3))->sensor_sign = 0;
					error_handler(err);
					((data_t *)list_get_data_at_index((*ptr)[i], j * 3))->sensor_value = d_1;
					error_handler(err);
				}
				else if (d_1 < 0)
				{
					((data_t *)list_get_data_at_index((*ptr)[i], j * 3))->sensor_sign = 1;
					error_handler(err);
					((data_t *)list_get_data_at_index((*ptr)[i], j * 3))->sensor_value = abs(d_1);
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

//call back funtions

//----------------------------------------------------------------------------//
data_ptr_t deep_copy(data_ptr_t data)
//----------------------------------------------------------------------------//
{
	data_t *t = (data_t *)malloc(sizeof(data_t));
	*t = *(data_t *)data;
	return (data_ptr_t)t; 
}

//----------------------------------------------------------------------------//
int compare_data(data_ptr_t data_1, data_ptr_t data_2)
//----------------------------------------------------------------------------//
{
	if (((data_t *)data_1)->sensor_sign != ((data_t *)data_2)->sensor_sign)
	{
		if (((data_t *)data_1)->sensor_value != ((data_t *)data_2)->sensor_value)
		{
			if (((data_t *)data_1)->sensor_sign)
				return 1;
			else
				return -1;
		}
		else
			return 0;
	}
	else
	{
		if (((data_t *)data_1)->sensor_sign)
			return (int)(((data_t *)data_2)->sensor_value - ((data_t *)data_1)->sensor_value);
		else
			return (int)(((data_t *)data_1)->sensor_value - ((data_t *)data_2)->sensor_value);
	}
}

//----------------------------------------------------------------------------//
void destory_data(data_ptr_t data)
//----------------------------------------------------------------------------//
{
	free((data_t *)data);
}

//----------------------------------------------------------------------------//
void error_handler(err_code err)
//----------------------------------------------------------------------------//
{
	switch (err)
	{
		case NONE:  // print nothing if nothing wrong with the array
			break;
		case NULL_POINTER_ERR:
			printf("+++++++++++ result ++++++++++++\n no pointer error (your list doesn't exist)\n--------------------------------\n\n");
			break;
		case NONE_EXIST_REF_ERR:
			printf("+++++++++++ result ++++++++++++\n no such reference (your reference is not valid)\n--------------------------------\n\n");
			break;
		case NON_NEXT_REF_ERR:
			printf("+++++++++++ result ++++++++++++\n no next reference (your reference is already the last one)	\n--------------------------------\n\n");
			break;
		case NON_PREVIOUS_REF_ERR:
			printf("+++++++++++ result ++++++++++++\n no previous reference (your reference is already the first one)\n--------------------------------\n\n");
			break;
		case NO_DATA_ERR:
			printf("+++++++++++ result ++++++++++++\n no data error (your data is not stored in the list)\n--------------------------------\n\n");
			break;
		default:
			printf("+++++++++++ result ++++++++++++\n unknow error \n--------------------------------\n\n");
			
		
	}			
}












