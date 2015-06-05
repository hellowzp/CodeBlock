#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <error.h>
#include "list.h"
#include "gateway.h"

#define DEBUG

int main() {
	list_ptr_t sensor_array[MAX];
	initRecord(sensor_array);
	
	int i;
	for(i=0; i<10; i++) { 
		//packet_ptr pkt;  //received packet
		Packet_union data;   //received sensor packet
		Record_ptr data_ptr;
	
		scanf("%d",(int*)&data.pkt_whole);
		printf("scanded value:%d\n",data.pkt_whole);
		data_ptr = makeRecord(data.pkt_seg);
		addRecord(sensor_array[data.pkt_seg.id],data_ptr);

		int sid = (int) data.pkt_seg.id;
		int size = list_size(sensor_array[sid]);
		float temp = ((Record_ptr)list_get_data_at_index(sensor_array[sid],size))->temperature;
		time_t tm = time(NULL);
		printf("new record added: id=%d temp=%3.2f @ %s\n",sid,temp,ctime(&tm));
	}
	
	//printRecord(sensor_array);
}
	
void initRecord(list_ptr_t rarray[]) {
	//the array has already been allocated memory since the time defined,
	//so just allocate memory for each element in the array since they are all pointers
	int i;
	for(i=0; i<MAX; i++) {
		rarray[i] = list_alloc();
		CHECK_PTR_NULL(rarray[i]);
	}
}

Record_ptr makeRecord(Packet pkt) {
	Record_ptr rptr = (Record_ptr)malloc(sizeof(Record));
	CHECK_PTR_NULL(rptr);
	rptr->time_stamp = time(NULL);
	rptr->temperature = ((pkt.tem)/512*10 + (pkt.tem)/64%8 + (pkt.tem)/8%8*0.1 + (pkt.tem)%8*0.01)*(pkt.sign?-1:1);
	printf("%d temp %4.2f\n",(int)(pkt.tem),rptr->temperature);
	return rptr;
}

void addRecord(list_ptr_t lptr,Record_ptr rptr) {
	list_insert_at_index(lptr,rptr,list_size(lptr)+1);	
}

void printRecord(list_ptr_t record_array[]) {
	int i,j;
	for(i=0; i<MAX; i++) {
		printf("data at sensor id %d:\n",i);
		for(j=0; j<list_size(record_array[i]); j++) {
			Record_ptr rptr = list_get_data_at_index(record_array[i],j);
			printf("temp:%f ",rptr->temperature);
		}
	}
}


