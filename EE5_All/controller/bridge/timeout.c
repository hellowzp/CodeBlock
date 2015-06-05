/*******************************************************************************
*
* FILENAME: timeout.c
*
* Description:
* 
*
*******************************************************************************/

/*-----------------------------------------------------------------------------
        include files
------------------------------------------------------------------------------*/

#include "timeout.h"

/*------------------------------------------------------------------------------
        definitions (defines, typedefs, ...)
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
        function prototypes
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
        global variable declarations
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
        implementation code
------------------------------------------------------------------------------*/
pthread_mutex_t mutex_list_in = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_list_out = PTHREAD_MUTEX_INITIALIZER;


extern list_ptr_t  list_in;
extern list_ptr_t  list_out;

int num_timeout;


void timer_handler(int sig){
  printf("in message time out\n\n\n\n");
//  MESSAGE *m = (MESSAGE *)malloc(size_MESSAGE);
//  if(m == NULL){
//      err_num = MALLOC_ERR;
//      ErrHandler();//this may not be a good way to handle it.
//  }

//  memset(m,0,size_MESSAGE);

//  pthread_mutex_lock(&mutex_list_in);
//  m->address = list_in->data->address;
//  m->timeout = 1;
//  list_delete_device_timeout(&list_in);
//  pthread_mutex_unlock(&mutex_list_in);

//  senddata(m);
}


struct sigaction sa_in_message;
struct itimerval timer;


void init_timer(void){
memset (&sa_in_message, 0, sizeof (sa_in_message));
sa_in_message.sa_handler = timer_handler;
if((sigaction(SIGALRM, &sa_in_message, NULL))== -1){
    err_num = SASET_ERR;
    ErrHandler();
    return;
  }
}

void set_timer(struct timeval time_interval){
    timer.it_value = time_interval;
    timer.it_interval.tv_sec = 0;
    timer.it_interval.tv_usec = 0;
    if((setitimer (ITIMER_REAL, &timer, NULL)) == -1){
       err_num = SETITIMER_ERR;
       ErrHandler();
       return;
    }
}


list_ptr_t list_initialization(void){
    return list_init();
}

void list_destroy(list_ptr_t *list){
    list_free_all(list);
}

int list_add_in(list_ptr_t *list, TIME_PROTOCOL duration,  DADDRESS address){
    struct timeval time_interval;
    pthread_mutex_lock(&mutex_list_in);
    if((list_put(list, address, time(NULL) + duration, duration,NULL)) == NULL){
        err_num = PUT_IN_ERR;
        ErrHandler();
        return 1;
    }
    pthread_mutex_unlock(&mutex_list_in);
    time_interval.tv_sec = ((*list)->data)->duration;
    time_interval.tv_usec = 0;
    set_timer(time_interval);
    return 0;
}

int list_add_out(list_ptr_t *list, TIME_PROTOCOL duration,  DADDRESS address, MESSAGE *message){
    struct timeval time_interval;
    pthread_mutex_lock(&mutex_list_out);
    list_put(list, address, time(NULL) + duration, duration,message);
    pthread_mutex_unlock(&mutex_list_out);
    time_interval.tv_sec = ((*list)->data)->duration;
    time_interval.tv_usec = 0;
    set_timer(time_interval);
    return 0;
}

int list_delete_device_timeout(list_ptr_t *list){
    struct timeval time_interval;
    pthread_mutex_lock(&mutex_list_out);
    DADDRESS address = (*list)->data->address;
    list_delete_by_address(list, address);
    if(((*list)->data) == NULL){
        time_interval.tv_sec = 0;
    }
    else{
    time_interval.tv_sec = ((*list)->data)->duration;}
    pthread_mutex_unlock(&mutex_list_out);
    time_interval.tv_usec = 0;
    set_timer(time_interval);
    return 0;
}

int list_delete_device(list_ptr_t *list,DADDRESS address){
    pthread_mutex_lock(&mutex_list_in);
    list_delete_by_address(list, address);
    pthread_mutex_unlock(&mutex_list_in);
    return 0;
}

int list_update_device(list_ptr_t *list, DADDRESS address){
    struct timeval time_interval;
    list_ptr_t temp;
    pthread_mutex_lock(&mutex_list_in);
     list_get_by_address(*list,address,&temp);
     if(temp == NULL){
         err_num = DEVICE_NOTCONNECT_ERR;
         ErrHandler();
         return 1;
     }
     pthread_mutex_unlock(&mutex_list_in);
    TIME duration;
    duration = temp->data->duration;
    pthread_mutex_lock(&mutex_list_in);
    list_update(list,address,time(NULL)+duration);
    pthread_mutex_unlock(&mutex_list_in);
    time_interval.tv_sec = ((*list)->data)->duration;
    time_interval.tv_usec = 0;
    set_timer(time_interval);

    return 0;
}


int list_device_sleep(list_ptr_t list, DADDRESS address, TIME_PROTOCOL duration){

    struct timeval time_interval;
    list_ptr_t temp;
     pthread_mutex_lock(&mutex_list_in);
     list_get_by_address(list,address,&temp);
     if(temp == NULL){
         err_num = DEVICE_NOTCONNECT_ERR;
         ErrHandler();
         return 1;
     }
    pthread_mutex_unlock(&mutex_list_in);
    temp->data->duration = duration;
    pthread_mutex_lock(&mutex_list_in);
    list_update(&list,address,time(NULL)+duration);
    pthread_mutex_unlock(&mutex_list_in);
    time_interval.tv_sec = (list->data)->duration;
    time_interval.tv_usec = 0;
    set_timer(time_interval);

    return 0;
}
