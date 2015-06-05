#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>
#include <pthread.h>

#include "mymqtt.h"
//#include "coordinator.h"

/***********************************
            Zigbee
************************************/
#define ZIGBEE_START_DELIMITER 0x7E

#define AT_COMMAND             0x08
#define AT_COMMAND_QUEUE       0x09
#define TRANSMIT_REQUEST       0x10
#define REMOTE_AT              0x17
#define CREATE_SOURCE_ROUTE    0x21
#define REG_JOINING_DEVICE     0x24
#define AT_RESPONSR            0x88
#define MODEM_STATUS           0x8A
#define TX_STATUS              0x8B
#define RECEIVE_PACKET         0x90
#define EXPLICIT_RX_INDICATOR  0x91
#define REMOTE_RESPONSE        0x97

#define PARAMETER_ERROR(fun) do{ printf("%s() error: invalid parameter...\n",fun); } while(0)
                                
typedef unsigned char Byte;
typedef char* String;

typedef struct {
	String name;
	String value;
} Map, *map_ptr_t;

typedef struct {
	char* address;     // mac address
	map_ptr_t payload; // waspmote sensor field
} Data, *data_ptr_t; 

/*
typedef struct {
	void* msg;   //mqtt message
	char* addr;
} MQTT, *mqtt_ptr_t;
*/

Byte xbee_set_checksum(String xbee_data, int len);
int xbee_txReq_data_assemble(String mac_addr,String ntwk_addr,char radius,char option,String RFData,int len,String* retStr);
int xbee_txReq_frame_assemble(String mac_addr,String ntwk_addr,char radius,char option,String RFData,int len);
//read the data section of the zigbee frame and call the corresponding sub-read function according to the frame type
mqtt_ptr_t xbee_data_read(String xbee_data,int len);
//read ZigBee Receive Packet(0x90) data section, return the desired data structure pointer 
mqtt_ptr_t xbee_rx_data_read(String xbee_data,int len);
mqtt_ptr_t xbee_exrx_data_read(String xbee_data,int len);
int xbee_remoteAT_frame_assemble(String mac_addr,String ntwk_addr,char option,String cmd, char para);
int xbee_localAT_frame_assemble(String cmd,char value);
int xbee_mqtt_wframe_assenble(mqtt_ptr_t mqtt);
int xbee_frame_escape(String buf,int len);  //length of buffer(outbuf)
int xbee_frame_descape(String buf,int len); //length of buffer(inbuf)

/***********************************
            Waspmote
************************************/
#define MAX_FRAME_SIZE   200
#define ALLOW_EMPTY_FIELD

String wframe_get_data_at_del( char* pkt, int n, char del);
//get data section between two dels including the ending del but excluding the beginning del
//if the beginning del is <=0 get before the second del including that del
//if the ending is too large get after the first excluding that del
String wframe_get_data_between_del( char* pkt, int len, int bn, int en, char del);
int wframe_assemble(Byte frame_type, Byte fields, char* serial_id,Byte seq,map_ptr_t payload,char** wframe);

/***********************************
            ttyUSB
************************************/

#ifdef RD
#define MODEM_DEVICE "/dev/ttyUSB0"
#endif
#ifdef WR
#define MODEM_DEVICE "/dev/ttyUSB0"
#endif

int tty_open(char* tty_name);
void tty_serial_config(int fds);
void tty_serial_init();
int tty_serial_read(int fds,String buf);
int tty_serial_write(int fds,String buf,int len);


/***********************************
            mystring
************************************/

#define FREE_STRING_ARRAY(sarray) do{ int i=0;  \
	while(*(sarray+i)) {                        \
		free(*(sarray+i));                      \
		*(sarray+i) = NULL;                     \
		i++;                                    \
	}                                           \
	free(sarray);                               \
	sarray = NULL;                              \
} while(0)

int int_to_str(int num,char** str);
String hex_to_str(String hexstr,int len);
int str_length( char* str);
long power(char a, unsigned char b);
int string_to_int(String s, int base);
//get sub-string including both bn and en
//if 0<=bn<=en<len not satisfied, print error
char* str_get_between_index( char* str, int len, int bn, int en);
//concatenate variable length of strings(sensor field)
//if allow empty field, treat NULL field as empty and also add it
//else exit with error
char* str_multi_cat(int num,  char del, ...);
int str_split( char* str, char del,char*** retArray);
map_ptr_t str_to_map( char* str, char fdel, char sdel);
int map_cat(map_ptr_t map,  char fdel,  char sdel, char** str);


/***********************************
          global variables
************************************/
String inbuf;
String outbuf;
int tty_fds;
int waspmote_seq;


/***********************************
            Zigbee
************************************/

Byte xbee_set_checksum(String xbee_data, int len) {
    if(!xbee_data) PARAMETER_ERROR("xbee_set_chechsum");
    int sum = 0;
    int i;
    for(i=0; i<len; i++) {   //can't use strlen() because the data may contain 0x00
        sum += xbee_data[i];
        sum = (sum>255)?(sum-256):sum;
    }
    return (Byte)(0xFF-sum); 
}

int xbee_txReq_data_assemble(String mac_addr,String ntwk_addr,char radius,char option,String RFData,int len,String* retStr) {
    if(!mac_addr || !ntwk_addr || !RFData || !retStr)
        PARAMETER_ERROR("xbee_txReq_data_assemble");
    //int len = strlen(RFData);
    String str = malloc(len+15);
    str[0] = 0x10;
    str[1] = 0x01;
    str[2] = 0;
    strncat(str,mac_addr,8);
    strncat(str,ntwk_addr,2);
    str[12] = radius;
    str[13] = option;
    str[14] = 0;
    strncat(str,RFData,len);
    *retStr = str;
    return len+14;
}

int xbee_txReq_frame_assemble(String mac_addr,String ntwk_addr,char radius,char option,String RFData,int len) {
	if(!mac_addr || !ntwk_addr || !RFData)
        PARAMETER_ERROR("xbee_txReq_frame_assemble");
    //int len = strlen(RFData) + 14;  //frame length field, not the total length   
    if(outbuf[0]) memset(outbuf,0,MAX_FRAME_SIZE);
    
    len += 14;
    outbuf[0] = ZIGBEE_START_DELIMITER;
    outbuf[1] = len/256;
    outbuf[2] = len%256;
    outbuf[3] = 0x10;  //frame type
    outbuf[4] = 0x01;  //frame id
    int i;
    for(i=0; i<8; i++) outbuf[5+i] = mac_addr[i];		
	outbuf[13] = ntwk_addr[0];
    outbuf[14] = ntwk_addr[1];
    outbuf[15] = radius;
    outbuf[16] = option;
    for(i=0; i<len-14; i++) outbuf[17+i] = RFData[i];
    outbuf[len+3] = xbee_set_checksum(&outbuf[3],len);
    outbuf[len+4] = 0;
    return len+4;   //return total length
}    

int xbee_remoteAT_frame_assemble(String mac_addr,String ntwk_addr,char option,String cmd, char para) {
	if(!mac_addr || !ntwk_addr || !cmd)
        PARAMETER_ERROR("xbee_remoteAT_frame_assemble");
    
    outbuf[0] = ZIGBEE_START_DELIMITER;
    outbuf[1] = 00;
    //outbuf[2] = len%256;
    outbuf[3] = 0x17;  //frame type
    outbuf[4] = 0x01;  //frame id
    int i;
    for(i=0; i<8; i++) outbuf[5+i] = mac_addr[i];		
	outbuf[13] = ntwk_addr[0];
    outbuf[14] = ntwk_addr[1];
    outbuf[15] = option;
    outbuf[16] = cmd[0];
    outbuf[17] = cmd[1];
    
    if(para=='Q') { //query, no para
		outbuf[2] = 15;
		outbuf[18] = xbee_set_checksum(&outbuf[3],15);
		return 19;
	} else {
		outbuf[2] = 15;
		outbuf[18] = para;
		outbuf[19] = xbee_set_checksum(&outbuf[3],16);
		return 20;
    }   
}

int xbee_localAT_frame_assemble(String cmd, char para) {
	if(!cmd) PARAMETER_ERROR("xbee_localAT_frame_assemble");
	outbuf[0] = ZIGBEE_START_DELIMITER;
    outbuf[1] = 00;
    //outbuf[2] = len%256;
    outbuf[3] = 0x08;  //frame type
    outbuf[4] = 0x01;  //frame id
    outbuf[5] = cmd[0];
    outbuf[6] = cmd[1];
    
    if(para=='Q') { //query, no para
		outbuf[2] = 4;
		outbuf[7] = xbee_set_checksum(&outbuf[3],4);
		return 8;
	} else {
		outbuf[2] = 5;
		outbuf[7] = para;
		outbuf[8] = xbee_set_checksum(&outbuf[3],5);
		return 9;
    }   
}

/*
int xbee_mqtt_wframe_assenble(mqtt_ptr_t mqtt) {
	if(!mqtt) PARAMETER_ERROR("xbee_mqtt_Wframe_assenble");
	int len = *(Byte*)(mqtt->msg+1) + 14;  //LENGTH FIELD not the total length
    outbuf[0] = ZIGBEE_START_DELIMITER;
    outbuf[1] = len/256;
    outbuf[2] = len%256;
    outbuf[3] = 0x10;  //frame type
    outbuf[4] = 0x01;  //frame id
    String mac_addr = mqtt->addr;
    int i;
    for(i=0; i<8; i++) outbuf[5+i] = mac_addr[i];		
	outbuf[13] = 0xFF;
    outbuf[14] = 0XFE;
    outbuf[15] = 0;
    outbuf[16] = 0;
    String RFData = (String)mqtt->msg;
    for(i=0; i<len-14; i++) outbuf[17+i] = RFData[i];
    outbuf[len+3] = xbee_set_checksum(&outbuf[3],len);
    return len+4;   //return total length
}
*/

int xbee_frame_escape(String buf, int len) {
	int i,j, leng=len;
	char esc[MAX_FRAME_SIZE];
	esc[0] = 0; //save the number of escape characters
	for(i=3; i<len-1; i++) {  //just escape the data section
		char c = buf[i];
		if(c==0x11 || c==0x13 || c==0x7D || c==0x7E) {
			esc[0]++;
			esc[esc[0]] = i;
			leng++;
		}
	}
	
	//insert algorithm
	esc[esc[0]+1] = len;
	for(i=esc[0];i>0;i--) {
		for(j=esc[i+1]-1;j>=esc[i];j--) {
			buf[i+j] = buf[j];			
		}
		//printf("%d %d %d ",i,j,outbuf[i+j+1]);
		buf[i+j+1] ^= 0x20;   //XOR
		//printf("%d\n",outbuf[i+j+1]);
		buf[i+j] = 0x7D;
	}	
	//outbuf[2] += esc[0];	//change length	
	return leng;
}

int xbee_frame_descape(String buf, int len) {
	if(!buf) PARAMETER_ERROR("xbee_frame_descape");
	int i,j, leng=len;
	char esc[MAX_FRAME_SIZE];
	esc[0] = 0; //save the number of escape characters
	for(i=3; i<len-1; i++) {  //just escape the data section
		if(buf[i]==0x7D) {
			esc[0]++;
			esc[esc[0]] = i;
			leng--;
		}
	}
	
	//insert algorithm
	esc[esc[0]+1] = len;
	for(i=1;i<=esc[0];i++) {
		for(j=esc[i]+1;j<esc[i+1];j++) {
			buf[j-i] = buf[j];			
		}
		buf[esc[i]+1-i] ^= 0x20; 
	}	
	
	return leng;
}
        
//read ZigBee Receive Packet(0x90) data section excluding the checksum, return the desired data structure pointer 
mqtt_ptr_t xbee_rx_data_read(String xbee_data,int len) {
	if(!xbee_data) PARAMETER_ERROR("xbee_rx_data_read");
	mqtt_ptr_t mqtt = malloc(sizeof(*mqtt));
	mqtt->ts = time(NULL);
	
	String mac_addr = str_get_between_index(xbee_data,len,1,8);
	int i,j;
	uint64_t sum = 0;
	for(i=0; i<8; i++) {
		Byte c = (mac_addr[i]<0) ? (mac_addr[i]+256):mac_addr[i]; 
		uint64_t pow = (uint64_t)c;
		for(j=0;j<7-i;j++){  //pow*power(256,7-i)
			pow *= 256;
		}
		sum += pow;
	}
	mqtt->addr = sum;
	
	//String wframe = str_get_between_index(xbee_data,len,12,len-2); //excluding the checksum
	//String s = wframe_get_data_between_del(wframe,len-13,1,2,'#');
	
	//begin from the start index of RF data
	for(i=12; i++; i<len) {
		if(xbee_data[i]=='#') {
			j = i;
			break;
		}
	}
	if(j+3>len-1) {
		printf("%s\n","# not found, invalid waspmote frame...");
		return NULL;
	}

	mqtt->len = len-j-3;
	mqtt->msg = (void*)str_get_between_index(xbee_data,len,j+3,len-1);
	String data = malloc(mqtt->len);
	data = memcpy(data,mqtt->msg,mqtt->len);
	printf("%s %ld\n",data,mqtt->addr);
	
	//int bytes = xbee_mqtt_wframe_assenble(mqtt);
	//tty_serial_write(tty_fds,outbuf,bytes);
	
	String ntw_addr = str_get_between_index(xbee_data,len,9,10);
	//String RFData = (char*)mqtt->msg;	
	//int leng = mqtt->len;
	//char ntw_addr[2] = {0xFF,0xFE};
	String RFData = "hello";
	int leng = 5;
	int bytes = xbee_txReq_frame_assemble(mac_addr,ntw_addr,0,0,RFData,leng);
	
	int logfds = open("./wresclog.txt", O_CREAT | O_RDWR | O_APPEND);
	int ret = 0;
	leng = bytes;
	String fbuf = outbuf;
	while (leng>0 && (ret=write(logfds,fbuf,leng))!=0) {
		if(ret==-1) {
			if(errno==EINTR)
				continue;
			perror("wrinting to log file");
		}
		leng -= ret;
		fbuf += ret;
	}
	if(close(logfds)==-1) perror("close log file");	
	
	bytes = tty_serial_write(tty_fds,outbuf,bytes);
	printf("write %d bytes... %d\n",bytes,RFData[0]);
	
	//free(mac_addr);
	//free(ntw_addr);
	//free(RFData); 

	return mqtt;

	//FIRST GET THE PAYLOAD STRING and then convert it to map format 
	/*String payload_str = NULL;
	map_ptr_t payload_map = NULL;
	int fields = *(wframe+4); //the fifth char, expressed as 0x03(char with asc value 3), so no need to convert (from asc) to number
	payload_str = wframe_get_data_between_del(wframe,4,fields+4,'#');
	payload_map = str_to_map(payload_str,'#',':');
	printf("%c %c %d\n",wframe[0],wframe[1],fields);
	
	data_ptr_t mydata = (data_ptr_t)malloc(sizeof(Data));
	mydata->address = mac_addr;
	mydata->payload = payload_map;
	free(payload_str);
	free(wframe);
	return mydata; */
}	

//read Explicit Rx_indicator(0x91) data section, return the desired data structure pointer 
mqtt_ptr_t xbee_exrx_data_read(String xbee_data,int len) {
	if(!xbee_data) PARAMETER_ERROR("xbee_exrx_data_read");	
	mqtt_ptr_t mqtt = malloc(sizeof(*mqtt));
	mqtt->ts = time(NULL);
	
	String mac_addr = str_get_between_index(xbee_data,len,1,8);
	int i,j;
	uint64_t sum = 0;
	for(i=0; i<8; i++) {
		Byte c = (mac_addr[i]<0) ? (mac_addr[i]+256):mac_addr[i]; 
		uint64_t pow = (uint64_t)c;
		for(j=0;j<7-i;j++){  //pow*power(256,7-i)
			pow *= 256;
		}
		sum += pow;
	}
	mqtt->addr = sum;
	free(mac_addr);

	//String wframe = str_get_between_index(xbee_data,len,12,len-2); //excluding the checksum
	//String s = wframe_get_data_between_del(wframe,len-13,1,2,'#');
	
	//begin from the start index of RF data
	for(i=18; i++; i<len) {
		if(xbee_data[i]=='#') {
			j = i;
			break;
		}
	}
	if(j+3>len-1) {
		printf("%s\n","# not found, invalid waspmote frame...");
		return NULL;
	}

	mqtt->len = len-j-3;
	mqtt->msg = (void*)str_get_between_index(xbee_data,len,j+3,len-1);	
	printf("%s\n",(char*)(mqtt->msg));
	
	//int bytes = xbee_mqtt_wframe_assenble(mqtt);
	//tty_serial_write(tty_fds,outbuf,bytes);
	
	return mqtt;
	/*
	String mac_addr = str_get_between_index(xbee_data,2,9);
	String wframe = str_get_between_index(xbee_data,18,len-2);
	
	//FIRST GET THE PAYLOAD STRING and then convert it to map format
	String payload_str = NULL;
	map_ptr_t payload_map = NULL;
	int fields = *(wframe+4); //the fifth char, expressed as 0x03(char with asc value 3), so no need to convert (from asc) to number
	payload_str = wframe_get_data_between_del(wframe,len-19,1,2,'#');
	payload_map = str_to_map(payload_str,'#',':');
	printf("%d %d %d\n",wframe[0],wframe[1],fields);
	
	data_ptr_t mydata = (data_ptr_t)malloc(sizeof(Data));
	mydata->address = mac_addr;
	mydata->payload = payload_map;
	free(payload_str);
	free(wframe);
	return mydata; */
}	

mqtt_ptr_t xbee_data_read(String xbee_data,int len) {
	if(!xbee_data) PARAMETER_ERROR("xbee_data_read");
	Byte frame_type = xbee_data[0];
	printf("received frame type: %d\n",(int)frame_type);
	
	mqtt_ptr_t mydata = NULL;
	switch(frame_type) {		
		case 0x90 :    //received an RF packet
			mydata = xbee_rx_data_read(xbee_data,len);
			break;
		case 0x91 :    //explicit rx indicator
			mydata = xbee_exrx_data_read(xbee_data,len);
			break;
		case 0x97 :    //remote AT response
		case 0x8A :    //modem status
			//printf("");
		case 0x8B :    //transmit status
			
		default:
			break;
	}
	return mydata;
}


/***********************************
            ttyUSB
************************************/

int tty_open(char* tty_name) {
	int tty_fds = open(tty_name,O_RDWR | O_NOCTTY | O_NONBLOCK | O_APPEND);
	if(tty_fds<0) {
        perror("open tty error");
        exit(EXIT_FAILURE);
	} 
	return tty_fds;
}

void tty_serial_config(int fds) {
	/*
	 * http://www.opensource.apple.com/source/xnu/xnu-1456.1.26/bsd/sys/termios.h
	 * typedef unsigned long	tcflag_t;
	   typedef unsigned char	cc_t;
	   typedef unsigned long	speed_t;

	   struct termios {
			tcflag_t	c_iflag;	input flags 
			tcflag_t	c_oflag;	output flags 
			tcflag_t	c_cflag;	control flags 
			tcflag_t	c_lflag;	local flags 
			cc_t		c_cc[NCCS];	control chars 
			speed_t		c_ispeed;	input speed 
			speed_t		c_ospeed;	output speed 
        };   
     */
	struct termios tio_old, tio_new;
	tcgetattr(fds,&tio_old); /* save the current serial port settings */
	//bzero(&newtio, sizeof(newtio));
	tio_new=tio_old;
	
	//input option:ignore parity check, map CR(Enter,carriage return,\r) to NL
	tio_new.c_iflag |= (IGNPAR | ICRNL);

	//output option: raw output
	tio_new.c_oflag &= ~OPOST;
	
	//control flags
	//8N1:8 bits, no parity check, 1 stop bit 
	tio_new.c_cflag &= ~PARENB;
	tio_new.c_cflag &= ~CSTOPB;
	tio_new.c_cflag &= ~CSIZE;
	tio_new.c_cflag |= CS8;
	//disable hardware control
	tio_new.c_cflag &= ~CRTSCTS;
	
	tio_new.c_cflag |= (CLOCAL | CREAD);	
	
	//line option: raw input mode
	tio_new.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
	
	//special control characters
	tio_new.c_cc[VMIN]  = 1;
	tio_new.c_cc[VTIME] = 0;
	
	//baud rate
	cfsetispeed(&tio_new, B115200);
	cfsetospeed(&tio_new, B115200);
	
	tcsetattr(fds, TCSANOW, &tio_new);//Make changes now without waiting for data to complete
}	

void tty_serial_init() {
	tty_fds = tty_open(MODEM_DEVICE);
	tty_serial_config(tty_fds);
	waspmote_seq = 0;
	
	if((inbuf = malloc(MAX_FRAME_SIZE)) == NULL ) {
		printf("%s\n","fail to allocate memory for inbuf...");
		exit(EXIT_FAILURE);
	} else {
		memset(inbuf,0,MAX_FRAME_SIZE);
	}

	if((outbuf = malloc(MAX_FRAME_SIZE)) == NULL ) {
		printf("%s\n","fail to allocate memory for outbuf...");
		exit(EXIT_FAILURE);
	} else {
		memset(outbuf,0,MAX_FRAME_SIZE);
	}	
}

//read one total frame and load the data section(except for the start three bytes) into buf
//and return the length of it(including the checksum bit)
int tty_serial_read(int fds, String tbuf) {
	if(fds<0 || !tbuf) PARAMETER_ERROR("tty_serial_read");
	if(tbuf[0]) memset(tbuf,0,MAX_FRAME_SIZE);
	
	while(tbuf[0]!='~') {
		if(read(fds,tbuf,1)<0) {
			perror("reading ttyUSB0");
			sleep(2);
		}
	}
	tbuf++; //point to the new position
		
	ssize_t ret;
	int i,bytes=1,leng = 2; //bytes:bytes already read, length: bytes to read
	while (leng!=0 && (ret=read(fds,tbuf,leng))!=0) {
		if(ret==-1) {
			if(errno==EINTR)
				continue;
			perror("read the starting 3 bytes");  //no data available in non-blocking mode or other errors
			sleep(2);
			return -1;
		}
		leng -= ret;
		tbuf += ret;
	}
	bytes += 2;
	
	leng = inbuf[1]*256 + inbuf[2] +1;  //including the checksum bit, be careful to use inbuf cauze tbuf is moving!!
	printf("received data section length including the checksum: %d\n",leng);
	
	//continue reading the followint bytes of length leng and check escape
	int nr_escapes = leng;
	do {
		leng = nr_escapes;
		while (leng!=0 && (ret=read(fds,tbuf,leng))!=0) {
			if(ret==-1) {
				if(errno==EINTR)
					continue;
				perror("read the data section error");
				sleep(1);
				return -1;
			}
			leng -= ret;
			tbuf += ret;
		}
		bytes += nr_escapes;
		
		int i, leng = nr_escapes;
		nr_escapes = 0;
		for(i=bytes-leng;i<bytes;i++) {  //check the newly read bytes
			if(inbuf[i]==0x7D) {
				printf("the byte after escape: %d %d\n",i, inbuf[i+1]);
				//int c = tbuf[1+i];
				//if(c==0x7E || c==0x11 || c==0x13 || c==0x7D) {
				nr_escapes++;
			}
		} 
	} while(nr_escapes);
	
	#ifndef DEBUG
	leng = bytes;
	String fbuf = inbuf;
	int logfds = open("./esclog.txt", O_CREAT | O_RDWR | O_APPEND);
	while (leng!=0 && (ret=write(logfds,fbuf,leng))!=0) {
		if(ret==-1) {
			if(errno==EINTR)
				continue;
			perror("wrinting to log file");
		}
		leng -= ret;
		fbuf += ret;
	}
	if(close(logfds)==-1) perror("close esclog file");
	#endif
	
	//descape and check checksum
	bytes = xbee_frame_descape(inbuf,bytes);		
	if(xbee_set_checksum(&inbuf[3],bytes-3)!=0) {
		printf("%s\n","checksum error...");
		//memset(inbuf,0,bytes);
		return 0;
	} else {
		printf("%s total bytes after descape: %d\n","checksum perfect!",bytes);
	}
	
	#ifndef DEBUG
	leng = bytes;
	fbuf = inbuf;
	int dlogfds = open("./esclog.txt", O_CREAT | O_RDWR | O_APPEND);
	while (leng!=0 && (ret=write(dlogfds,fbuf,leng))!=0) {
		if(ret==-1) {
			if(errno==EINTR)
				continue;
			perror("wrinting to dlog file");
		}
		leng -= ret;
		fbuf += ret;
	}
	if(close(dlogfds)==-1) perror("close desclog file");
	#endif
	
	return bytes;
}

int tty_serial_write(int fds, String buf, int len) {
	if(fds<0 || !buf) PARAMETER_ERROR("tty_serial_write");
	
	ssize_t ret;
	int bytes = 0;
	while (len!=0 && (ret=write(fds,buf,len))!=0) {
		if(ret==-1) {
			if(errno==EINTR)
				continue;
			perror("read the starting 3 bytes");
			return 0;
		}
		len -= ret;
		buf += ret;
		bytes+=ret;
	}
	memset(buf,0,MAX_FRAME_SIZE);
	return bytes;
}

/***********************************
            Waspmote
************************************/
/*
String wframe_get_data_at_del( char* pkt, int n,  char del) {
	if(!pkt || n<=0) {
		printf("%s\n","wframe_get_data_at_del(): invalid parameter...");
		return NULL;
	}
	int i,j,cnt = 0;
	int len = str_length(pkt);
	for(j=0; j<len; j++) {
		if(*(pkt+j)==del) {
			cnt++;
			if(cnt==n-1) i=j;
			if(cnt==n) break;
		}
	}
	if(n==1) return str_get_between_index(pkt, 0, j-1); //char at j is the del, remove it
	if(i+1<j-1) return str_get_between_index(pkt, i+1, j-1);
	else if(i+1==j-1) {    //between the 2 dels contains only one char
		char* c = (char*)malloc(2);
		*c = *(pkt+i+1);
		*(c+1) = 0;
		return c;
	} else {               //the 2 dels are next to each other
		char* c = (char*)malloc(1);
		*c = 0;
		return c;
	}
}

String wframe_get_data_between_del( char* pkt, int len, int bn, int en,  char del) {
	if(!pkt || bn<0 || en<bn) {
		printf("%s\n","wframe_get_data_between_del(): invalid parameter...");
	}
	int i,j,cnt = 0;
	for(j=0; j<len; j++) {
		if(*(pkt+j)==del) {
			cnt++;
			if(cnt==bn) i=j;
			if(cnt==en) break;
		}
	}
	if(bn==0) return str_get_between_index(pkt, len, 0, j);  //get before the second
	
	if(j==len) j -= 1;
	return str_get_between_index(pkt, len, i+1, j);
}

int wframe_assemble(Byte frame_type, Byte fields, char* serial_id,Byte seq,map_ptr_t payload,char** wframe) {
	if(!payload || !wframe) PARAMETER_ERROR("wframe_assemble");
	char* seq_str = NULL;
	char* pld_str = NULL;
	int_to_str(seq,&seq_str);
	map_cat(payload,'#',':',&pld_str);
	char* start = malloc(6); //first 5 bytes
	*start = 60;
	*(start+1) = 61;
	*(start+2) = 62;
	*(start+3) = (char)frame_type;
	*(start+4) = (char)fields;
	*(start+5) = 0;
	
	*wframe = str_multi_cat(5,'#',start,serial_id,"WASPMOTE_XBEE",seq_str,pld_str);  
	free(start);
	free(seq_str);
	free(pld_str);
	return str_length(*wframe);
}	
*/
/***********************************
            mystring
************************************/
int int_to_str(int num,char** str) {
	int bits = 1;
	int temp = num;
	while(num = num/10) {
		bits++;
		//num = num/10;
	}
	*str = malloc(bits+1);
	int i;
	for(i=0; i<bits; i++) {
		*(*str+bits-1-i) = temp%10 + 48;
		temp = temp/10;
	}
	*(*str+bits) = 0;
	return bits;
}

String hex_to_str(String hex,int len) {
	//int len = strlen(hex);
	if(len!=4 || len!=16) {
		printf("%s\n","invalid address string");
		return NULL;
	} 
	int i, l = len/2;
	String s = malloc(l+1);
	for(i=0; i<l; i++) {
		int msb = hex[2*i];
		int lsb = hex[2*i+1];
		if(48<=msb && msb<=57) {
			msb -= 48;         //numbers
		} else if(65<=msb && msb<=70) {
			msb -= 55;         //upper-case letters
		} else if(97<=msb && msb<=102) {
			msb -= 87;         //lower-case letters
		} else {
			printf("%s\n","invalid address..");
			exit(EXIT_FAILURE);
		}
		if(48<=lsb && lsb<=57) {
			lsb -= 48;         //numbers
		} else if(65<=lsb && lsb<=70) {
			lsb -= 55;         //upper-case letters
		} else if(97<=lsb && lsb<=102) {
			lsb -= 87;         //lower-case letters
		} else {
			printf("%s\n","invalid address..");
			exit(EXIT_FAILURE);
		}
		s[i] = msb*16 + lsb;
	}
	s[l] = 0;
	return s;
}		

int str_length( char* str) {
	if(!str) return -1;
	int len = 0;
	while(*(str+len)) len++;
	return len;
}

long power(char a, unsigned char b) {
	unsigned char i;
	long pow = 1;
	if(a>=48) a-=48;  //like if a='1' instead of a=1
	for(i=0; i<b; i++) {
		pow *= a;
	}
	return pow;
}

String str_get_between_index( char* str, int len, int bn, int en) {
	if(!str || bn<0 || bn>en || en>len) {
		printf("%s\n","str_get_between_index(): invalid parameter...");
		return NULL;
	}
	//int len = str_length(str);
	//if(en>=len) return NULL;
	char* data = (char*)malloc(en-bn+2);
	int i;
	for(i=bn; i<=en; i++) {
		*(data+i-bn) = *(str+i);
	}
	*(data+en-bn+1) = 0;
	return data;
}


int main() {
    tty_serial_init();
    
    #ifdef RD
    #ifdef AU
    while(1) {
		int bytes = 0;
		while(bytes<=0) {
			bytes = tty_serial_read(tty_fds,inbuf);
			//printf("received data length: %d\n",bytes);
		}
		printf("received frame length: %d %d %d %d %d\n",bytes,inbuf[2],inbuf[inbuf[2]+3],inbuf[5],inbuf[6]);
		mqtt_ptr_t mydata = xbee_data_read(&inbuf[3],bytes-4); //only the data section, excluding checksum
		if(mydata) {      
			//data_print(mydata);
			//free(mydata);
		}

		int logfds = open("./rdesclog.txt", O_CREAT | O_RDWR | O_APPEND);
		int ret = 0, leng = bytes;
		String fbuf = inbuf;
		while (leng>0 && (ret=write(logfds,fbuf,leng))!=0) {
			if(ret==-1) {
				if(errno==EINTR)
					continue;
				perror("wrinting to log file");
			}
			leng -= ret;
			fbuf += ret;
		}
		if(close(logfds)==-1) perror("close log file");	
	}
		    
    #else
	
    int bytes = 0;
    while(bytes<=0) {			
		memset(inbuf,0,MAX_FRAME_SIZE);
		bytes = read(tty_fds,inbuf,MAX_FRAME_SIZE);
		printf("received data length: %d %d %d %d %d\n",bytes,inbuf[2],inbuf[inbuf[2]+4+2],inbuf[5],inbuf[6]);
		sleep(3);
	}
			
	bytes = xbee_frame_descape(inbuf,bytes);
	printf("received data length: %d %d %d %d %d\n",bytes,inbuf[2],inbuf[inbuf[2]+3],inbuf[5],inbuf[6]);
	if(xbee_set_checksum(&inbuf[3],inbuf[2]+1)) printf("%s\n","checksum error");
	
	/*
	int logfds = open("./esclog.txt", O_CREAT | O_RDWR | O_APPEND);
	int ret = 0, leng = bytes;
	String fbuf = inbuf;
	while (leng>0 && (ret=write(logfds,fbuf,leng))!=0) {
		if(ret==-1) {
			if(errno==EINTR)
				continue;
			perror("wrinting to log file");
		}
		leng -= ret;
		fbuf += ret;
	}
	if(close(logfds)==-1) perror("close log file");				
	*/
		
	mqtt_ptr_t mydata = xbee_data_read(&inbuf[3],bytes-4);
	if(mydata) {      
		//data_print(mydata);
		//free(mydata);
	}
	#endif
	
	if(close(tty_fds)==-1) perror("close ttyUSB file");
    #endif 

	#ifdef WR

	while(1) {
		char mac_addr[8] = {0x00,0x13,0xA2,0x00,0x40,0x7A,0x24,0x7A};
		char ntw_addr[2] = {0xFF,0xFE};
		//String RFData = (char*)mqtt->msg;	
		//int leng = mqtt->len;
		//char ntw_addr[2] = {0xFF,0xFE};
		String RFData = "hello";
		int leng = 5;
		int bytes = xbee_txReq_frame_assemble(mac_addr,ntw_addr,0,0,RFData,leng);
	
		/*
		int logfds = open("./wresclog.txt", O_CREAT | O_RDWR | O_APPEND);
		int ret = 0;
		leng = bytes;
		String fbuf = outbuf;
		while (leng>0 && (ret=write(logfds,fbuf,leng))!=0) {
			if(ret==-1) {
				if(errno==EINTR)
					continue;
				perror("wrinting to log file");
			}
			leng -= ret;
			fbuf += ret;
		}
		if(close(logfds)==-1) perror("close log file");	
		*/
		
		bytes = tty_serial_write(tty_fds,outbuf,bytes);
		printf("write %d bytes... %d\n",bytes,RFData[0]);
		
		sleep(10);
    }

    #endif

    
    return 0;
}


