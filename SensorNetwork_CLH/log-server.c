#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>


#define LOG_FIFO "/tmp/logFifo"
#define MAX_BUF 1024

int main()
{
    int fd1;
	FILE *fp_log;
	char log_event[70],buf[BUFSIZ];
	int sequence=0;
	char* fifo_name=LOG_FIFO;
	struct stat st;

	time_t timeNow;
	
	// if no fifos, create 
  	if (stat(fifo_name, &st) != 0)
    mkfifo(fifo_name, 0666);
   
    fd1= open(fifo_name,O_RDONLY);

    memset(buf, 0, sizeof(buf));
	
	fp_log = fopen("./gateway.log", "a");
	if(fp_log== NULL) 
	{
		printf("I couldn't open log_server for writing.\n");
		exit(0);
	} 

	 while (1)
	 {
//attention: here for req 14
		if(read(fd1, buf, BUFSIZ)>0)
		{
	        sequence++;
			timeNow=time(NULL); 
			sprintf(log_event, "\n <%d>: at %ld :  %s\n",sequence,(long int)timeNow,buf);   //working

			printf("****** %s",log_event);
			fwrite(log_event,1,sizeof(log_event),fp_log);	

	        /* clean buf from any data */
	        memset(buf, 0, sizeof(buf));
			sleep(1);
		}	
	
   	}

	fclose(fp_log);
	close(fd1);
	unlink(fifo_name);

	return 0;

}
