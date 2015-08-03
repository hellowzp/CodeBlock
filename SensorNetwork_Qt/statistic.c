#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#include "statistic.h"
#include "gateway.h"
#include "linkedlist.h"

	char buffer0[BUFSIZE];
	data_t sensor_data;
	packet_ptr_t sensor_packet_temp;

	list_ptr_t  list_buff;
	list_ptr_t *all_sensor_list;
	
	int *IDFlagCheck;    //check this sensor is active and just starts to send data

	int *list_index;   //index in the array of time interval
	int *tempAver;

	int data_buffer;
	
	sensor_id_t * idptr;      //config
	sensor_value_t * valueptr;
	sensor_ts_t * tsptr;


void sensor_malloc()
{
	idptr=(sensor_id_t *)malloc(sizeof(sensor_id_t));
	valueptr=(sensor_value_t *)malloc(sizeof(sensor_value_t));
	tsptr=(sensor_ts_t *)malloc(sizeof(sensor_ts_t));
	
	IDFlagCheck=(int *)malloc(Sensor_Number*sizeof(int));    

	list_index=(int *)malloc(Sensor_Number*sizeof(int));

	tempAver=(int *)malloc(Sensor_Number*sizeof(int));

	all_sensor_list=(list_ptr_t *)malloc(Sensor_Number*sizeof(list_ptr_t));	
}


void data_manipulation(int sensorid)
{

		int sensor_id;
		
		data_ptr_t my_data_ptr;     //list
		my_data_ptr=(data_ptr_t)malloc(sizeof(data_ptr_t));
		sensor_id=sensorid;
	
		
		if(IDFlagCheck[sensor_id]==0)
			all_sensor_list[sensor_id]=list_alloc(&data_destory,&data_compare, &data_copy,my_data_ptr);	  //gateway
		IDFlagCheck[sensor_id]=1;
		
		list_insert_at_index(all_sensor_list[sensor_id],&data_buffer,list_index[sensor_id]);	//
				
		list_index[sensor_id]++; 
			
	    tempAver[sensor_data.id]=0;
	    
	    //calculating avterage temperature	   
	    list_buff=list_get_first_reference(all_sensor_list[sensor_id]);  

	    if(list_buff==NULL)
			printf("\nlist error");
	    else
	    {
			while(list_buff)
			{
				tempAver[sensor_id]+=((packet_ptr_t)((int *)(list_buff->data)))->value;		
				list_buff=list_buff->next;  
			}	

	    if(list_size(list_buff)!=0)
	            tempAver[sensor_id]=(tempAver[sensor_id])/list_size(list_buff);					 		

	    }
	    			
}

void sensor_free()
{
	int i;
	for(i=0;i<Sensor_Number;i++)
	{   
		if(IDFlagCheck[i]==1)
		   mylist_free(all_sensor_list[i]);	
	}
	free(IDFlagCheck);
	free(list_index);
//attention: here for req 10
	free(idptr);
	free(valueptr);
	free(tsptr);
}


int *get_IDFlagCheck()
{
	 return IDFlagCheck;
	
}
