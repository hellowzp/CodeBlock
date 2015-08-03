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

#include "mymqtt.h"
#include "coordinator.h"


/***********************************
          global variables
************************************/
extern String co_inbuf;
extern String co_outbuf;
extern int tty_fds;

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

int xbee_txReq_frame_assemble(String mac_addr,String ntwk_addr,char radius,char option,String RFData,int len) {
	if(!mac_addr || !ntwk_addr || !RFData)
        PARAMETER_ERROR("xbee_txReq_frame_assemble");
    //int len = strlen(RFData) + 14;  //frame length field, not the total length   
    if(co_outbuf[0]) memset(co_outbuf,0,MAX_FRAME_SIZE);
    
    len += 14;
    co_outbuf[0] = ZIGBEE_START_DELIMITER;
    co_outbuf[1] = len/256;
    co_outbuf[2] = len%256;
    co_outbuf[3] = 0x10;  //frame type
    co_outbuf[4] = 0x01;  //frame id
    int i;
    for(i=0; i<8; i++) co_outbuf[5+i] = mac_addr[i];		
	co_outbuf[13] = ntwk_addr[0];
    co_outbuf[14] = ntwk_addr[1];
    co_outbuf[15] = radius;
    co_outbuf[16] = option;
    for(i=0; i<len-14; i++) co_outbuf[17+i] = RFData[i];
    co_outbuf[len+3] = xbee_set_checksum(&co_outbuf[3],len);
    co_outbuf[len+4] = 0;
    return len+4;   //return total length
}    

int xbee_remoteAT_frame_assemble(String mac_addr,String ntwk_addr,char option,String cmd, char para) {
	if(!mac_addr || !ntwk_addr || !cmd)
        PARAMETER_ERROR("xbee_remoteAT_frame_assemble");
    
    co_outbuf[0] = ZIGBEE_START_DELIMITER;
    co_outbuf[1] = 00;
    //co_outbuf[2] = len%256;
    co_outbuf[3] = 0x17;  //frame type
    co_outbuf[4] = 0x01;  //frame id
    int i;
    for(i=0; i<8; i++) co_outbuf[5+i] = mac_addr[i];		
	co_outbuf[13] = ntwk_addr[0];
    co_outbuf[14] = ntwk_addr[1];
    co_outbuf[15] = option;
    co_outbuf[16] = cmd[0];
    co_outbuf[17] = cmd[1];
    
    if(para=='Q') { //query, no para
		co_outbuf[2] = 15;
		co_outbuf[18] = xbee_set_checksum(&co_outbuf[3],15);
		return 19;
	} else {
		co_outbuf[2] = 15;
		co_outbuf[18] = para;
		co_outbuf[19] = xbee_set_checksum(&co_outbuf[3],16);
		return 20;
    }   
}

int xbee_localAT_frame_assemble(String cmd, char para) {
	if(!cmd) PARAMETER_ERROR("xbee_localAT_frame_assemble");
	co_outbuf[0] = ZIGBEE_START_DELIMITER;
    co_outbuf[1] = 00;
    //co_outbuf[2] = len%256;
    co_outbuf[3] = 0x08;  //frame type
    co_outbuf[4] = 0x01;  //frame id
    co_outbuf[5] = cmd[0];
    co_outbuf[6] = cmd[1];
    
    if(para=='Q') { //query, no para
		co_outbuf[2] = 4;
		co_outbuf[7] = xbee_set_checksum(&co_outbuf[3],4);
		return 8;
	} else {
		co_outbuf[2] = 5;
		co_outbuf[7] = para;
		co_outbuf[8] = xbee_set_checksum(&co_outbuf[3],5);
		return 9;
    }   
}

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
		//printf("%d %d %d ",i,j,co_outbuf[i+j+1]);
		buf[i+j+1] ^= 0x20;   //XOR
		//printf("%d\n",co_outbuf[i+j+1]);
		buf[i+j] = 0x7D;
	}	
	//co_outbuf[2] += esc[0];	//change length	
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
	printf("%s %ld\n",(char*)(mqtt->msg),mqtt->addr);
	
	
	return mqtt;
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
	//tty_serial_write(tty_fds,co_outbuf,bytes);
	
	return mqtt;
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
	int tty_fds = open(tty_name,O_RDWR | O_NOCTTY | O_NONBLOCK);
	if(tty_fds<0) {
        perror("open tty error");
        exit(EXIT_FAILURE);
	} 
	return tty_fds;
}

void tty_serial_config(int fds) {
	
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
	
	if((co_inbuf = malloc(MAX_FRAME_SIZE)) == NULL ) {
		printf("%s\n","fail to allocate memory for co_inbuf...");
		exit(EXIT_FAILURE);
	} else {
		memset(co_inbuf,0,MAX_FRAME_SIZE);
	}

	if((co_outbuf = malloc(MAX_FRAME_SIZE)) == NULL ) {
		printf("%s\n","fail to allocate memory for co_outbuf...");
		exit(EXIT_FAILURE);
	} else {
		memset(co_outbuf,0,MAX_FRAME_SIZE);
	}	

	/*
	//BROADCAST to use API mode 2
	char mac_addr[8] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF };
	char ntw_addr[2] = { 0xFF, 0xFE };
	int bytes = xbee_remoteAT_frame_assemble(mac_addr,ntw_addr,2, "AP", 2 );
	tty_serial_write(tty_fds, co_outbuf, bytes); */
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
	
	leng = co_inbuf[1]*256 + co_inbuf[2] +1;  //including the checksum bit, be careful to use co_inbuf cauze tbuf is moving!!
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
			if(co_inbuf[i]==0x7D) {
				printf("the byte after escape: %d %d\n",i, co_inbuf[i+1]);
				//int c = tbuf[1+i];
				//if(c==0x7E || c==0x11 || c==0x13 || c==0x7D) {
				nr_escapes++;
			}
		} 
	} while(nr_escapes);
	
	/*
	#ifndef DEBUG
	leng = bytes;
	String fbuf = co_inbuf;
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
	#endif  */
	
	//descape and check checksum
	bytes = xbee_frame_descape(co_inbuf,bytes);		
	if(xbee_set_checksum(&co_inbuf[3],bytes-3)!=0) {
		printf("%s\n","checksum error...");
		//memset(co_inbuf,0,bytes);
		return 0;
	} else {
		printf("%s total bytes after descape: %d\n","checksum perfect!",bytes);
	}
	
	/*
	#ifndef DEBUG
	leng = bytes;
	fbuf = co_inbuf;
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
	#endif   */
	
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
            mystring
************************************/

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

