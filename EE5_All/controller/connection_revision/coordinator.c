#include "coordinator.h"

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

void run_coordinator(int fd_pipe_from_bridge, int fd_pipe_to_bridge);
void* coordinator_from_zigbee(void* arg);
void* coordinator_to_zigbee(void* arg);

Byte xbee_set_checksum(String xbee_data, int len);
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
            ttyUSB
************************************/

#define MAX_FRAME_SIZE   200
#define MODEM_DEVICE "/dev/ttyUSB0"

int tty_open(char* tty_name);
void tty_serial_config(int fds);
void tty_serial_init();
int tty_serial_read(int fds,String buf);
int tty_serial_write(int fds,String buf,int len);


/***********************************
            mystring
************************************/

char* str_get_between_index( char* str, int len, int bn, int en);


/***********************************
          global variables
************************************/
extern String co_inbuf;
extern String co_outbuf;
extern int tty_fds;
uint8_t shutdown;

/*-------------------------------------------------------------------------------*/
uint32_t coordinator_start(int* child_pid, int* file_descriptor_write, int* file_descriptor_read){
/*-------------------------------------------------------------------------------*/	
if(database_name == NULL)
	return DATABASE_INVALID_INPUT;

int fd_in[2];
int fd_out[2];

int err_code = pipe(fd_in);
if(err_code != 0){
	return 1;
}

err_code = pipe(fd_out);
if(err_code != 0){
	return 1;
}

int return_code = 0;

/* block signals until initialization is done */
signal(SIGTERM, SIG_IGN);
signal(SIGQUIT, SIG_IGN); 

return_code = fork();
if(return_code <0){
	return -1;
}

if(return_code == 0){
	shutdown = 0;

	close(fd_in[1]); // close the write descriptor to the input pipe
	close(fd_out[0]); // close the read descriptor to the output pipe
	
	signal(SIGQUIT, signal_handler);
	signal(SIGTERM, signal_handler);

	run_coordinator(fd_in[0], fd_out[1]);
	/* for safety, an unreachable exit */
	exit(1);
} else {
	
	*child_pid = return_code;
	signal(SIGTERM, SIG_DFL);
	signal(SIGQUIT, SIG_DFL);

	*child_pid = return_code;
	close(fd_in[0]); // close the read descriptor to the input pipe
//	close(fd_out[1]); // close the write descriptor to the output pipe
	*file_descriptor_write = fd_in[1];
	*file_descriptor_read = fd_out[0];
}
return DATABASE_OK;
}

void signal_handler(int sig){
	if(sig == SIGTERM)
		shutdown = 1;

	if(sig == SIGQUIT)
		exit(0);
}

/* creates an input and an output thread for reading and writing messages */
/*-------------------------------------------------------------------------------*/
void run_coordinator(int fd_pipe_from_bridge, int fd_pipe_to_bridge) {
/*-------------------------------------------------------------------------------*/	
	tty_serial_init();

	pthread_t pthread_read_from_zigbee, pthread_send_to_zigbee;

	int* tmp_fd_pipe_from_bridge = (int*) malloc(sizeof(int));
	int* tmp_fd_pipe_to_bridge = (int*) malloc(sizeof(int));

	*tmp_fd_pipe_to_bridge = fd_pipe_to_bridge;
	*tmp_fd_pipe_from_bridge = fd_pipe_from_bridge;

	int error = 0;

	void* return_value_read_from_zigbee;
	void* return_value_write_to_zigbee;

	error = pthread_create(&pthread_read_from_zigbee, NULL, coordinator_from_zigbee, tmp_fd_pipe_to_bridge);
	if( error != 0 ){
   	 DEBUG_PRINT("can't create thread to read from the zigbee network: %s\n",strerror(err));
		 exit(EXIT_FAILURE);
	}

	error = pthread_create(&pthread_send_to_zigbee, NULL, coordinator_to_zigbee, tmp_fd_pipe_from_bridge);
	
	if( erro != 0) {
		DEBUG_PRINT("can't creat the thread to send messages to the zigbee network: %s\n",strerror(err));
		exit(EXIT_FAILURE);
	}

	pthread_join(pthread_read_from_zigbee, &return_value_read_from_zigbee);
	pthread_join(pthread_send_to_zigbee, &return_value_write_to_zigbee);

	exit(EXIT_FAILURE);
}

/* read data from the zigbee network */
/*-------------------------------------------------------------------------------*/
void* coordinator_from_zigbee(void* arg) {
/*-------------------------------------------------------------------------------*/	

	int fd_pipe_to_bridge = *((int*) arg);
	free(arg);

	while(shutdown != 1) {
		
		int bytes = 0;
		bytes = tty_serial_read(tty_fds, co_inbuf);
		
		DEBUG_PRINT("received frame length: %d %d %d %d %d\n\n",bytes,co_inbuf[2],co_inbuf[co_inbuf[2]+3],co_inbuf[5],co_inbuf[6]);
		
		mqtt_ptr_t mydata = xbee_data_read(&co_inbuf[3],bytes-4); //only the data section, excluding checksum
		if(mydata) {
			Byte nbytes = sizeof(*mydata) + mydata->len;   //actual data bytes
			void* bytestream = malloc(nbytes+1);
			*(Byte*)bytestream = nbytes;
			memcpy(bytestream+1, mydata, sizeof(*mydata));
			memcpy(bytestream+sizeof(*mydata)+1, mydata->msg, mydata->len);
			write(fd_pipe_to_bridge, bytestream, nbytes+1);
			free(mydata->msg);
			free(mydata);
			free(bytestream);
		}
	}
	pthread_exit(NULL);
}

void* coordinator_to_zigbee(void* arg) {

	int fd_pipe_from_bridge = *((int*) arg);
	free(arg);

    while(1) {
        uint32_t total_length = 0;

        /* read the first byte, this is the length */
        int bytes_read = read(fd_pipe_from_bridge, &total_length ,1);

        if(bytes_read != 1)
            exit(1)

        if( total_length <= sizeof(MQTT)){
            DEBUG_PRINT("%s\n","mqtt data too short...");
            continue ;
        }

        uint32_t size_header = sizeof(MQTT);
        uint32_t size_data = total_length - size_header;

        mqtt_ptr_t mqtt = malloc( size_header);
        mqtt->msg =  malloc(size_data);

        bytes_read = read(fd_pipe_from_bridge, mqtt, size_header);

        if(bytes_read != size_header)
            exit(1)

        bytes_read = read(fd_pipe_from_bridge, data, size_data);

        if(bytes_read != size_data)
            exit(1)

        if(mqtt->len != *(uint8_t*)data){
            DEBUG_PRINT("%s\n","unmatched length field...");
            free(mqtt->msg);
            free(mqtt);
            continue ;
        }

        uint64_t address = mqtt->addr;

        String mac_addr = malloc(sizeof(uint64_t));
        
        int i;
        for(i=0; i<8; i++) {
            mac_addr[7-i] = address%256;
            address /= 256;
        }

        char ntw_addr[2] = {0xFF,0xFE};
        String RFData = (String)data;

        bytes = xbee_txReq_frame_assemble(mac_addr,ntw_addr,0,0,RFData,mqtt->len);

        bytes = tty_serial_write(tty_fds,co_outbuf,bytes);
        DEBUG_PRINT("write %d bytes... %d\n",bytes,*(Byte*)data);

        free(mqtt->msg);
        free(mqtt);
        free(mac_addr);
    }
}



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
    int tty_fds = open(tty_name,O_RDWR | O_NOCTTY | O_APPEND);
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
	
	//input option:ignore parity check, don't map CR(Enter,carriage return,\r) to NL
	tio_new.c_iflag |= IGNPAR;
	tio_new.c_iflag &= ~(INLCR | ICRNL | IGNCR);
	tio_new.c_iflag &= ~(IXON | IXOFF | IXANY);
	

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
	
	
	//descape and check checksum
    bytes = xbee_frame_descape(co_inbuf,bytes);
    Byte cs = xbee_set_checksum(&co_inbuf[3],bytes-3);
	if(cs!=0) {
        if(cs == 3 && co_inbuf[38] == 10) {
			printf("%s\n","line return ...");
            //memset(co_inbuf,0,bytes);
		} else {
            printf("%s %d %d\n","checksum error...",cs,co_inbuf[38]);
			return 0;
		}
	} else {
        printf("%s total bytes after descape: %d seq: %d\n","checksum perfect!",bytes,co_inbuf[38]);
	}	

	
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
