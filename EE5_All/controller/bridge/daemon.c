#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>

#include "../mqtt_sn_db/mqtt_sn_db.h"
#include "err_handler.h"
#include "handle_incoming_message_v2.h"
#include "handle_output_message.h"
#include "interface.h"
#include "our_protocol.h"
#include "timeout.h"
#include "sorted_double_linked_list.h"
#include <pthread.h>
#include <poll.h>

#include "../connection/coordinator.h"
#include "../connection/mymqtt.h"
#include <sys/select.h>

void run_bridge();
void *read_database(void * p);
void *read_coordinator(void * p);
void *read_message_from_server(void * p);

void run_coordinator();
void coordinator_tty_read();
void coordinator_pipe_read();
void send_to_bridge();

int fd_write, fd_read; //data base
pid_t monitor_pid, bridge_pid, coordinator_pid, database_pid;
list_ptr_t list_in, list_out;

#define DEBUG_PRINT(...) \
  do{                                                     \
      printf("In %s in function %s at line %d:", __FILE__,__func__,__LINE__);\
      printf(__VA_ARGS__);\
  } while(0)

#define DEBUG


/*********************************************
             coordinator
**********************************************/
int up_stream_pipe[2];
int down_stream_pipe[2];

String co_inbuf;
String co_outbuf;
int tty_fds;

//1 to restart -1 to kill default 0
int KILL_COORDINATOR_OR_RESTART;
int KILL_DATABASE_OR_RESTART;
int KILL_SERVER_OR_RESTART;

int main(){
	
    int childExitStatus;

    DEBUG_PRINT("monitor process before create database.\n");

    database_open("mqtt_db",&database_pid,"database_err_message",&fd_write,&fd_read);

    DEBUG_PRINT("monitor process before create bridge.\n");
    
    pipe(up_stream_pipe);
	pipe(down_stream_pipe);
	
    bridge_pid = fork();

    if(bridge_pid == 0){
        run_bridge();
    }else{

        DEBUG_PRINT("monitor process before create coordinator.\n");
		coordinator_pid = fork();
        if(coordinator_pid == 0){
            run_coordinator();
        }
    }
    DEBUG_PRINT("monitor process wait for children.\n");
    
	waitpid(bridge_pid,&childExitStatus,0);
    waitpid(coordinator_pid,&childExitStatus,0);
    waitpid(database_pid,&childExitStatus,0);

    return 0;
}

void run_bridge(){

    list_in = list_initialization();
    list_out = list_initialization();

    init_timer1();
    init_timer2();

    pthread_t pth_server;
    pthread_t pth_coordinator;
    pthread_t pth_database;

    int *p1, *p2, *p3;

    if(pthread_create(&pth_coordinator,NULL,read_coordinator,NULL)){
        perror("create thread unsuccessful\n");
    }  
    if(pthread_create(&pth_database,NULL,read_database,NULL)){
        perror("create thread unsuccessful\n");
    }
    if(pthread_create(&pth_server,NULL,read_server,NULL)){
        perror("create thread unsuccessful\n");
    }
    
	if(pthread_join(pth_coordinator, (void**)&p1)){
        perror("join thread unsuccessful\n");
    }
    if(pthread_join(pth_database, (void**)&p2)){
        perror("join thread unsuccessful\n");
    }
    if(pthread_join(pth_server, (void**)&p3)){
        perror("join thread unsuccessful\n");
    }
    
    while(1);
}

void *read_database(void * p){

    while(1){
            read_from_db(&fd_read);
        }
    pthread_exit(NULL);
    return NULL;
    
}

void *read_coordinator(void * p){
    
    close(up_stream_pipe[1]);    //parent only read from up stream
	close(down_stream_pipe[0]);  //parent only write to down stream
	
	while(1) {
		Byte bytes = 0;
		read(up_stream_pipe[0],&bytes,1);
		if(bytes<=sizeof(MQTT)) {
			printf("%s\n","mqtt data too short...");
			return NULL;
		}
		mqtt_ptr_t mqtt = malloc(sizeof(*mqtt));
		void* data =  malloc(bytes-sizeof(*mqtt));
		read(up_stream_pipe[0],mqtt,sizeof(*mqtt));
		read(up_stream_pipe[0], data, bytes-sizeof(*mqtt));
		mqtt->msg = data;
	}
	/*		
	int status_flag = fcntl(up_stream_pipe[0],F_GETFL);
	fcntl(up_stream_pipe[0], F_SETFL, status_flag | O_ASYNC | O_NONBLOCK);	
	fcntl(up_stream_pipe[0], F_SETOWN, getpid());	
	
	signal(SIGIO,parent_read_callback);		
	signal(SIGCHLD,child_run);
				
	char c = 0;
	while(1) {
		read(STDIN_FILENO,&c,1);
		if(c=='q') {             //kill the child
			kill(pid,SIGUSR1);
		}
		if(c=='s') {
			printf("%s %d\n","parent begin to sending hello...",down_stream_pipe[1]);
			int bytes = write(down_stream_pipe[1],"hello from parent",20);
			printf("parent write %d bytes\n",bytes);
		}
		if(c=='k') {
			kill(pid,SIGSEGV);
		}
	}
	*/
}

void *server_read(void * p){

}

void run_coordinator() {
	
	tty_serial_init();
	close(up_stream_pipe[0]);    
	close(down_stream_pipe[1]); 
	
	struct timeval tv;
	fd_set readfds;
	
	FD_ZERO(&readfds);
	FD_SET(tty_fds,&readfds);
	FD_SET(down_stream_pipe[0],&readfds);
	
	tv.tv_sec = 5;
	tv.tv_usec = 0;
	
	while(1) {
		int ret = select(2,&readfds,NULL,NULL,&tv);
		if(ret == -1) {
			perror("select");
			exit(EXIT_FAILURE);
		} else if (ret==0) {
			printf("%s\n","5 seconds elapsed, no data available to read..");
		} else {    //nr fds ready to read			
			if(FD_SET(tty_fds,&readfds))      coordinator_tty_read();
			if(FD_SET(down_stream_pipe[0],&readfds)) coordinator_pipe_read();
		}
	}
	
	/*
	int status_flag = fcntl(down_stream_pipe[0],F_GETFL);
	fcntl(down_stream_pipe[0], F_SETFL, status_flag | O_ASYNC);
	fcntl(down_stream_pipe[0], F_SETOWN, getpid());
	
	signal(SIGIO,child_read_callback);
	signal(SIGSEGV,child_exit);    //block here..
	signal(SIGUSR1,child_exit);
	
	pthread_t pth_tty, pth_pipe;
	int err;
	void* ttyStatus;
	void* pipeStatus;
	
	if((err=pthread_create(&pth_tty,NULL,tty_read,NULL))!=0) {
		printf("can't create tty thread: %s\n",strerror(err));
		//exit(EXIT_FAILURE);
	}	
	if((err=pthread_create(&pth_pipe,NULL,pipe_read,NULL))!=0) {
		printf("can't create pipe thread: %s\n",strerror(err));
		//exit(EXIT_FAILURE);
	}
	pthread_join(pth_tty,&ttyStatus);	
	pthread_join(pth_pipe,&pipeStatus);
	*/
	
	exit(EXIT_FAILURE);
}	

void coordinator_tty_read() {
	
	int bytes = 0;
	bytes = tty_serial_read(tty_fds,co_inbuf);
	printf("received frame length: %d %d %d %d %d\n",bytes,co_inbuf[2],co_inbuf[co_inbuf[2]+3],co_inbuf[5],co_inbuf[6]);
	mqtt_ptr_t mydata = xbee_data_read(&co_inbuf[3],bytes-4); //only the data section, excluding checksum
	if(mydata) {      
		Byte nbytes = sizeof(*mydata) + mydata->len;   //actual data bytes
		void* bytestream = malloc(nbytes+1);
		*(Byte*)bytestream = nbytes;		
		memcpy(bytestream+1,mydata,sizeof(*mydata));
		memcpy(bytestream+sizeof(*mydata)+1,mydata->msg,mydata->len);		
		write(up_stream_pipe[1],bytestream,nbytes+1);
		free(mydata);
		free(bytestream);
	}
		
	/*
	int logfds = open("./rdesclog.txt", O_CREAT | O_RDWR | O_APPEND);
	int ret = 0, leng = bytes;
	String fbuf = co_inbuf;
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
}

void coordinator_pipe_read() {
	Byte bytes = 0;
	read(down_stream_pipe[0],&bytes,1);
	if(bytes<=sizeof(MQTT)) {
		printf("%s\n","mqtt data too short...");
		return NULL;
	}
	mqtt_ptr_t mqtt = malloc(sizeof(*mqtt));
	void* data =  malloc(bytes-sizeof(*mqtt));
	read(down_stream_pipe[0],mqtt,sizeof(*mqtt));
	read(down_stream_pipe[0], data, bytes-sizeof(*mqtt));
	//mqtt->msg = data;
	
	if(mqtt->len != *(Byte*)data) {
		printf("%s\n","unmatched length field...");
		return ;
	}
	
	uint64_t address = mqtt->addr;
	String mac_addr = malloc(8);
	int i;
	for(i=0; i<8; i++) {
		mac_addr[7-i] = address%256;
		address /= 256;
	}
	char ntw_addr[2] = {0xFF,0xFE};
	String RFData = (String)data;	
	//memcpy(RFData,mqtt->msg,mqtt->len);
	
	int bytes = xbee_txReq_frame_assemble(mac_addr,ntw_addr,0,0,RFData,mqtt->len);

	/*
	int logfds = open("./wresclog.txt", O_CREAT | O_RDWR | O_APPEND);
	int ret = 0;
	leng = bytes;
	String fbuf = co_outbuf;
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
	
	bytes = tty_serial_write(tty_fds,co_outbuf,bytes);
	printf("write %d bytes... %d\n",bytes,RFData[0]);
	
	free(data);
	free(mqtt);
	free(mac_addr);
	//free(RFData);
}

void send_to_bridge(){
    DEBUG_PRINT("I am in sen_to_bridge function.\n");
    mqtt_ptr_t out = (mqtt_ptr_t)malloc(size_MQTT);
    if(out == NULL){
        err_num = MALLOC_ERR;
        ErrHandler();
        return;
    }

    memset(out, 0, size_MQTT);

    out->addr = 2;
    out->len = 22;

    char *mess = (char *)malloc(22);

    char *temp = mess;

    *temp = 0x22;
    *(++temp) = 0x04;
    *(++temp) = 0x00;
    *(++temp) = 0x01;
    *(++temp) = 0x0004;

    char *client_id1 = (char *)malloc(16);
    if(out == NULL){
        err_num = MALLOC_ERR;
        ErrHandler();
        return;
    }
      snprintf(client_id1,16,"the first test.");

      char *client_id = client_id1;

            *(++temp) = *client_id;

    int i;
    for(i = 0; i<((out->len)-7);i++){
        *(++temp) = *(++client_id);
    }

    free(client_id1);
    out->msg = (void *)mess;
    out->ts = time(NULL);
DEBUG_PRINT("I am writing to bridge.\n");
    uint8_t written = write(fd_coordinator[1][0], out, size_MQTT);
    if(written != size_MQTT){
        //TODO do something
    } else {
            written = write(fd_coordinator[0][1], out->msg, out->len);
            if((unsigned char)written != (out->len)){
                //TODO do something (the same)
            } else {
                //TODO don't free but reuse
                free(mess);
            }
        }
    DEBUG_PRINT("I finish writing to bridge.\n");
    free(out);
}

int CHILD_RESTART;  

void init() {
	CHILD_RESTART = -1;
}

void child_exit(int signo) {
	printf("signal %d %s\n",signo,"child begin to exit...");
	exit(EXIT_FAILURE);
}

void child_cleanup() {
	printf("%s\n","child clean up before exit..");
}

void parent_read_callback(int signo) {
	char buf[20];
	printf("%s %d\n","data available, parent begin to read...",signo);
	int bytes = read(up_stream_pipe[0],buf,20);
	printf("parent receive data: %s, %d bytes, signal: %d\n",buf,bytes,signo);	
}

void child_read_callback(int signo) {
	char buf[10];
	printf("%s %d\n","data available, child begin to read...",signo);
	int bytes = read(down_stream_pipe[0],buf,20);
	printf("child receive data: %s, %d bytes signal: %d\n",buf,bytes,signo);	
	write(up_stream_pipe[1],"hello from child",20);
}

void child_run(int signo) {
	
	CHILD_RESTART++;
	if(CHILD_RESTART) {
		printf("child killed by signal %d, begin to restart child...\n",signo);
		close(up_stream_pipe[0]);
		close(down_stream_pipe[1]);
		//sleep(5);
	}
		
	pipe(up_stream_pipe);
	pipe(down_stream_pipe);
	
	pid_t pid = fork();
	if(pid<0) {
		printf("%s\n","fail to folk...");
	} else if(pid>0) {
		
		//if(!CHILD_RESTART) {   //just set at first time if case of error like when closing already closed fds
			
			close(up_stream_pipe[1]);    //parent only read from up stream
			close(down_stream_pipe[0]);  //parent only write to down stream
			
			int status_flag = fcntl(up_stream_pipe[0],F_GETFL);
			fcntl(up_stream_pipe[0], F_SETFL, status_flag | O_ASYNC | O_NONBLOCK);	
			fcntl(up_stream_pipe[0], F_SETOWN, getpid());	
		
			
			signal(SIGIO,parent_read_callback);		
			signal(SIGCHLD,child_run);
		//}
		
		char c = 0;
		while(1) {
			read(STDIN_FILENO,&c,1);
			if(c=='q') {             //kill the child
				kill(pid,SIGUSR1);
			}
			if(c=='s') {
				printf("%s %d\n","parent begin to sending hello...",down_stream_pipe[1]);
				int bytes = write(down_stream_pipe[1],"hello from parent",20);
				printf("parent write %d bytes\n",bytes);
			}
			if(c=='k') {
				kill(pid,SIGSEGV);
			}
		}
			
	} else {
		
		printf("%s %d %d\n","new child started...",getpid(),getppid());
		atexit(child_cleanup);
		
		close(up_stream_pipe[0]);    
		close(down_stream_pipe[1]); 
		
		int status_flag = fcntl(down_stream_pipe[0],F_GETFL);
		fcntl(down_stream_pipe[0], F_SETFL, status_flag | O_ASYNC | O_NONBLOCK);
		fcntl(down_stream_pipe[0], F_SETOWN, getpid());
		
		signal(SIGIO,child_read_callback);
		signal(SIGSEGV,child_exit);    //block here..
		signal(SIGUSR1,child_exit);
		
		while(1);
	}
}
