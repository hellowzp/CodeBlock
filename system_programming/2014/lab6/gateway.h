#ifndef GATEWAY_H
#define GATEWAY_H

#define MAX 4096  
#define CHECK_PTR_NULL(ptr) do{ if((ptr)== NULL) { \
	                                perror("Error to allocata memory"); \
	                                exit(EXIT_FAILURE); \
	                            } } while(0)

typedef struct {
	unsigned id:12;
	unsigned seq:4;
	unsigned flags:2;
	unsigned sign:1;
	unsigned tem:12;
	unsigned parity:1;
} Packet;

typedef union {
	Packet pkt_seg;
	unsigned short int pkt_whole;
} Packet_union;

typedef struct {
	float temperature;
	time_t time_stamp;
} Record,*Record_ptr;

void initRecord(list_ptr_t record_array[]);
Record_ptr makeRecord(Packet pkt);
void addRecord(list_ptr_t lptr,Record_ptr rec);
void printRecord(list_ptr_t record_array[]);
float packet_getTemp(Packet_union data);


#endif
