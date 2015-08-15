/****************************************
*      sensor gateway
****************************************/
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include "simulator.h"
#include "list.h"


#define COMPARISON_LIMIT 10
#define SENSOR_ID_MONITOR 0x01

struct packet_record {
		outpacket content; 
        int time;
	};
typedef struct packet_record packetrecord;
typedef packetrecord *packet_ptr;

int temp_atlist(packet_ptr packet_loc);
void data_compression(list_ptr_t list_ptr);

int main (void) 
{
	outpacket data;
    list_ptr_t first_sensor_list=list_alloc ();
       
	while ( 1 ) {
                scanf("%p",(void **)&data);
                printf ("%x\n", data);
		
				if(data.id_b1==SENSOR_ID_MONITOR)
				{
					packet_ptr packet_loc=malloc ( sizeof(packetrecord) );
                    packet_loc->content=data;
                    packet_loc->time=(int)time(NULL);
                    if(first_sensor_list != NULL){
                    first_sensor_list=list_insert_at_index( first_sensor_list,packet_loc,list_size(first_sensor_list));
                    }
				}
	
                printf("%d\n",list_size(first_sensor_list) );
                if(list_size(first_sensor_list)>COMPARISON_LIMIT){
                   data_compression(first_sensor_list);
                }  
	}
	return 0;
}

void data_compression(list_ptr_t list_ptr){
   int i=1;
   int sum=0;
   packet_ptr packet_loc;
   while(i<list_size( list_ptr) ){
      packet_loc=list_get_data_at_index( list_ptr, i);
      sum=sum+temp_atlist(packet_loc);
      if(i%3==0){
         int average=sum/3;
         (packet_loc->content).value_b1=(0xff)&average;
         int highno=average>>8;
         (packet_loc->content).value_b2=(0x1f)&highno;
         sum=0;
      }
      i++;
   }
   i=1;
   int j=1;
   int size=list_size( list_ptr );
   while(i<size){
     if(i%3 != 0){
     list_ptr=list_free_at_index( list_ptr, j);
     }
     else j++;
     i++;
   }    
}

int temp_atlist(packet_ptr packet_loc){
    outpacket data=packet_loc->content;
    int output=data.value_b2;
    output=data.value_b1+(output<<8);
    return output;
}
