#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>

#define FIFO_NAME "sensor_fifo"

void child_cleanup(int signo) {
	printf("signal %d %s\n",signo,"child begin to exit...");
	exit(EXIT_FAILURE);
}

int main() {
	//int ret = mkfifo(FIFO_NAME,S_IRWXU);
	//if(ret==-1) perror("mkfifo");
	
	int pfds = open(FIFO_NAME, O_RDONLY | O_NONBLOCK);
	if(pfds<0) {
        perror("parent open fifo for reading error");
        exit(EXIT_FAILURE);
	}
	
	pid_t pid = fork();
	if(pid>0) {
		signal(SIGCHLD,child_cleanup);
		
		char buf[100];
		while(1) {
			int bytes = read(pfds,buf,100);
			if(bytes>0) printf("%s\n",buf);
			sleep(5);
		}	
	} else if (pid==0) {
		int cfds = open(FIFO_NAME, O_WRONLY | O_NONBLOCK);
		if(cfds<0) {
        perror("child open fifo for writing error");
        exit(EXIT_FAILURE);
	}
		char c = 0;
		while(1) {
			read(STDIN_FILENO,&c,1);
			if(c=='s') write(cfds,"writing",7);
			if(c=='q') exit(0);
		}
			
	} else {
		printf("%s\n","fork failed...");
		exit(EXIT_FAILURE);
	}
	
	wait(NULL);
	
	return 0;
}
