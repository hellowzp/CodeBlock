#define _GNU_SOURCE  // NON-STANDARD fcloseall
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/prctl.h>
#include <fcntl.h>
#include <errno.h>
#include <pthread.h>
#include <signal.h>
#include <sys/sem.h>
#include <sys/wait.h>
#include <time.h>
#include <inttypes.h>

#include "config.h"
#include "list.h"
#include "queue.h"
#include "tcpsocket.h"
#include "sensor_db.h"
#include "util.h"

#define FIFO_NAME "temp/logfifo"
#define LOG_FILE "temp/gateway.log"

#ifdef DEBUG
    #define ARGNUM 3
#else
    #define ARGNUM 2
#endif

const unsigned int QUEUE_ELEMENT_SIZE = sizeof(sensor_data_t);

int port;
int fifo_write_fds;
float min_tmp;
float max_tmp;
queue_ptr_t sensor_queue;
list_ptr_t sensor_list;
pthread_mutex_t list_mutex;
pthread_mutex_t fifo_mutex;

void list_element_copy(element_ptr_t* des, element_ptr_t src) {
    assert(des && src);
    MALLOC(*des, sizeof(list_element_t));
    memcpy(*des, src, sizeof(list_element_t));

}

void list_element_free(element_ptr_t* element){
    DEBUG_PRINTF("free element @%p\n", element);
}

int list_element_compare_by_id(element_ptr_t x, element_ptr_t y) {
    sensor_id_t x_id = ((list_element_ptr_t) x )->sensor_id;
    sensor_id_t y_id = ((list_element_ptr_t) y )->sensor_id;
    return x_id > y_id ?  1 :
           x_id < y_id ? -1 : 0;
}

void list_element_print(element_ptr_t element) {
    if(element) {
        list_element_ptr_t e = (list_element_ptr_t)element;
        printf("sensor_id(%d) room_id(%d) temperature(%3.1f) @ %s",
               e->sensor_id, e->room_id, e->avg_tmp, asctime(localtime(&e->ts)));
    }
}

void queue_element_copy(element_ptr_t* des, element_ptr_t src) {
    assert(des && *des && src);
    memcpy(*des, src, QUEUE_ELEMENT_SIZE);
}

void queue_element_free(element_ptr_t* element){
    DEBUG_PRINTF("free element @%p\n", *element);
}

void queue_element_print(element_ptr_t element){
    sensor_data_ptr_t e = (sensor_data_ptr_t)element;
    printf("id:%d temperature:%d @ %s",
        e->id, e->tmp, asctime(localtime(&e->ts)));
}

static void sig_kill_child(int sig);

static void clean_before_exit(void);

static int sem_id = 0;
union semun {
    int val;
    struct semid_ds *buf;
    unsigned short *arry;
};

static int set_semvalue();
static void del_semvalue();
static int semaphore_p();
static int semaphore_v();

static void* tcp_connect(void* arg);
static void client_cleanup_if_stopped(void* arg);
static void* client_response(void* client);
static void* data_manage(void* arg);
static void* storage_manage(void* arg);
static void child_cleanup(int signo);
static void send_log_msg(int fds, const char* info);

int main(int argv, char* args[]) {
    if(argv!=ARGNUM) {
		printf("usage: %s %s\n",args[0],"port");
		exit(EXIT_FAILURE);
	} 

	#ifdef SET_MIN_TEMP
    min_tmp = (float)SET_MIN_TEMP;
	
	#ifdef SET_MAX_TEMP
    max_tmp = (float)SET_MAX_TEMP;
	#else 
	#error "SET_MIN_TEMP and SET_MAX_TEMP should be given as preprocessor directives."
	#endif
	
	#else 
	#error "SET_MIN_TEMP and SET_MAX_TEMP should be given as preprocessor directives."
	#endif
	
	port = (int)strtol(args[1],NULL,10);  //server port
	if(port <= 0 || port > 65535){
       printf("invalid port\n");
       exit(-2);
    }
    
    atexit( clean_before_exit );
    sensor_queue = queue_create();
    sensor_list = list_create( &list_element_copy, &list_element_free,
                               &list_element_compare_by_id, &list_element_print);
	
	list_mutex = (pthread_mutex_t)PTHREAD_MUTEX_INITIALIZER;
	fifo_mutex = (pthread_mutex_t)PTHREAD_MUTEX_INITIALIZER;
	
	sem_id = semget((key_t)1234, 1, 0666 | IPC_CREAT);	
	if(!set_semvalue()) {
		fprintf(stderr, "Failed to initialize semaphore\n");
		exit(EXIT_FAILURE);
	}

	if( access(FIFO_NAME,F_OK) != 0) {   //SEE MAN 2 ACCESS
        printf("access fifo test: creating new fifo\n");
		int ret = mkfifo(FIFO_NAME,S_IRWXU);
		if(ret==-1){
			perror("mkfifo");
			exit(EXIT_FAILURE);
		}
	}
	
    pid_t pid = fork();
    if(pid>0) {
        sleep(2);   //let the child open fifo for read first
        int pfds = open(FIFO_NAME, O_WRONLY);
        while(pfds<0) {
            perror("parent open fifo for writing error");
            if(errno==ENXIO) {
                sleep(2);
                pfds = open(FIFO_NAME, O_WRONLY);
            } else {
                exit(EXIT_FAILURE);
            }
        }
        fifo_write_fds = pfds;
		
        pthread_t pth_tcp_con, pth_data_man, pth_strg_man;
        int err;
        void* retStatus;
		
        if((err=pthread_create(&pth_tcp_con,NULL,tcp_connect,NULL))!=0) {
            printf("can't create tcp connection thread: %s\n",strerror(err));
            exit(EXIT_FAILURE);
        }
		
        if((err=pthread_create(&pth_data_man,NULL,data_manage,NULL))!=0) {
            printf("can't create data management thread: %s\n",strerror(err));
            exit(EXIT_FAILURE);
        }
		
        if((err=pthread_create(&pth_strg_man,NULL,storage_manage,NULL))!=0) {
            printf("can't create staorage management thread: %s\n",strerror(err));
            exit(EXIT_FAILURE);
        }
		
        prctl(PR_SET_PDEATHSIG, SIGHUP);
        waitpid(pid, NULL, 0);
		
        pthread_join(pth_tcp_con,&retStatus);
        printf("tcp connection thread terminated with status: %s\n",(char*)retStatus);
        pthread_join(pth_data_man,&retStatus);
        printf("data management thread terminated with status: %s\n",(char*)retStatus);
        pthread_join(pth_strg_man,&retStatus);
        printf("staorage management thread terminated with status: %s\n",(char*)retStatus);
		
    } else if (pid==0) {
        signal(SIGHUP, sig_kill_child); // ask kernel to deliver sighup signal when parent exits

        int fifo_read_fds = open(FIFO_NAME, O_RDONLY);
        if (fifo_read_fds<0) {
            perror("child open fifo for reading error");
            exit(EXIT_FAILURE);
        }
		
        int log_fds = open(LOG_FILE, O_CREAT | O_WRONLY | O_APPEND, S_IRWXU | S_IRWXG);
        if(log_fds<0) {
            perror("child open log file error");
            exit(EXIT_FAILURE);
        }
		
        char buf[100];
        memset(buf,0,100);
        while(1) {
            int bytes = read(fifo_read_fds,buf,100);
            if(bytes==-1) perror("reading fifo");
            else if(bytes>0) write(log_fds,buf,bytes);
//			sleep(60);
        }
			
    } else {
        perror("fork");
        exit(EXIT_FAILURE);
    }
	
	return 0;
}


static void* tcp_connect(void* arg) {
    DEBUG_PRINTF("%s\n","tcp connection thread created.");
    Socket server = tcp_passive_open(port);
    while (1) {
        DEBUG_PRINTF("%s\n","server waiting for new client...");
        pthread_t resp;
        Socket client = tcp_wait_for_connection(server);
		if(client) {
            pthread_create(&resp,NULL,client_response,client);  //new thread need the client as the parameter
			//void* cltStatus; //returned client status if client connection is closed
			//pthread_join(resp,&cltStatus);	//will block here..
			//printf("%s\n",(char*)cltStatus);
		}
    }

    //sleep(1);	/* to allow socket to drain */
    tcp_close( &server );

    pthread_exit((void*)"tcp connection thread return successfully...");
}

static void* client_response(void* client) {
    DEBUG_PRINTF("%s\n","new client thread created.\n");
    send_log_msg(fifo_write_fds, "new sensor node connected");
	
    uint16_t sensor_id;
    double temperature;
    time_t timestamp;
    int bytes;
    sensor_data_ptr_t sensor_data;
    MALLOC(sensor_data, sizeof(sensor_data_t));
    do {
        bytes = tcp_receive( client, (void *)&sensor_id, sizeof(sensor_id));
        // bytes == 0 indicates tcp connection teardown
        assert( (bytes == sizeof(sensor_id)) || (bytes == 0) );
        bytes = tcp_receive( client, (void *)&temperature, sizeof(temperature));
        assert( (bytes == sizeof(temperature)) || (bytes == 0) );
        bytes = tcp_receive( client, (void *)&timestamp, sizeof(timestamp));
        assert( (bytes == sizeof(timestamp)) || (bytes == 0) );
        if (bytes) {
          printf("server received new data:\nsensor id = %" PRIu16 " - temperature = %g - timestamp = %ld\n",
                  sensor_id, temperature, (long int)timestamp );
        }
        if(temperature<=min_tmp || temperature>=max_tmp) {
            char* msg;
            MALLOC(msg,80);
            snprintf(msg, 80, "Warning: abnormal temperature %4.2f @ Room %d Sensor %d",
                     temperature, sensor_id, sensor_id % 10);
            send_log_msg(fifo_write_fds, msg);
            free(msg);
        }

        sensor_data->id = sensor_id;
        sensor_data->tmp = (sensor_value_t)(temperature * 10);
        sensor_data->ts = timestamp;
        sensor_data->statu = 0;
        queue_enqueue(sensor_queue, sensor_data);
        queue_print(sensor_queue);
        DEBUG_PRINT("semaphore_v\n");
        semaphore_v(); semaphore_v(); // two threads waiting

      } while (bytes);
	
	tcp_close( &client );	
    send_log_msg(fifo_write_fds,"sensor node disconnected");
    DEBUG_PRINTF("client thread with sensor_id %d stopped...\n",sensor_id);
	pthread_exit((void*)("client closed...")); //better to describe client exit status
}


static void* data_manage(void* arg) {
    DEBUG_PRINTF("data management thread created...\n");
    while(1) {
        semaphore_p();
        DEBUG_PRINT("semaphore_p\n");
        DEBUG_PRINT("queue top...\n");
        sensor_data_ptr_t top = (sensor_data_ptr_t)queue_top(sensor_queue);
        DEBUG_PRINT("%s %p\n","queue top finished...",top);
        if(top) {
            DEBUG_PRINT("insert sensor data to list: %d %d %ld\n",top->id,top->tmp,top->ts);
            list_node_ptr_t node = list_get_first_reference(sensor_list);
            list_element_ptr_t element;
            while(node) {
                list_element_ptr_t e = (list_element_ptr_t)list_get_element_at_index(sensor_list, 0);
                if( e && e->sensor_id == top->id ){
                    break;
                } else {
                    node = list_get_next_reference(sensor_list, node);
                }
            }

            if(node) {
                element = list_get_element_at_reference(sensor_list, node);
                element->avg_tmp = element->avg_tmp / 1.25f + top->tmp / 50.0f;
                element->ts = top->ts;
            } else {
                MALLOC(element, sizeof(list_element_t));
                element->sensor_id = top->id;
                element->room_id = top->id % 10;
                element->avg_tmp = top->tmp / 10.0f;
                element->ts = top->ts;
                list_insert_at_end(sensor_list, element);
            }
            list_print(sensor_list);

            top->statu ++;
            if(top->statu >= 2) {
                queue_dequeue(sensor_queue);
            }
        }
    }
	
    DEBUG_PRINTF("%s","data management thread stopped...\n");
	pthread_exit((void*)("data management thread stopped...")); //better to describe thread exit status
}


static void* storage_manage(void* arg) {
    DEBUG_PRINTF("%s\n","mysql thread created..");
    MYSQL* mysql = init_connection();
    if(!mysql) {
        int i = 3; //AUTO-RECONNECTION
        while(!mysql && --i){
            sleep(10);
            mysql = init_connection();
        }
    }

    if(!mysql) {
        send_log_msg(fifo_write_fds,"mysql server connection failed");
        exit(EXIT_FAILURE);
    } else {
        DEBUG_PRINT("%s\n","mysql connection created..");
    }
	
    while(1) {
        semaphore_p();
        DEBUG_PRINT("semaphore_p\n");
        DEBUG_PRINT("%s\n","queue top...");
        sensor_data_ptr_t top = (sensor_data_ptr_t)queue_top(sensor_queue);
        DEBUG_PRINT("%s %p\n","queue top finished...",top);
        if(top) {
            DEBUG_PRINT("insert sensor data to database: %d %d %ld\n",top->id,top->tmp,top->ts);
            if(insert_sensor(mysql, top->id, top->tmp, top->ts)==0) {
                DEBUG_PRINT("%s\n","write to mysql successfully...");
                MYSQL_RES* res = find_sensor_all(mysql);
                print_result(res);
                free_result(res);
                top->statu ++;
            }
            if(top->statu==2) {
                queue_dequeue(sensor_queue);
            }
        }
    }
	
    DEBUG_PRINTF("%s","storage management thread stopped...\n");
	pthread_exit((void*)("storage management thread stopped...")); //better to describe thread exit status
}

static void send_log_msg(int fds, const char* info) {
	char* msg = malloc(100);
	static unsigned int logseq = 0;
	logseq++;
	time_t t = time(NULL);
    int bytes = snprintf(msg,100,"EVENT %d: %s @ %s", logseq,info,ctime(&t));
	pthread_mutex_lock(&fifo_mutex);
	write(fds,msg,bytes);
	pthread_mutex_unlock(&fifo_mutex);
	free(msg);
}

static int set_semvalue()
{
	union semun sem_union;

	sem_union.val = 0;
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

static void child_cleanup(int signo) {
    printf("signal %d %s\n",signo,"child begin to exit...");
    exit(EXIT_FAILURE);
}

static void sig_kill_child(int sig) {
    if(sig == SIGHUP) {
        exit(EXIT_SUCCESS);
        DEBUG_PRINTF("child killed by signal");
    }
}

static void clean_before_exit(void) {
    DEBUG_PRINT("free resource before exit...\n");
    queue_free(&sensor_queue);
    list_fFree(&sensor_list);
    del_semvalue();
    fcloseall();
}

