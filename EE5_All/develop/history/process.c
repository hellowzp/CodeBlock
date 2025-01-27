#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>

int up_stream_pipe[2];
int down_stream_pipe[2];

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
	int bytes = read(up_stream_pipe[0],buf,20);
	printf("parent receive data: %s, %d bytes signal: %d\n",buf,bytes,signo);	
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
	pipe(up_stream_pipe);
	pipe(down_stream_pipe);

	//signal(SIGIO,SIG_IGN);
	
	pid_t pid = fork();
	if(pid<0) {
		printf("%s\n","fail to folk...");
	} else if(pid>0) {
		
		if(!CHILD_RESTART) {   //just set at first time if case of error like when closing already closed fds
			close(up_stream_pipe[1]);    //parent only read from up stream
			close(down_stream_pipe[0]);  //parent only write to down stream
			
			int status_flag = fcntl(up_stream_pipe[0],F_GETFL);
			fcntl(up_stream_pipe[0], F_SETFL, status_flag | O_NONBLOCK | O_ASYNC);	
			fcntl(up_stream_pipe[0], F_SETOWN, getpid());	
			
			signal(SIGIO,parent_read_callback);		
			signal(SIGCHLD,child_run);
			signal(SIGPIPE,child_run);
		}
		
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
		printf("%s\n","new child started...");
		atexit(child_cleanup);
		
		close(up_stream_pipe[0]);    
		close(down_stream_pipe[1]); 
		
		int status_flag = fcntl(down_stream_pipe[0],F_GETFL);
		fcntl(down_stream_pipe[0], F_SETFL, status_flag | O_NONBLOCK | O_ASYNC );
		fcntl(down_stream_pipe[0], F_SETOWN, getpid());
		
		signal(SIGIO,child_read_callback);
		signal(SIGSEGV,child_exit);    //block here..
		signal(SIGUSR1,child_exit);
		
		while(1);
	}
}

int main() {
	
	init();
	
	
	
	
	
	child_run(0);
	
	return 0;
}

