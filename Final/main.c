#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sqlite3.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/wait.h>

#include "config.h"
#include "sbuffer.h"
#include "connmgr.h"
#include "datamgr.h"
#include "sensor_db.h"
#include "errmacros.h"
#include "lib/dplist.h"

#define FIFO_NAME 	"logFifo"

void *connmgr(void *arg);
void *datamgr(void *arg);
void *storagemgr(void *arg);



sbuffer_t *buffer;
FILE *fp_FIFO_w;
int main(int argc, const char *argv[])
{
	pid_t child_pd, pid;
	FILE* fp_FIFO_r;
	pthread_t connmgr_thread,
			  datamgr_thread,
			  storagemgr_thread;

	if (argc < 2)
	{
		puts("not enough command line argument, set port number");
		exit(EXIT_FAILURE);
	}

	int port_number = atoi(argv[1]);
	printf("port number = %d\n", port_number);

	/* Create the FIFO if it does not exist */ 
	CHECK_MKFIFO(mkfifo(FIFO_NAME, 0666));

	child_pd = fork();
	SYSCALL_ERROR(child_pd);

	if (child_pd == 0) //child process
	{
		FILE * fp_gateway;
		char str[256];
		char *retval = NULL;
		int sequence_number = 0;

		fp_FIFO_r = fopen(FIFO_NAME, "r");
		FILE_OPEN_ERROR(fp_FIFO_r);

  		fp_gateway = fopen("gateway.log", "w");
		FILE_OPEN_ERROR(fp_gateway);

		while(1)
		{
			retval = fgets(str,256,fp_FIFO_r);
			if(retval == NULL) break;
			sequence_number++;
#ifdef DEBUG
			fprintf(stderr, "DEBUG log_mssge_recv, %2d %ld %s", sequence_number, time(NULL), str);
#endif
			fprintf(fp_gateway, "%2d %ld %s", sequence_number, time(NULL), str);
		}

		FILE_CLOSE_ERROR(fclose(fp_gateway));
		FILE_CLOSE_ERROR(fclose(fp_FIFO_r));
		exit(EXIT_SUCCESS);
	}

	fp_FIFO_w = fopen(FIFO_NAME, "w");
	FILE_OPEN_ERROR(fp_FIFO_w);
	
	if (sbuffer_init(&buffer) != SBUFFER_SUCCESS) exit(EXIT_FAILURE);

	int result;	
	result = pthread_create(&connmgr_thread, NULL, connmgr, &port_number);
	PTHREAD_ERROR(result);
	result = pthread_create(&datamgr_thread, NULL, datamgr, NULL);
	PTHREAD_ERROR(result);
	result = pthread_create(&storagemgr_thread, NULL, storagemgr, NULL);
	PTHREAD_ERROR(result);

	result = pthread_join(connmgr_thread, NULL);
	PTHREAD_ERROR(result);
	result = pthread_join(datamgr_thread, NULL);
	PTHREAD_ERROR(result);
	result = pthread_join(storagemgr_thread, NULL);
	PTHREAD_ERROR(result);

	
	FILE_CLOSE_ERROR(fclose(fp_FIFO_w));
	if (sbuffer_free(&buffer) != SBUFFER_SUCCESS) exit(EXIT_FAILURE);

	pid = wait(NULL);
	SYSCALL_ERROR(pid);

	return 0;
}

void *connmgr(void * arg)
{
	int port_number = *((int *)arg);
	
	connmgr_listen(port_number, &buffer);
	connmgr_free();
	pthread_exit(NULL);
}

void *datamgr(void *arg)
{
	FILE *fp_text;
	fp_text = fopen("room_sensor.map", "r");
	FILE_ERROR(fp_text, "Couldn't open room_sensor.map\n");
	
	datamgr_parse_sensor_data(fp_text, &buffer);
	datamgr_free();
	FILE_CLOSE_ERROR(fclose(fp_text));
	pthread_exit(NULL);
}

void *storagemgr(void *arg)
{
	sqlite3 *db;
	db = init_connection(1);
	
	storagemgr_parse_sensor_data(db, &buffer);
	disconnect(db);
	pthread_exit(NULL);
}
