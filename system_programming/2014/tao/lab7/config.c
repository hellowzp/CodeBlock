/****************************************
*      config generate
****************************************/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <string.h>
#include "config.h"


int main (void) 
{
	int number;
	int i;
	sensor_data_ptr_t temp_packet_ptr;
    temp_packet_ptr=(sensor_data_ptr_t)malloc(sizeof(sensor_data_t));
    //store the value into a list
	for(i=0;i<10;i++)
    {
		scanf("%d", &number);
		printf ("the sensor parameter is %d\n", number);
        
		temp_packet_ptr = (sensor_data_ptr_t)&number;   //number's value to temp_packet_ptr
        sensor_data_ptr_t NewNode[i];
        
        NewNode[i]=(sensor_data_ptr_t)malloc(sizeof(sensor_data_t));
        if(NewNode[i]==NULL)
        {
           err=NULL_POINTER_ERR;
           error_handler(err);
        }
        //deepcopy
        temp_packet_ptr=deep_copy(temp_packet_ptr);
        
		NewNode[i]=temp_packet_ptr;
		
		NewNode[i]->ts = time(NULL);				
		NewNode[i]->id = temp_packet_ptr->id;	
		NewNode[i]->value = temp_packet_ptr->value;
		
	    //store the value into file		
		FILE *fp;
		fp=fopen("./myfile.dat","a");
		sensor_data_t* d;
		d=(sensor_data_t*)(&NewNode[i]);
		
		fwrite(d,sizeof(sensor_data_t),1,fp);
		fclose(fp);

        printf("recieve value is %d,%d\n",temp_packet_ptr->id,temp_packet_ptr->value);

        //i++;
		
	}

     free(temp_packet_ptr);
	//store the value of file into database
	
	return 0;
}


//----------------------------------------------------------------------------//
sensor_data_ptr_t deep_copy(sensor_data_ptr_t data)
//----------------------------------------------------------------------------//
{
	sensor_data_ptr_t t = (sensor_data_ptr_t)malloc(sizeof(sensor_data_t));
	t = (sensor_data_ptr_t)data;
	return (sensor_data_ptr_t)t; 
}


//-----------------------------------------------------------------------------//
void error_handler(err_code err)
//-----------------------------------------------------------------------------//
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
