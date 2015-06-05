#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

#include "config.h"
#include "tcpsocket.h"

#define POINTER_CHECK(ptrcheck,fun) do{ if(ptrcheck) printf("%s() error: invalid parameter...\n",fun); } while(0)

sensor_id_t sid;    //uint16_t
unsigned int frq;
unsigned int sseq;
unsigned int port;  //remote gateway ip and port used to create tcp connection
char* ip;

/*can be used to set or check parity bit(return 0 if right)
int sensor_pkt_parity(packet_ptr_t ppt) {
    packet_union_t punion;
    punion.pkt_seg = *ppt;

    int cnt = 0;
    int bits = 32;
    while(bits) {
        cnt += punion.pkt_whole%2;
        punion.pkt_whole /= 2;
        bits--;
    }

    if(cnt%2) {
        ppt->parity ^= 1; //XOR with 1:invert
        return 1;
    } else {
        return 0;
    }
} */

void sensor_set_parity(packet_ptr_t ppt) {
    packet_union_t punion;
    punion.pkt_seg = *ppt;
	//printf("%d %d\n",ppt->parity, punion.pkt_whole);
	
    int cnt = 0;
    int bits = 32;
    while(bits) {
        cnt += punion.pkt_whole%2;
        punion.pkt_whole /= 2;
        bits--;
    }

    if(cnt%2) ppt->parity ^= 1; //XOR with 1:invert  
    //printf("%d %d\n",ppt->parity, punion.pkt_whole);      
} 

void sensor_pkt_sim(packet_ptr_t ppt,sensor_data_ptr_t sdp) {
	POINTER_CHECK(!ppt || !sdp , "sensor_pkt_sim");
	memset(ppt,0,sizeof(*ppt));
	memset(sdp,0,sizeof(*sdp));

	sseq++;
	ppt->seq = sseq;
	ppt->id = sid;

	srand((unsigned)time(NULL));
	int t = rand()%6000 - 2000;
	ppt->sign = (t>0)?0:1;
    ppt->tem = (t>0)?t:-t;
	sensor_set_parity(ppt);

	sdp->id = sid;
	sdp->value = t;
	sdp->ts = time(NULL);
}


int main(int argv, char* args[]) {

	if(argv!=3) {  //command line parameters wrong(run-time) ==>instead of compile options
		printf("usage: %s %s\n",args[0],"ip port");
		exit(EXIT_FAILURE);
	} 

	#ifndef SET_ID
	//#pragma message("sensor ID should be set by the preprocessor directive SET_ID at compile-time.")
	#error "sensor ID should be set by the preprocessor directive SET_ID at compile-time."
	#endif

	#ifndef SET_FREQUENCY
	#define SET_FREQUENCY 60
	#endif

	//initialization
	sid = SET_ID;
	frq = SET_FREQUENCY;
	sseq = 0;
	port = (int)strtol(args[2],NULL,10);  //remote gateway ip and port used to create tcp connection
	ip = args[1];

	char filename[20];
	int len = snprintf(filename,20,"log%d.msg",sid);

	packet_ptr_t ppt = malloc(sizeof(packet_t));
	sensor_data_ptr_t sdp = malloc(sizeof(sensor_data_t));

	int log_fds = open(filename,O_CREAT | O_RDWR | O_APPEND);
	if(log_fds<0) {
        perror("open log file error");
        exit(EXIT_FAILURE);
	}
	
	//create tcp connection to the gateway
	Socket client = tcp_active_open(port,ip);

	while(1) {
		sensor_pkt_sim(ppt,sdp);
		tcp_send(client,ppt,sizeof(*ppt));

		char logstr[100];
		len = snprintf(logstr,100,"%d: %5.2fC @ %s", sid,  //automatically line buffered to log file??
                  (float)(sdp->value)/100.0f, ctime(&(sdp->ts)));
		printf("%d %d %d %s\n",sid,len,sdp->value,logstr);
		write(log_fds,logstr,len);
		
		sleep(10);
	}
	
	tcp_close(&client);
	return 0;
}
