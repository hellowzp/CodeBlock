/****************************************
*      sensor gateway
****************************************/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include "list.h"

typedef struct {
	unsigned id:12;
	unsigned sequence:4;
	unsigned flags:2;
	unsigned sign:1;
	unsigned value:12;
	unsigned parity:1;
} Packet;

typedef union {
	Packet packet;
	unsigned int code;
} Data_packet;

typedef struct {
	float temperature;
	time_t time_stamp;
} Record;

typedef struct {
	list_ptr_t data;
	int id;
} Sensor;
	
int compareRecord(const void* a,const void* b)
{
	return ((Record*)(*(data_ptr_t *)a))->time_stamp-((Record*)(*(data_ptr_t *)b))->time_stamp; 
}

void* copyRecord(void* record)
{
	Record* ptr = (Record *) malloc( sizeof(Record));	
	assert(ptr!=NULL);	
	ptr->temperature = ((Record *)record)->temperature;
	ptr->time_stamp = ((Record *)record)->time_stamp;
	return (void* )ptr;
}

void destroyRecord(void* record)  
{
	free(record);
}	
	
int compareSensor(const void* a,const void* b)
{
	return ((Sensor *)(*(data_ptr_t *)a))->id-((Sensor *)(*(data_ptr_t *)b))->id; 
}

void* copySensor(void* sensor)
{
	Sensor* ptr = (Sensor *) malloc( sizeof(Sensor));
	assert(ptr!=NULL);		
	ptr->data=list_alloc(compareRecord,copyRecord,destroyRecord);
	int size=list_size(((Sensor*)sensor)->data);
	int i;
	for(i=1;i<=size;i++)
	{
		ptr->data=list_insert_at_index(ptr->data,list_get_data_at_index(((Sensor*)sensor)->data,i),i);
	}
	ptr->id=((Sensor *)sensor)->id;
	return (void* )ptr;
}

void destroySensor(void* sensor)  
{
	list_free(((Sensor*)sensor)->data);
	free(sensor);
}

unsigned check(Data_packet data_packet)
{
	unsigned int c = 0 ;
    while (data_packet.code > 0)
    {
        if((data_packet.code & 1) == 1) 
        {
			c++;
		}
        data_packet.code=data_packet.code>>1;
    }
    return (c%2==0);
}

void insert_record(list_ptr_t sensors, Data_packet data_packet, time_t time_stamp)
{
	Record record;
	record.temperature=data_packet.packet.value*(data_packet.packet.sign?-1:1)/10.0;
	record.time_stamp=time_stamp;
	Sensor sensor;
	sensor.id=data_packet.packet.id;
	int index=list_get_index_of_data(sensors,(void *)&sensor);
	if(index!=-1)
	{
		sensor=*((Sensor*)list_get_data_at_index(sensors,index));
		sensor.data=list_insert_at_index(sensor.data,(void *)&record,list_size(sensor.data)+1);
	}
	else
	{
		sensor.data=list_alloc(compareRecord,copyRecord,destroyRecord);
		sensor.data=list_insert_at_index(sensor.data,(void *)&record,0);
		sensors=list_insert_sorted(sensors,&sensor);
	}
}

void data_compression(list_ptr_t sensors)
{
	int i;
	int j;
	list_ptr_t data;
	int size;
	for(i=1;i<=list_size(sensors);i++)
	{
		data=((Sensor*)list_get_data_at_index(sensors,i))->data;
		size=list_size(data);
		for(j=1;j<=size/3;j++)
		{
			((Record*)list_get_data_at_index(data,j))->temperature=(((Record*)list_get_data_at_index(data,3*j-2))->temperature+((Record*)list_get_data_at_index(data,3*j-1))->temperature+((Record*)list_get_data_at_index(data,j+1))->temperature)/3;
			((Record*)list_get_data_at_index(data,j))->time_stamp=((Record*)list_get_data_at_index(data,3*j))->time_stamp;
		}
		for(j=1;j<=size%3;j++)
		{
			((Record*)list_get_data_at_index(data,size/3+j))->temperature=((Record*)list_get_data_at_index(data,size/3*3+j))->temperature;
			((Record*)list_get_data_at_index(data,size/3+j))->time_stamp=((Record*)list_get_data_at_index(data,size/3*3+j))->time_stamp;
		}
		for(j=1;j<=size-size/3-size%3;j++)
		{
			data=list_free_at_index(data,j);
		}
	}
}

void print_data(list_ptr_t sensors)
{
	int i;
	int j;
	list_ptr_t data;
	int size;
	time_t rawtime;
	struct tm * timeinfo;
	for(i=1;i<=list_size(sensors);i++)
	{
		data=((Sensor*)list_get_data_at_index(sensors,i))->data;
		size=list_size(data);
		printf("\nThe data of sensor %d is:\n",((Sensor*)list_get_data_at_index(sensors,i))->id);
		for(j=1;j<=size;j++)
		{
			rawtime=((Record*)list_get_data_at_index(data,j))->time_stamp;
			time(&rawtime);
			timeinfo = localtime(&rawtime);
			printf("Temperature: %.1f\t",((Record*)list_get_data_at_index(data,j))->temperature);
			printf("Time: %s",asctime(timeinfo));
		}
	}
}

int main (void) 
{
	unsigned int number;
	Data_packet data_packet;
	time_t time_stamp;
	list_ptr_t sensors=list_alloc(compareSensor,copySensor,destroySensor);
	int i;
	
	//manuplate the data
	printf("Program is manuplating 50000 packets now...\n");
	for(i=1;i<=50000;i++)
	{
		scanf("%d", &number);
		time(&time_stamp);
		data_packet.code=number;
		if(check(data_packet)&&data_packet.packet.id<=6)
		{
			insert_record(sensors,data_packet,time_stamp);
		}
	}
	
	//print the data
	print_data(sensors);
	
	printf("\nProgram is compress the data now...\n");
	//compress the data
	data_compression(sensors);
	
	//print the data
	print_data(sensors);
	
	return 0;
}

