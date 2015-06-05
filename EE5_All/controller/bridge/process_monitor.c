/*******************************************************************************
*
* FILENAME: process_monitor.c
*
* Description:
* Handles the processes for the entire application
*
*******************************************************************************/

/*-----------------------------------------------------------------------------
        include files
------------------------------------------------------------------------------*/
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <stdint.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "handle_message_in.h"
#include "handle_message_out.h"
#include "interface.h"
#include "timeout.h"
#include "mqtt_sn_bridge.h"
#include "../mqtt_sn_db/mqtt_sn_db.h"
#include "../connection/coordinator.h"
#include "../connection/mymqtt.h"
#include "sorted_double_linked_list/sorted_double_linked_list.h"
#include "err_handler.h"

/*------------------------------------------------------------------------------
        definitions (defines, typedefs, ...)
------------------------------------------------------------------------------*/

#define DEBUG_PRINT(...) \
  do{                                                     \
      printf("In %s in function %s at line %d:", __FILE__,__func__,__LINE__);\
      printf(__VA_ARGS__);\
  } while(0)

#define DEBUG

/*------------------------------------------------------------------------------
        function prototypes
------------------------------------------------------------------------------*/

//child process function
void run_bridge();
void run_coordinator();

//thread funtions for bridge
void *read_db_t(void * p);
void *read_coordinator_t(void * p);
void *read_controller_t(void * p);

//test funtion
//void send_to_bridge();

//coordinator method
void* coordinator_tty_read(void *);
void* coordinator_pipe_read(void *);

//receive sigquit from controller, shutdown the monitor process and all the child process.
void sig_controller_handler(int sig);

//check the pid of exit process and terminate the other processes
void check_and_clean(pid_t pid_info);

   extern ERR_NUM err_num;

    //pid of all the processes.
    pid_t monitor_pid, bridge_pid, coordinator_pid, database_pid;

    //keep track of the exited child's pid
    pid_t pid_info;

    //data base pipe descriptor
    int fd_write_db, fd_read_db;

    //coordinator pipe descriptor
    int up_stream_pipe[2];
    int down_stream_pipe[2];

  //coordinator global variable
    String co_inbuf;
    String co_outbuf;
    int tty_fds;

    //1 to restart -1 to kill default 0
    //int KILL_COORDINATOR_OR_RESTART;
    //int KILL_DATABASE_OR_RESTART;
    //int KILL_SERVER_OR_RESTART;

    //initialize the list to store device information for timeout.
    list_ptr_t list_in, list_out;

    pthread_mutex_t mutex_write_db = PTHREAD_MUTEX_INITIALIZER;

/*------------------------------------------------------------------------------
          implementation code
------------------------------------------------------------------------------*/
int main(int argc, char **argv){

 //   signal(SIGQUIT,sig_controller_handler);

    while(1){
   int childstatus;
    FILE *log_child_status = fopen("child_exit_status","a+");

    //initialize coordinator pipe
    pipe(up_stream_pipe);
    pipe(down_stream_pipe);


    database_open(argv[1],&database_pid,"database_err_message",&fd_write_db,&fd_read_db);


    fprintf(log_child_status,"the pid of database process is %d \n",database_pid);
//    printf("the pid of database process is %d \n",database_pid);

    bridge_pid = fork();

    if(bridge_pid == 0){
        run_bridge();
        exit(1);
    }else{

    fprintf(log_child_status,"the pid of bridge process is %d \n",bridge_pid);
  //   printf("the pid of bridge process is %d \n",bridge_pid);

        coordinator_pid = fork();
        if(coordinator_pid == 0){
            run_coordinator();
            exit(1);
        }
        fprintf(log_child_status,"the pid of coordinator process is %d \n",coordinator_pid);
 //       printf("the pid of coordinator process is %d \n",coordinator_pid);
    }

    fclose(log_child_status);
    DEBUG_PRINT("wait for child process.\n");

       pid_info = wait(&childstatus);
        if(WIFEXITED(childstatus)){
         log_child_status = fopen("child_exit_status","a+");
          fclose(log_child_status);

       printf("child process %d exit with status: %d \n",pid_info,WEXITSTATUS(childstatus));
        }
        check_and_clean(pid_info);
  //      printf("the pid of the first exit process %d \n",pid_info);

//        pid_info = wait(&childstatus);
//         if(WIFEXITED(childstatus)){
//          log_child_status = fopen("child_exit_status","a+");
//           fclose(log_child_status);
//          check_and_clean(pid_info);
//  //      printf("child process %d exit with status: %d \n",pid_info,WEXITSTATUS(childstatus));
//check_and_clean(pid_info);
 // }

 //        printf("the pid of the first exit process %d \n",pid_info);

sleep(5);
    }
    return 0;
}

void check_and_clean(pid_t pid_info){
    if(pid_info == database_pid){
        kill(bridge_pid,SIGTERM);
        kill(coordinator_pid,SIGTERM);
    }else if(pid_info == bridge_pid){
        kill(database_pid,SIGTERM);
        kill(coordinator_pid,SIGTERM);
    }else if(pid_info == coordinator_pid){
        kill(database_pid,SIGTERM);
        kill(bridge_pid,SIGTERM);
    }else{
        err_num = CHILD_EXIT_ERR;
        ErrHandler();
    }
}

void sig_controller_handler(int sig){
    kill(database_pid,SIGTERM);
    kill(bridge_pid,SIGTERM);
    kill(coordinator_pid,SIGTERM);
    sleep(1);
    raise(SIGTERM);
}

/*------------------------------------------------------------------------------
          code for bridge process
------------------------------------------------------------------------------*/
void run_bridge(){

    close(up_stream_pipe[1]);    //parent only read from up stream
    close(down_stream_pipe[0]);  //parent only write to down stream

//    list_in = list_initialization();
//    list_out = list_initialization();

//    init_timer();

//    int status_flag = fcntl(up_stream_pipe[0],F_GETFL);
//    DEBUG_PRINT("%d\n",status_flag);

    pthread_t read_coordinator_thread;
    pthread_t read_db_thread;
    pthread_t read_controller_thread;

     int *p1, *p2, *p3;

 //   printf("CREATE READ COORDINATOR THREAD\n");
    if(pthread_create(&read_coordinator_thread,NULL,read_coordinator_t,NULL)){
        perror("create thread unsuccessful\n");
    }


  //  printf("CREATE READ DATABASE THREAD\n");
    if(pthread_create(&read_db_thread,NULL,read_db_t,NULL)){
        perror("create thread unsuccessful\n");
    }

//    char c = 0;
//    while(1) {

//        read(STDIN_FILENO,&c,1);
//        if(c=='q') {             //kill the child
//            //kill(pid,SIGUSR1);
//        }
//        if(c=='s') {
//            printf("%s %d\n","parent begin to sending hello...",down_stream_pipe[1]);
//            int bytes = write(down_stream_pipe[1],"hello from parent",17);
//            printf("parent write %d bytes\n",bytes);
//        }
//        if(c=='k') {
//            //kill(pid,SIGSEGV);
//        }
//
//    }

	printf("CREATE READ CONTROLLER THREAD\n");
	if(pthread_create(&read_controller_thread,NULL,read_controller_t,NULL)){
    	perror("create thread unsuccessful\n");
	}

    if(pthread_join(read_coordinator_thread, (void**)&p1)){
        perror("join thread unsuccessful\n");
    }

    if(pthread_join(read_db_thread, (void**)&p2)){
        perror("join thread unsuccessful\n");
    }

    if(pthread_join(read_controller_thread, (void**)&p3)){
        perror("join thread unsuccessful\n");
   }

    exit(EXIT_FAILURE);
}

void *read_db_t(void * p){

    while(1){
  //      DEBUG_PRINT("WAITING TO READ FROM DATA BASE.\n");
            read_db(&fd_read_db);
  //      DEBUG_PRINT("FINISH READING FROM DATA BASE.\n");
            sleep(1);
        }
    pthread_exit(NULL);
    return NULL;
}

void *read_coordinator_t(void * p){

    while(1) {
 //       DEBUG_PRINT("START READ FROM COORDINATOR.\n");
       read_connection();
 //      DEBUG_PRINT("FINISH READ FROM COORDINATOR.\n");
       sleep(1);
    }
    pthread_exit(NULL);
    return NULL;
}

void *read_controller_t(void * p){

    while(1){
 //       DEBUG_PRINT("WAITING TO READ FROM JAVA.\n");
        read_java();
 //       DEBUG_PRINT("FINISH READ FROM JAVA.\n");
        sleep(1);
    }
    pthread_exit(NULL);
    return NULL;
}


/*------------------------------------------------------------------------------
          code for coordinator process
------------------------------------------------------------------------------*/
void run_coordinator() {


tty_serial_init();
close(up_stream_pipe[0]);
close(down_stream_pipe[1]);

/*
while(1) {

    struct timeval tv;
    fd_set readfds;

    FD_ZERO(&readfds);
    FD_SET(tty_fds,&readfds);
    FD_SET(down_stream_pipe[0],&readfds);

    tv.tv_sec = 5;
    tv.tv_usec = 0;

    int ret = select(2,&readfds,NULL,NULL,&tv);
    if(ret == -1) {
        perror("select");
        exit(EXIT_FAILURE);
    } else if (ret==0) {
        printf("%s\n","5 seconds elapsed, no data available to read...");
        //sleep(5);
    } else {    //nr fds ready to read
        if(FD_ISSET(tty_fds,&readfds))      coordinator_tty_read();
        if(FD_ISSET(down_stream_pipe[0],&readfds)) coordinator_pipe_read();
    }
}  */

pthread_t pth_tty, pth_pipe;
int err;
void* ttyStatus;
void* pipeStatus;

if((err=pthread_create(&pth_tty,NULL,coordinator_tty_read,NULL))!=0) {
    //exit(EXIT_FAILURE);
}
if((err=pthread_create(&pth_pipe,NULL,coordinator_pipe_read,NULL))!=0) {
    //exit(EXIT_FAILURE);
}

pthread_join(pth_tty,&ttyStatus);
pthread_join(pth_pipe,&pipeStatus);

exit(EXIT_FAILURE);
}

void* coordinator_tty_read(void* arg) {


while(1) {
    int bytes = 0;
    bytes = tty_serial_read(tty_fds,co_inbuf);
    DEBUG_PRINT("received frame length: %d %d %d %d %d\n\n",bytes,co_inbuf[2],co_inbuf[co_inbuf[2]+3],co_inbuf[5],co_inbuf[6]);


    mqtt_ptr_t mydata = xbee_data_read(&co_inbuf[3],bytes-4); //only the data section, excluding checksum
    if(mydata) {
        Byte nbytes = sizeof(*mydata) + mydata->len;   //actual data bytes
        void* bytestream = malloc(nbytes+1);
        *(Byte*)bytestream = nbytes;
        memcpy(bytestream+1, mydata, sizeof(*mydata));
        memcpy(bytestream+sizeof(*mydata)+1, mydata->msg, mydata->len);
        write(up_stream_pipe[1], bytestream, nbytes+1);
        free(mydata->msg);
        free(mydata);
        free(bytestream);
    }

//    mqtt_ptr_t mqtt = malloc(sizeof(*mqtt));
//    mqtt->addr = 12345;
//    mqtt->len = 6;
//    mqtt->ts = time(NULL);
//    mqtt->msg = malloc(6);
//    *(Byte*)mqtt->msg = 6;
//    memcpy(mqtt->msg+1, "hello", 5);

//    Byte nbytes = sizeof(*mqtt) + mqtt->len;   //actual data bytes
//    void* bytestream = malloc(nbytes+1);
//    *(Byte*)bytestream = nbytes;
//    memcpy(bytestream+1, mqtt, sizeof(*mqtt));
//    memcpy(bytestream+sizeof(*mqtt)+1, mqtt->msg, mqtt->len);
//    DEBUG_PRINT("coordinator write %d bytes to pipe...\n",nbytes);
//    nbytes = write(up_stream_pipe[1], bytestream, nbytes+1);
//    free(mqtt->msg);
//    free(mqtt);
//    free(bytestream);
//    DEBUG_PRINT("coordinator write %d bytes to pipe...\n",nbytes);
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

void* coordinator_pipe_read(void* arg) {

    DEBUG_PRINT("%s %d\n","coordinator_pipe_read thread read pipe", down_stream_pipe[0]);

    while(1) {
        Byte bytes = 0;
        DEBUG_PRINT("%s %d\n","coordinator_pipe_read thread read pipe read from bridge", down_stream_pipe[0]);
        read(down_stream_pipe[0],&bytes,1);
        DEBUG_PRINT("%s %d\n","coordinator_pipe_read thread read pipe read from bridge finish", down_stream_pipe[0]);
        if(bytes<=sizeof(MQTT)) {
            DEBUG_PRINT("%s\n","mqtt data too short...");
            continue ;
        }
        mqtt_ptr_t mqtt = malloc(sizeof(*mqtt));
        void* data =  malloc(bytes-sizeof(*mqtt));

        read(down_stream_pipe[0],mqtt,sizeof(*mqtt));
        read(down_stream_pipe[0], data, bytes-sizeof(*mqtt));
        mqtt->msg = data;

        if(mqtt->len != *(Byte*)data) {
            DEBUG_PRINT("%s\n","unmatched length field...");
            free(data);
            free(mqtt);
            continue ;
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

        bytes = xbee_txReq_frame_assemble(mac_addr,ntw_addr,0,0,RFData,mqtt->len);

        /*
        int logfds = open("./wwresclog.txt", O_CREAT | O_RDWR | O_APPEND);
        int ret = 0;
        int leng = bytes;
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
        if(close(logfds)==-1) perror("close log file");  */


        bytes = tty_serial_write(tty_fds,co_outbuf,bytes);
        DEBUG_PRINT("write %d bytes... %d\n",bytes,*(Byte*)data);
        //free(RFData);
        free(data);
        free(mqtt);
        free(mac_addr);

    }



}



