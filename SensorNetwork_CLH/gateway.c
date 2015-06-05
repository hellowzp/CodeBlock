#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>

#include "gateway.h"
#include "linkedlist.h"

//parity check
int parityCheck(packet_ptr_t dataPacket)
{
	int parity= 0 ;
    while (*(int*)dataPacket)
    {
       if(parity==0)
         parity=1;
       else
         parity=0;
       (*(int*)dataPacket)=(*(int*)dataPacket)&((*(int*)dataPacket)-1);
    }    
    return parity;
}

/*
//initial list
void init_array(list_ptr_t **ptr)
{
	*ptr = (list_ptr_t *)malloc(sensor_number * sizeof(list_ptr_t));
	if (*ptr == NULL)
	{
		printf("Wrong memory");
		assert(1==0);
	}

}

//destroy list
void destory_array(list_ptr_t **ptr)
{
	int i;	
	// free each parameter list of sensor array
	for (i = 0; i < sensor_number; i++)
		mylist_free((*ptr)[i]);
	// free the sensor array 
	free(*ptr);
}

//compress data
void data_compression(list_ptr_t **ptr)
{
	int i, j;
	int v1, v2, v3;
	for (i = 0; i < sensor_number; i++)
	{
		if (((*ptr)[i] != NULL) && (list_size((*ptr)[i]) >= 3))  
		{	
			for (j = 0; j < (list_size((*ptr)[i]) - 3) / 3; j++)
			{
				if (((data_t *)list_get_data_at_index((*ptr)[i], j * 3))->signbit)      //get first data
					v1= -(int)(((data_t *)list_get_data_at_index((*ptr)[i], j * 3))->value);
				else
					v1 = (int)(((data_t *)list_get_data_at_index((*ptr)[i], j * 3))->value);
				
				error_handler(err);

				if (((data_t *)list_get_data_at_index((*ptr)[i], j * 3))->signbit)      //get second data
					v2 = -(int)(((data_t *)list_get_data_at_index((*ptr)[i], j * 3 + 1))->value);
				else
					v2 = (int)(((data_t *)list_get_data_at_index((*ptr)[i], j * 3 + 1))->value);
				error_handler(err);

				if (((data_t *)list_get_data_at_index((*ptr)[i], j * 3))->signbit)     //get third data
					v3 = -(int)(((data_t *)list_get_data_at_index((*ptr)[i], j * 3 + 2))->value);
				else
					v3 = (int)(((data_t *)list_get_data_at_index((*ptr)[i], j * 3 + 2))->value);
				error_handler(err);

				v1 = (v1 + v2 + v3) / 3;                                              
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
	printf("\nCompression now");
}
*/

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

//destroy data
void data_destory(data_ptr_t data)
{
	free((data_t *)data);
}

/*
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
*/
