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
	int   number;
	int   i=0,j;
    FILE *fp;
	sensor_data_t  temp_packet;
	sensor_data_t  package;
	sensor_data_ptr_t  d;
    //temp_packet_ptr=(sensor_data_ptr_t)malloc(sizeof(sensor_data_t));
    //store the value into a list
    fp=fopen("./myfile.data","a");
	do
    {
		scanf("%d", &number);
		printf ("the sensor parameter is %d\n", number);
        
		temp_packet = *((sensor_data_ptr_t)&number);   //number's value to temp_packet_ptr
 
        
        //deepcopy
        temp_packet=deep_copy(&temp_packet);
        
		
		package.ts = time(NULL);				
		package.id = temp_packet.id;	
		package.value = temp_packet.value;
		
	    //store the value into file		

		

		d=(sensor_data_ptr_t)(&package);
		
		fwrite(d,sizeof(sensor_data_t),1,fp);


        printf("recieve value is %d,%d\n",temp_packet.id,temp_packet.value);
        if((j=fwrite(d,sizeof(sensor_data_t),1,fp))<1){
			printf("write failed!");
			exit(1);
		}

        i++;
		
	}while(i<10);
    fclose(fp);
	//store the value of file into database
	
	return 0;
}


//----------------------------------------------------------------------------//
sensor_data_t deep_copy(sensor_data_ptr_t data)
//----------------------------------------------------------------------------//
{
	sensor_data_t t;
	t = (sensor_data_t)(*data);
	return (sensor_data_t)t; 
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
