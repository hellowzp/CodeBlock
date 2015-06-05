/****************************************
*      config generate
****************************************/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <string.h>
#include "DataServer.h"
#include "saveindb.h"
#include "tcpsocket.h"
#include "D_list.h"
#include <pthread.h>

time_t base_T ;	
list_ptr_t  *sensor_array;
#define LOG_FILE "./logfile.txt"
#define PARA_FILE "./parameter.txt"

void* myprocess (void* arg) 
{
	int   i=0,j;
    FILE *fp;
    FILE *fp_p;
    int ones_check = 0;
	sensor_data_t  temp_packet;
	sensor_data_t  package;
	sensor_data_ptr_t  d;
	int number=0;
	packet_ptr_t temp_packet_ptr;
	int parityNum=0;               
	data_t data;
	time_t t_1;
	char pac[1024];
	char buffer[1024];
	int bytes=0;
	Socket client;
	pthread_mutex_t log_m;
	pthread_mutex_t db_m;
	pthread_mutex_t para_m;
    time(&base_T);//keep to track time
    init_array(&sensor_array); //initialization
    
    client = ((argument_ptr)arg)->client;
    log_m = ((argument_ptr)arg)->log_m;
    db_m = ((argument_ptr)arg)->db_m;
    para_m = ((argument_ptr)arg)->para_m;
    d= (sensor_data_ptr_t)malloc(sizeof(sensor_data_t)); 
    temp_packet_ptr = (packet_ptr_t)malloc(sizeof(sensor_packet));


	
	while(1)
    {	
		bytes = tcp_receive( client, buffer, BUFSIZE ); 
		printf("received message of %d bytes: %s\n", bytes, buffer );


		
		
		strcpy(pac,buffer);
		number = atoi(pac);
		printf ("the gateway is %d\n", number);
        
		temp_packet_ptr = (packet_ptr_t)(&number);  //number's value to temp_packet_ptr

		printf ("the sensor parameter is %d\n", *temp_packet_ptr);


 
        
        //deepcopy
        *temp_packet_ptr=deep_copy(temp_packet_ptr);
        
		time(&t_1);
		package.ts = t_1;				
		package.id = temp_packet_ptr->id;	
		package.value = temp_packet_ptr->value;
		
		printf("time is %d\n",(int)package.ts);
		// to count the number of '1' in the data packet
		for (ones_check = 0; *(int *)temp_packet_ptr; (*(int *)temp_packet_ptr) >>= 1)
	    ones_check += (*(int *)temp_packet_ptr & 1);
		parityNum=ones_check;
		
		if (parityNum % 2) 
		{	
			pthread_mutex_lock(&log_m);
			printf("1\n");
			fp = openFile(LOG_FILE,"a");
			printf("2\n");
			logkeep = LOG_CHECK_NO;
			log_handler(fp,logkeep,client);
			printf("3\n");
			closeFile(fp);
			pthread_mutex_unlock(&log_m);
			tcp_send( client, (void*)buffer, bytes );
			continue;
		}
		
	    ////if check pass then save the value to database and list	
		pthread_mutex_lock(&log_m);
		printf("1\n");
		fp = openFile(LOG_FILE,"a");
		printf("2\n");
		logkeep = LOG_CHECK_OK;
		log_handler(fp,logkeep,client);
		printf("3\n");
		closeFile(fp);
		pthread_mutex_unlock(&log_m);
		
		//statistic server
		save_in_list((sensor_data_ptr_t)&package,para_m,fp_p);


		d=(sensor_data_ptr_t)(&package);
        printf("recieve value is %d,%d\n",package.id,package.value);
        //save data into database
        
        pthread_mutex_lock(&db_m);
        
		save_fun(d);
		
		pthread_mutex_unlock(&db_m);
		// echo msg back to client
		tcp_send( client, (void*)buffer, bytes );
		memset(buffer,0,BUFSIZE);
	}
    
    fclose(fp);
}


//----------------------------------------------------------------------------//
sensor_packet deep_copy(packet_ptr_t data)
//----------------------------------------------------------------------------//
{
	sensor_packet t;
	t = (sensor_packet)(*data);
	return (sensor_packet)t; 
}

//----------------------------------------------------------------------------//
//  Statistic Server Part
//----------------------------------------------------------------------------//

void  save_in_list (sensor_data_ptr_t temp_packet_ptr,pthread_mutex_t para_m,FILE* fp) 
{ 
	             
	sensor_data_t data;
	time_t variable_T;
	time(&variable_T);	
	int i=0;
	double sum=0,t = 0,t_1=0,t_2=0,average = 0,av_time = 0;	
	
	printf("temp time is %d\n",(int)temp_packet_ptr->ts);
	printf("list value is %d\n",(int)temp_packet_ptr->value);
	data.ts = temp_packet_ptr->ts;				
	data.id = temp_packet_ptr->id;	
	data.value = temp_packet_ptr->value;

		
	
	data =copydata(data);
	printf("DATA value is %d\n",(int)data.value);
	//if the linked list for this sensor does not exsit, then creat one
	if (sensor_array[(int)temp_packet_ptr->id] == NULL) 
		list_alloc(&data_destory,&data_compare,&data_copy, (data_ptr_t)&data);//(function's return value)

	else    //add the data at beginning of list,position->0
		list_insert_at_index(sensor_array[(int)temp_packet_ptr->id], (data_ptr_t)&data, 0);
		error_handler(err);	

	time(&variable_T);
		
	if ((variable_T - base_T) < day_time)
		{
			for(i=0;i<list_size(sensor_array[(int)temp_packet_ptr->id]);i++)
			{
				printf("caculate\n");
				sum += (int)(((sensor_data_t)*((sensor_data_ptr_t)list_get_data_at_index(sensor_array[(int)temp_packet_ptr->id],i))).value); 
			}
			average = sum/(i+1);
			av_time = t/(i+1);
			//printf("sensor average value is %d \n",(int)sum);
			for(i=0;i<list_size(sensor_array[(int)temp_packet_ptr->id]);i++)
			{	
				if(i=0){
					
					t_1=(double)((sensor_data_t)*((sensor_data_ptr_t)list_get_data_at_index(sensor_array[(int)temp_packet_ptr->id],i))).ts;
					t_2=0.0;
				}
				if(i>0){
				
					t_1=(double)((sensor_data_t)*((sensor_data_ptr_t)list_get_data_at_index(sensor_array[(int)temp_packet_ptr->id],i))).ts;
					t_2=(double)((sensor_data_t)*((sensor_data_ptr_t)list_get_data_at_index(sensor_array[(int)temp_packet_ptr->id],i-1))).ts;
				}
				t=(t_2-t_1)/(i+1);
			}
			av_time = t;
			//printf("sensor average recieve time is %d seconds \n ",(int)t_1);
			pthread_mutex_lock(&para_m);
			fp = openFile(PARA_FILE,"a");
			parameter_save(fp,data.value,(data.ts/10000000),data.id);
			closeFile(fp);
			pthread_mutex_unlock(&para_m);

		}
	else
		{
			time(&base_T);
			time(&variable_T);
			variable_T = (time_t)((int)variable_T-day_time);      //refresh timestamp
			for(i=0;i<=list_size(sensor_array[(int)temp_packet_ptr->id]);i++)
			{
				sum += (int)((sensor_data_t)*((sensor_data_ptr_t)list_get_data_at_index(sensor_array[(int)temp_packet_ptr->id],i))).value; 
			}
			average = sum/(i+1);
			av_time = t/(i+1);
			//printf("sensor average value is %d \n",(int)average);
			for(i=0;i<list_size(sensor_array[(int)temp_packet_ptr->id]);i++)
			{	
				if(i=0){
					
					t_1=(double)((sensor_data_t)*((sensor_data_ptr_t)list_get_data_at_index(sensor_array[(int)temp_packet_ptr->id],i))).ts;
					t_2=0.0;
				}
				if(i>0){
				
					t_1=(double)((sensor_data_t)*((sensor_data_ptr_t)list_get_data_at_index(sensor_array[(int)temp_packet_ptr->id],i))).ts;
					t_2=(double)((sensor_data_t)*((sensor_data_ptr_t)list_get_data_at_index(sensor_array[(int)temp_packet_ptr->id],i-1))).ts;
				}
				t=(t_2-t_1)/(i+1);
			}
			av_time = t;
			//printf("sensor average recieve time is %d seconds \n ",(int)av_time);
			pthread_mutex_lock(&para_m);
			fp = openFile(PARA_FILE,"a");
			parameter_save(fp,data.value,(data.ts/10000000),data.id);
			closeFile(fp);
			pthread_mutex_unlock(&para_m);
			//remove the oldest sensor record
			free_sensor_list(&sensor_array[(int)temp_packet_ptr->id],0);
		}
	
    
} 

void init_array(list_ptr_t **ptr)
{
	*ptr = (list_ptr_t *)malloc(number_of_sensors * sizeof(list_ptr_t));
	if (*ptr == NULL)
	{
		printf("Wrong memory");
		assert(1==0);
	}

}

list_ptr_t free_sensor_list(list_ptr_t *ptr,int index)
{
	return list_free_at_index( *ptr,index);
}
void destory_array(list_ptr_t **ptr)

{
	int i;	
	// free each parameter list of sensor array
	for (i = 0; i < number_of_sensors; i++)
		list_free_all((*ptr)[i]);
	// free the sensor array 
	free(*ptr);
}

sensor_data_t copydata(sensor_data_t data)
{
	sensor_data_t t;
	t=data;
	return t;
}

data_ptr_t data_copy(data_ptr_t data)
{
	data_t *t = (data_t *)malloc(sizeof(data_t));
	*t = *(data_t *)data;
	return (data_ptr_t)t; 
}

int data_compare(data_ptr_t d1, data_ptr_t d2)
{
	if (((data_t *)d1)->signbit != ((data_t *)2)->signbit)
	{
		if (((data_t *)d1)->value != ((data_t *)d2)->value)
		{
			if (((data_t *)d1)->signbit)
				return 1;
			else
				return -1;
		}
		else
			return 0;
	}
	else
	{
		if (((data_t *)d1)->signbit)
			return (int)(((data_t *)d2)->value - ((data_t *)d1)->value);
		else
			return (int)(((data_t *)d1)->value - ((data_t *)d2)->value);
	}
}


void data_destory(data_ptr_t data)
{
	free((data_t *)data);
}


void error_handler(err_code err)
{
	switch (err)
	{
		case NONE: 
			break;
		case NULL_POINTER_ERR:
			printf("error:no pointer \n");
			break;
		case NONE_EXIST_REF_ERR:
			printf("no such reference \n");
			break;
		case NON_NEXT_REF_ERR:
			printf(" no next reference \n");
			break;
		case NON_PREVIOUS_REF_ERR:
			printf(" no previous reference\n");
			break;
		case NO_DATA_ERR:
			printf(" no data  \n");
			break;
		default:
			printf(" unknow error \n");
			
		
	}
}


//-------------------------------------------------------------------------//
//  LogServer Part
//-------------------------------------------------------------------------//
int getTime(char out[512], int fmt)               
{ 
    if(out == NULL) 
        return -1; 
    
    time_t t; 
    struct tm* timeinfo;
    t = time(NULL); 
	
    timeinfo= localtime(&t); 
    strcpy(out,(asctime(timeinfo)));
    printf("gettime %s\n",out);
    return 0; 
} 
  
FILE* openFile(const char *fileName, const char *mode)  // 
{ 
    FILE *fp = fopen(fileName, mode); 
    return fp; 
} 
  

int writeFile(FILE *fp, char *str)          
{ 
    assert(fp != NULL && str != NULL); 
    char curTime[100] = {0}; 
    int ret = -1; 
    
  
    getTime(curTime, 0); 
    ret = fprintf(fp, "[%s] %s\n", curTime,str); 
    fflush(fp);
  
    if(ret >= 0) 
    { 
        fflush(fp); 
        return 0;               
    } 
    else
        return -1; 
} 
  
int closeFile(FILE *fp) 
{ 
    return fclose(fp); 
} 

void parameter_save(FILE *fp,int value,int t,int id)
{
	assert(fp != NULL );
	char curTime[100] = {0};
	char str[1024];
	getTime(curTime, 0);
	int ret=-1;
	
	sprintf(str,"The sensor id is %d ,average value is %d,average time is %d\n",id,value,t);
	ret = fprintf(fp, "%s -----> %s\n", curTime,str); 
	fflush(fp);
	
}
char* log_handler(FILE *fp,log_info logkeep,Socket client)
{	
	assert(fp != NULL );
	char str[1024];
	char curTime[100] = {0};
	getTime(curTime, 0); 
	int ret=-1;
	switch (logkeep)
	{
		case LOGIN_IP_PORT: 
			sprintf(str,"Client login , ip is : %s , port : %d \n" , get_ip_addr(client) , get_port( client));
			ret = fprintf(fp, "%s -----> %s\n", curTime,str); 
			fflush(fp);	
			break;
		case LOGOFF_IP_PORT:
			sprintf(str,"Client logoff , ip is : %s , port : %d \n" , get_ip_addr(client) , get_port( client));
			ret = fprintf(fp, "%s -----> %s\n", curTime,str); 
			fflush(fp);		
			break;
		case LOG_CLIENT_APP:
			sprintf(str,"Client_app login , ip is : %s , port : %d \n" , get_ip_addr(client) , get_port( client));
			ret = fprintf(fp, "%s -----> %s\n", curTime,str); 
			fflush(fp);	
			break;
		case LOG_GATEOPEN:
			sprintf(str," Gateway is opening\n");
			ret = fprintf(fp, "%s -----> %s\n", curTime,str); 
			fflush(fp);	
			break;
		case LOG_CHECK_NO:
			sprintf(str," Parity check has failed!");
			ret = fprintf(fp, "%s -----> %s\n", curTime,str); 
			fflush(fp);	
			break;
		case LOG_CHECK_OK:
			sprintf(str," Parity check has passed!");
			ret = fprintf(fp, "%s -----> %s\n", curTime,str); 
			fflush(fp);		
			break;
		default:
			sprintf(str," unknow error \n");
			ret = fprintf(fp, "%s -----> %s\n", curTime,str); 
			fflush(fp);		
			break;
		
	}
				
}

char* log_off(FILE* fp,char* ip,int port)
{	
	assert(fp != NULL );
	char str[1024];
	char curTime[100] = {0};
	getTime(curTime, 0); 
	sprintf(str,"Client logoff , ip is : %s , port : %d \n" , ip , port);
	fprintf(fp, "%s -----> %s\n", curTime,str); 
	fflush(fp);
}
