#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/sem.h>
#include <signal.h>


static int sem_id = 0;
static int set_semvalue();
static void del_semvalue();
static int semaphore_p();
static int semaphore_v();

void child_cleanup() {
	printf("\n%s\n","child clean up before exit..");
}

void parent_cleanup() {
	printf("\n%s\n","parent clean up before exit..");
	del_semvalue();
	exit(0);
}

union semun
{
	int val;
	struct semid_ds *buf;
	unsigned short *arry;
};

static int set_semvalue()
{
	union semun sem_union;

	sem_union.val = 1;
	if(semctl(sem_id, 0, SETVAL, sem_union) == -1)
		return 0;
	return 1;
}

static void del_semvalue()
{
	union semun sem_union;

	if(semctl(sem_id, 0, IPC_RMID, sem_union) == -1) 
		fprintf(stderr, "Failed to delete semaphore\n");
}

static int semaphore_p()
{
	struct sembuf sem_b;
	sem_b.sem_num = 0;
	sem_b.sem_op = -1;//P()
	sem_b.sem_flg = SEM_UNDO;
	if(semop(sem_id, &sem_b, 1) == -1)
	{
		fprintf(stderr, "semaphore_p failed\n");
		return 0;
	}
	return 1;
}

static int semaphore_v()
{
	struct sembuf sem_b;
	sem_b.sem_num = 0;
	sem_b.sem_op = 1;//V()
	sem_b.sem_flg = SEM_UNDO;
	if(semop(sem_id, &sem_b, 1) == -1)
	{
		fprintf(stderr, "semaphore_v failed\n");
		return 0;
	}
	return 1;
}


int main() {
	
	atexit(parent_cleanup);
	
	sem_id = semget((key_t)1234, 1, 0666 | IPC_CREAT);
	
	if(!set_semvalue())
	{
		fprintf(stderr, "Failed to initialize semaphore\n");
		exit(EXIT_FAILURE);
	}
	
	pid_t pid = fork();
	if(pid<0) {
		printf("%s\n","fail to folk...");
	} else if(pid>0) {
		
		char c = '0';
		while(1) {
			read(STDIN_FILENO,&c,1);
			if(c=='b') {             //kill the child
				kill(pid,SIGUSR1);
				exit(0);
			}
			if(c=='s') {   //the condition to allow child post
				printf("%s\n","parent increment semaphore...");
				semaphore_v();
			}
			if(c=='k') {
				kill(pid,SIGUSR1);   //this will cause parent_cleanup!! why?
			}
			
		}

	sleep(10);
	printf("\n%d - finished\n", getpid());
	
	wait(NULL);

	exit(EXIT_SUCCESS);
			
	} else {
		
		printf("%s %d %d\n","new child started...",getpid(),getppid());
		atexit(child_cleanup);
		
		signal(SIGUSR1,exit);
				
		printf("%s\n","child process running...");
	
		while(1) {
			semaphore_p();
			printf("%s\n","child decrement semaphore...");
			//do whatever here...
		}
		
		exit(EXIT_FAILURE);			
	}
}
