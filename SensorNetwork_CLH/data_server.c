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

#include "tcpsocket.h"
#include "gateway.h"
#include "linkedlist.h"
#include "statistic.h"
#include "thread.h"
#include "my_queue.h"


/*------------------------------------------------------------------------------
		macro definition
------------------------------------------------------------------------------*/
#define THREADNUM 10

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
#endif

#define LOG_FIFO "logFifo"

/*------------------------------------------------------------------------------
		variable declare
------------------------------------------------------------------------------*/

	packet_ptr_t sensor_packet_temp;
 
	int *temp_aver;
	int *tenDataFlag;
	
	MYSQL *conn;
	
	Queue *q_create;

	char* fifo_name=LOG_FIFO;
	int fd0;
	int PORT_SENSOR;
	char logbuf[100];
	int my_MAXSIZE;

/*------------------------------------------------------------------------------
		function declare
------------------------------------------------------------------------------*/
                             
void * thread_task_TCP_Con (void *arg);
void* ReadqueueToDataBase(void *arg);

/*------------------------------------------------------------------------------
		main
------------------------------------------------------------------------------*/

int main( int argc, char *argv[] )
{
    Socket server,client;   
    int fd_master,newSocketFlag;
    fd_set readfd;
  
    if(argc != 2)
    {
       printf("Incorrect number of args\n");
       exit(-1);
    }
    server_port = (int)strtol(args[1],NULL,10);  //server port
	if(server_port <= 0 || server_port > 65535){
       printf("invalid port\n");
       exit(-2);
    }
    
/*****************************************************************************
						variable initialization
***************************************************************************** */

        //tasks maxim number 
        sensor_malloc();
        temp_aver=(int*)malloc(Sensor_Number*sizeof(int));

        //pool initial
        pool=(CThread_pool *)malloc(sizeof(CThread_pool));
    
        my_MAXSIZE=SET_QUEUE_SIZE; 
	    
	    q_create=QueueCreate();

/*****************************************************************************
						SQL connection
***************************************************************************** */
	
       fd0= open(fifo_name,O_WRONLY);

	   conn = mysql_init(NULL); 
//for GT
//	mysql_real_connect(conn, "studev.groept.be", "a13_syssoft", "a13_syssoft", "a13_syssoft", 0, NULL, 0);
//for my own
	printf("sql connection.. ***\n");
	mysql_real_connect(conn, "localhost", "root", "root", "sensor", 0, NULL, 0);
	printf("sql connected.. ***\n"); 

/*****************************************************************************
					Create	Multi-Thread & connection manager
***************************************************************************** */    
    printf("init pool...\n");    

    //initial thread pool
    pool_init (THREADNUM);
 
    //create server and get their fd  
    server=tcp_passive_open(PORT_SENSOR);          
  
	while(1)
	{ 
	        printf("server waiting for connection.. ***\n");    
			client = tcp_wait_for_connection( server);
			printf("new connection.. ***\n"); 

		//add task to new worker
	        pool_add_worker (thread_task_TCP_Con, client);  /////////
	
		//write new connection infor to FIFO-->log file
		sprintf(logbuf, "Sensor node connection with Ip address %s, socket port %d  has  connected;\n",get_ip_addr(client),PORT_SENSOR);
		printf("  \nwrite ip address & port to fifo:  %s\n",logbuf);
	    write(fd0, logbuf, strlen(logbuf)+1);
	
		//sleep for a while to make sure the queue is not empty in the beginning
		sleep(5);
		//read data from the queue and save it into the database
		pool_add_worker (ReadqueueToDataBase, client);	 /////////	   
   }
		mysql_close(conn);
		//free memory         
	    pool_destroy ();
        QueueDestroy(&q_create);

		//delete fifo
		close(fd0);		
		unlink(fifo_name);
       
		free(temp_aver);
		sensor_free();
 
		return 0;
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

	client=arg;	 
	bytes=1;    //tcp receiver > 0 => do

    //skip the compression for quickly testing
	//int tenDataFlag=0;
	
     do
     {	
	    bytes = tcp_receive( client, buffer0, BUFSIZE ); //statistic
	        
	   /**********************************************************
					sensor data communication
	   ************************************************************/ 
			
	    sensorData=atoi(buffer0);
	    data_buffer=sensorData;    //statistic :get sensor id
	   
	    printf("\nreceived data is: %ld",sensorData );
	    
	    sensor_packet_temp=(packet_ptr_t)&sensorData;
//attention: here for req 9	    
	    sensor_data.timestamp=time(NULL);   //statistic    gateway
	    sensor_data.value=sensor_packet_temp->value;    
	    sensor_data.id=sensor_packet_temp->id;
	    
	    printf("\n>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>");
	    printf("\n>>>>>>>>>>>>>>>>>>>>>>>>>>ddddddddddd %d >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n",sensor_data.id);
	    	printf("\n>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>");
	    
	    sensor_packet_temp->parity=parityCheck(sensor_packet_temp);
	    
	    //echo msg back to client
	    tcp_send( client, (void*)buffer0, bytes );

	    ID_buffer=sensor_data.id;	 

	/*****************************************************************************
			   		Data Compression
	***************************************************************************** */ 
	    data_manipulation(ID_buffer);            //compression
	    

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
	   	  //tenDataFlag++;
	   	  
	   //if(tenDataFlag==10)          /*skip the checking(if we already have 10 data for this sensor then enqueue) just for quickly seeing the data*/
	   {
		//tenDataFlag=0;
		if(my_MAXSIZE>QueueSize(q_create))
		{
			long int data_averaged=(long int)((sensor_data.id<<20)+((long int)(temp_aver[sensor_data.id])<<1));
			packet_ptr_t sensor_data_tempera=(packet_ptr_t)&data_averaged;	
			
			int temperatureTemp=sensor_data_tempera->value/10;

//attention: here for req 7
			//euqueue here			
			Enqueue(q_create,(DATATYPE)data_averaged);
			
//attention: here for req 11, 15			
			//temperature too low
			if(temperatureTemp<SET_MIN_TEMP)
			{
				sprintf(logbuf,"Temperature of sensor %d is too low: %d °C;\n",sensor_data.id%30,temperatureTemp); // in the whole project, i only show the data of sensor id from 0 to 30
				printf("write low T to fifo:  %s\n",logbuf);
			        write(fd0, logbuf, strlen(logbuf)+1);
				perror("Write:"); //Very crude error check
   			        memset(logbuf, 0, sizeof(logbuf));
			}

			//temperature too high
			else if(temperatureTemp>SET_MAX_TEMP)
			{
				sprintf(logbuf,"Temperature of sensor %d is too high: %d °C;\n",sensor_data.id%30,temperatureTemp);// in the whole project, i only show the data of sensor id from 0 to 30
				printf("write high T to fifo:  %s\n",logbuf);
			        write(fd0, logbuf, strlen(logbuf)+1);
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

	   }
    }while(bytes>0);
    


	/*****************************************************************************
			   		Log event for losing connection
	***************************************************************************** */ 
	printf("\n>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>");
    printf("\nClient with Ip address %s, socket port %d loss connection ;",get_ip_addr(client),get_port(client));		  	
    printf("\n>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>");
    
	sprintf(logbuf, "\nClient with Ip address %s, socket port %d loss connection;\n",get_ip_addr(client),get_port(client));
	printf("write loss ip_addr & port to fifo:  %s\n",logbuf);
	write(fd0, logbuf, strlen(logbuf)+1);

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

   return NULL;

}
