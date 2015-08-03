/*******************************************************************************
       send data from sensor to server and upload to database, multi-threading									    
*******************************************************************************/
#include <my_global.h>
#include <mysql.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <sys/time.h> 
#include <netinet/in.h>
#include <assert.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "tcpsocket.h"
#include "gateway.h"
#include "linkedlist.h"
#include "statistic.h"
#include "thread.h"
#include "my_queue.h"


/*------------------------------------------------------------------------------
		macro definition
------------------------------------------------------------------------------*/
<<<<<<< HEAD
#define THREADNUM 5

//attention: here for req 12
#ifndef SET_QUEUE_SIZE
#define SET_QUEUE_SIZE 100
#endif

//attention: here for req 11
#ifndef SET_MIN_TEMP
#define SET_MIN_TEMP 5
#endif

//attention: here for req 11
#ifndef SET_MAX_TEMP
#define SET_MAX_TEMP 200
=======
#define THREADNUM 10

//attention: here for req 12
#ifndef SET_QUEUE_SIZE
#define SET_QUEUE_SIZE 30000
#endif

//attention: here for req 11
#ifndef MIN_TEMP
#define MIN_TEMP 5
#endif

//attention: here for req 11
#ifndef MAX_TEMP
#define MAX_TEMP 200
>>>>>>> 72ab7c9359283513d60aec90b8ff12ff7c918271
#endif

#define FIFO_NAME "logFifo"

<<<<<<< HEAD

int *temp_aver;
Queue *q_create;

int fifo_fds;
int server_port;
char logbuf[100];
int QUEUE_SIZE;
MYSQL *conn;

packet_ptr_t sensor_packet_temp;


=======
/*------------------------------------------------------------------------------
		variable declare
------------------------------------------------------------------------------*/

	packet_ptr_t sensor_packet_temp;
 
	int *temp_aver;
	int *tenDataFlag;
	
	MYSQL *conn;
	
	Queue *q_create;

    int fifo_fds;
	int server_port;
	char logbuf[100];
    int QUEUE_SIZE;  //EXTERN TO my_queue.c

/*------------------------------------------------------------------------------
		function declare
------------------------------------------------------------------------------*/
                             
>>>>>>> 72ab7c9359283513d60aec90b8ff12ff7c918271
void * thread_task_TCP_Con (void *arg);
void * ReadqueueToDataBase(void *arg);
static void exit_clean();

/*------------------------------------------------------------------------------
		main
------------------------------------------------------------------------------*/

int main( int argc, char *argv[] )
{
    Socket server,client;   

    if(argc != 2)
    {
       printf("Incorrect number of args\n");
       exit(-1);
    }
<<<<<<< HEAD

    server_port = (int)strtol(argv[1],NULL,10);  //server port
    if(server_port <= 0 || server_port > 65535){
=======
    
    server_port = (int)strtol(argv[1],NULL,10);
    if(server_port <= 0 || server_port > 65535)
    {
>>>>>>> 72ab7c9359283513d60aec90b8ff12ff7c918271
       printf("invalid port\n");
       exit(-2);
    }
    
/*****************************************************************************
						variable initialization
***************************************************************************** */

<<<<<<< HEAD
    //tasks maxim number
    sensor_malloc();
    temp_aver=(int*)malloc(Sensor_Number*sizeof(int));

    //pool initial
    pool=(CThread_pool *)malloc(sizeof(CThread_pool));

    QUEUE_SIZE = SET_QUEUE_SIZE;
    q_create = QueueCreate();

    struct stat st;
    if (stat(FIFO_NAME, &st) != 0)
        mkfifo(FIFO_NAME, 0666);

    pid_t pid = fork();
    if(pid>0) {

        fifo_fds = open(FIFO_NAME, O_WRONLY);
        if(fifo_fds<0) {
            perror("parent open fifo for writing error");
            exit(EXIT_FAILURE);
        }

        conn = mysql_init(NULL);
        mysql_real_connect(conn, "studev.groept.be", "a13_syssoft", "a13_syssoft", "a13_syssoft", 0, NULL, 0);
    //    mysql_real_connect(conn, "localhost", "root", "root", "sensor", 0, NULL, 0);
        printf("sql connection created.. ***\n");

        //initial thread pool
        pool_init (THREADNUM);

        //create server and get their fd
        server=tcp_passive_open(server_port);

        atexit(exit_clean);

        while(1) {
            client = tcp_wait_for_connection( server);
            printf("new client connection.. ***\n");
=======
    //use an array to store all infor: sensor data list, tmp ...
    sensor_malloc();
	temp_aver=(int*)malloc(Sensor_Number*sizeof(int));

    //pool initial, included from thread.h
    pool=(CThread_pool *)malloc(sizeof(CThread_pool));

    QUEUE_SIZE = SET_QUEUE_SIZE;

    q_create=QueueCreate();

    // create FIFO if not exists
    struct stat st;
    if ( stat( FIFO_NAME, &st) != 0)
        mkfifo( FIFO_NAME, 0666);

    atexit(exit_clean);

    pid_t pid = 3; //fork();
    if(pid>0) {

        fifo_fds = open(FIFO_NAME, O_WRONLY);

        /*****************************************************************************
                                SQL connection
        ***************************************************************************** */
        conn = mysql_init(NULL);
    //for GT
    //	mysql_real_connect(conn, "studev.groept.be", "a13_syssoft", "a13_syssoft", "Xuemei_Lu", 0, NULL, 0);
    //for my own
        mysql_real_connect(conn, "localhost", "root", "root", "sensor", 0, NULL, 0);

    /*****************************************************************************
                        Create	Multi-Thread & connection manager
    ***************************************************************************** */

        pool_init (THREADNUM);
        server=tcp_passive_open(server_port);

        while(1){
            client = tcp_wait_for_connection( server);
            printf("new client connection created...\n");
>>>>>>> 72ab7c9359283513d60aec90b8ff12ff7c918271

            //add task to new worker
            pool_add_worker (thread_task_TCP_Con, client);  /////////

            //write new connection infor to FIFO-->log file
<<<<<<< HEAD
            sprintf(logbuf, "Sensor node connection with Ip address %s, socket port %d  has  connected;\n",get_ip_addr(client),server_port);
            printf("  \nwrite ip address & port to fifo:  %s\n",logbuf);
            write(fifo_fds, logbuf, strlen(logbuf)+1);

            //read data from the queue and save it into the database
            pool_add_worker (ReadqueueToDataBase, client);	 /////////
        }

        int retStatus;
        waitpid(pid, &retStatus, 0);
        printf("child process exit with status \%d\n", retStatus);

    } else if (pid==0) {
        fifo_fds = open(FIFO_NAME, O_RDONLY);
        if(fifo_fds<0) {
            perror("child open fifo for reading error");
            exit(EXIT_FAILURE);
        }

        int log_fds = open("gateway.log",O_CREAT | O_WRONLY | O_APPEND, S_IRWXU | S_IRWXG);
        if(log_fds<0) {
            perror("child open log file error");
            exit(EXIT_FAILURE);
        }

        char buf[100];
        memset(buf,0,100);
        while(1) {
            int bytes = read(fifo_fds,buf,100);
            if(bytes==-1) perror("reading fifo");
            else if(bytes>0) write(log_fds,buf,bytes);
//			sleep(60);
        }

    } else {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    return 0;
=======
            sprintf(logbuf, "Sensor node connection with ip address %s and port %d has connected\n",get_ip_addr(client),server_port);
            printf("write new tcp/ip connection event to fifo:\n\t%s\n",logbuf);
            write(fifo_fds, logbuf, strlen(logbuf)+1);

            //read data from the queue and save it into the database
            pool_add_worker (ReadqueueToDataBase, client);
        }
   }

////        waitpid(pid, NULL, 0);

//    } else if (pid==0) {
////        printf("child process created\n");

////        int cfds = open(FIFO_NAME, O_RDONLY);
////        if(cfds<0) {
////            perror("child open fifo for reading error");
////            exit(EXIT_FAILURE);
////        }

////        printf("child process opened FIFO for reading\n");

////        int log_fds = open("gateway.log",O_CREAT | O_WRONLY | O_APPEND, S_IRWXU | S_IRWXG);
////        if(log_fds<0) {
////            perror("child open log file error");
////            exit(EXIT_FAILURE);
////        }

////        char buf[100];
////        memset(buf,0,100);
////        while(1) {
////            int bytes = read(cfds,buf,100);
////            if(bytes==-1) perror("reading fifo");
////            else if(bytes>0) write(log_fds,buf,bytes);
////            printf("child process read new log message: %s\n", buf);
////        }

//    } else {
//        perror("fork");
//        exit(EXIT_FAILURE);
//    }

    return 0;

>>>>>>> 72ab7c9359283513d60aec90b8ff12ff7c918271
}


/*****************************************************************************
			thread task for new TCP Connection
***************************************************************************** */ 
void * thread_task_TCP_Con (void *arg)
{
	printf ("\nthreadid is 0x%x\n", (int)pthread_self());
	Socket client;
	int bytes,i,ID_buffer;
	long int sensorData;

<<<<<<< HEAD
	client=arg;	 
	bytes=1;    //tcp receiver > 0 => do

     while(bytes>0)
     {	
	    bytes = tcp_receive( client, buffer0, BUFSIZE ); //statistic
	    sensorData=atoi(buffer0);
	    data_buffer=sensorData;    //statistic :get sensor id
	   
	    printf("\nreceived data is: %ld",sensorData );
	    
	    sensor_packet_temp=(packet_ptr_t)&sensorData;
//attention: here for req 9	    
	    sensor_data.timestamp=time(NULL);   //statistic    gateway
	    sensor_data.value=sensor_packet_temp->value;    
	    sensor_data.id=sensor_packet_temp->id;
	    
        printf("\n>>>>>>>>>>>>ddddddddddd %d >>>>>>\n",sensor_data.id);
	    
	    sensor_packet_temp->parity=parityCheck(sensor_packet_temp);
	    
	    //echo msg back to client
	    tcp_send( client, (void*)buffer0, bytes );

	    ID_buffer=sensor_data.id;	 

	/*****************************************************************************
			   		Data Compression
	***************************************************************************** */ 
        data_manipulation(ID_buffer);
	    

	/*****************************************************************************
			   		send infor to queue, 
					log event if something happens
	***************************************************************************** */ 
	    //copy data into my pointers: temp_aver[]  	
	    for(i=0;i<Sensor_Number;i++)
	    {
	         if(IDFlagCheck[i])
	         {
                temp_aver[i]=tempAver[i];
  	         }
	    }

	   {
        if(QUEUE_SIZE>QueueSize(q_create))
		{
			long int data_averaged=(long int)((sensor_data.id<<20)+((long int)(temp_aver[sensor_data.id])<<1));
			packet_ptr_t sensor_data_tempera=(packet_ptr_t)&data_averaged;	
			
			int temperatureTemp=sensor_data_tempera->value/10;


			Enqueue(q_create,(DATATYPE)data_averaged);
			

			if(temperatureTemp<SET_MIN_TEMP)
			{
				sprintf(logbuf,"Temperature of sensor %d is too low: %d °C;\n",sensor_data.id%30,temperatureTemp); // in the whole project, i only show the data of sensor id from 0 to 30
				printf("write low T to fifo:  %s\n",logbuf);
                    write(fifo_fds, logbuf, strlen(logbuf)+1);
				perror("Write:"); //Very crude error check
   			        memset(logbuf, 0, sizeof(logbuf));
			}

			else if(temperatureTemp>SET_MAX_TEMP)
			{
				sprintf(logbuf,"Temperature of sensor %d is too high: %d °C;\n",sensor_data.id%30,temperatureTemp);// in the whole project, i only show the data of sensor id from 0 to 30
				printf("write high T to fifo:  %s\n",logbuf);
                    write(fifo_fds, logbuf, strlen(logbuf)+1);
				perror("Write:"); //Very crude error check
   			        memset(logbuf, 0, sizeof(logbuf));
			}

			else
			{
				//do nothing:just print
				sprintf(logbuf,"Temperature of sensor %d is OKAY: %d °C;\n",sensor_data.id%30,temperatureTemp);// in the whole project, i only show the data of sensor id from 0 to 30
				printf("  write fifo:  %s\n",logbuf);
			}

		}

	   }
    }
    

=======
    client=arg;

    bytes = tcp_receive( client, buffer0, BUFSIZE ); //statistic


    /**********************************************************
                sensor data communication
                related variables are defined in statistics.h
    ************************************************************/

    sensorData=atoi(buffer0);
    data_buffer=sensorData;    //statistic: get sensor id

    printf("\nreceived data length: %d bytes %ld\n",bytes,sensorData );

    sensor_packet_temp = (packet_ptr_t)&sensorData;
    sensor_data.timestamp=time(NULL);   //statistic gateway
    sensor_data.value=sensor_packet_temp->value;
    sensor_data.id=sensor_packet_temp->id;

    printf("sensor_data id: %d\n",sensor_data.id);

    sensor_packet_temp->parity=parityCheck(sensor_packet_temp);

    //echo msg back to client
    //	    tcp_send( client, (void*)buffer0, bytes );

    ID_buffer=sensor_data.id;

    /*****************************************************************************
                Data Compression
    ***************************************************************************** */
    printf("\ndata compression\n");
    data_manipulation(ID_buffer);

    /*****************************************************************************
                    send infor to queue,
                    log event if something happens
    ***************************************************************************** */
    printf("\nsend infor to queue\n");
    //copy data into my pointers: temp_aver[]
    for(i=0;i<Sensor_Number;i++)
    {
         if(IDFlagCheck[i]==1)
         {
            temp_aver[i]=tempAver[i];
            printf("sensor_id %d average temperature %d\n", i, temp_aver[i]);
         }
    }

    if(SET_QUEUE_SIZE>QueueSize(q_create))
    {
        long int data_averaged=(long int)((sensor_data.id<<20)+((long int)(temp_aver[sensor_data.id])<<1));
        packet_ptr_t sensor_data_tempera=(packet_ptr_t)&data_averaged;

        int temperatureTemp=sensor_data_tempera->value/10;

        Enqueue(q_create,(DATATYPE)data_averaged);

        if(temperatureTemp<MIN_TEMP)
        {
            sprintf(logbuf,"Temperature of sensor %d is too low: %d °C;\n",sensor_data.id%30,temperatureTemp); // in the whole project, i only show the data of sensor id from 0 to 30
            printf("write low T to fifo:  %s\n",logbuf);
                write(fifo_fds, logbuf, strlen(logbuf)+1);
            perror("Write:"); //Very crude error check
                memset(logbuf, 0, sizeof(logbuf));
        }

        //temperature too high
        else if(temperatureTemp>MAX_TEMP)
        {
            sprintf(logbuf,"Temperature of sensor %d is too high: %d °C;\n",sensor_data.id%30,temperatureTemp);// in the whole project, i only show the data of sensor id from 0 to 30
            printf("write high T to fifo:  %s\n",logbuf);
                write(fifo_fds, logbuf, strlen(logbuf)+1);
            perror("Write:"); //Very crude error check
                memset(logbuf, 0, sizeof(logbuf));
        }

        //temperature is normal
        else
        {
            //do nothing:just print
            sprintf(logbuf,"Temperature of sensor %d is OKAY: %d °C;\n",sensor_data.id%30,temperatureTemp);// in the whole project, i only show the data of sensor id from 0 to 30
            printf("  write fifo:  %s\n",logbuf);
        }

    }
>>>>>>> 72ab7c9359283513d60aec90b8ff12ff7c918271

	/*****************************************************************************
			   		Log event for losing connection
	***************************************************************************** */ 
	printf("\n>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>");
    printf("\nClient with Ip address %s, socket port %d loss connection ;",get_ip_addr(client),get_port(client));		  	
    printf("\n>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>");
    
	sprintf(logbuf, "\nClient with Ip address %s, socket port %d loss connection;\n",get_ip_addr(client),get_port(client));
	printf("write loss ip_addr & port to fifo:  %s\n",logbuf);
    write(fifo_fds, logbuf, strlen(logbuf)+1);

	//close client   		
	printf("\nclose client ");
	tcp_close( &client ); 
	                     
	return NULL;
}


/*****************************************************************************
   	thread task for read queue and save data to database
***************************************************************************** */ 
void* ReadqueueToDataBase(void *arg)
{
    while(1)
    {
	char container[200];
	char id_buffer[32];
	char value_buffer[32];
	char ts_buffer[32];
	packet_ptr_t sensor_data_from_queue;	

	sleep(1);

	if(QueueSize(q_create)!=0)
	{
//attention: here for req 12
		sensor_data_from_queue=(packet_ptr_t)QueueTop(q_create);
		Dequeue(q_create);

	    printf("%ld\t%ld\t%ld\n", (long int)sensor_data_from_queue->id%30,((long int)sensor_data_from_queue->value<4096)?(long int)sensor_data_from_queue->value/10:(((long int)sensor_data_from_queue->value)>>1)/10*(-1),(long int)time(NULL));

		//save what is read out into database
		sprintf(id_buffer, "%ld", (long int)sensor_data_from_queue->id%30);

		if(sensor_data_from_queue->value<4096)
			sprintf(value_buffer, "%ld",(long int)sensor_data_from_queue->value/10);
		else
			sprintf(value_buffer, "%ld",(((long int)sensor_data_from_queue->value)>>1)/10*(-1));

//		sprintf(ts_buffer, "%ld", (long int)sensor_data_from_queue->ts);

		sprintf(ts_buffer, "%ld", (long int)time(NULL));

		strcpy(container, "INSERT  INTO xuemei (sensor_id, sensor_value, timestamp) VALUES(");
		strcat(container, id_buffer);
		strcat(container, ",");
		strcat(container, value_buffer);
		strcat(container, ",");
		strcat(container, ts_buffer);
		strcat(container, ")");

<<<<<<< HEAD
        printf("%s\n",container);

=======
>>>>>>> 72ab7c9359283513d60aec90b8ff12ff7c918271
		//excute the query
		mysql_query(conn, container);

		//reset the query container after each query
		memset(container, 0, 200);
	}

	else
	{
		printf("queue is empty\n");
		break;
	}

   }

<<<<<<< HEAD
   return NULL;
=======
    return NULL;
>>>>>>> 72ab7c9359283513d60aec90b8ff12ff7c918271

}

static void exit_clean() {
<<<<<<< HEAD
    printf("clean before exit ...");
    mysql_close(conn);
    //free memory
    pool_destroy ();
    QueueDestroy(&q_create);

    //delete fifo
=======
    mysql_close(conn);
    pool_destroy ();
    QueueDestroy(&q_create);

>>>>>>> 72ab7c9359283513d60aec90b8ff12ff7c918271
    close(fifo_fds);
    unlink(FIFO_NAME);

    free(temp_aver);
    sensor_free();
<<<<<<< HEAD

=======
>>>>>>> 72ab7c9359283513d60aec90b8ff12ff7c918271
}






