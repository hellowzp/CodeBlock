#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <fcntl.h>
#include <errno.h>
#include <pthread.h>
#include <signal.h>
#include <time.h>
#include <inttypes.h>
#include <unistd.h>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/prctl.h>
#include <sys/wait.h>


#include "config.h"
#include "sbuffer.h"
#include "connmgr.h"
#include "lib/tcpsock.h"
#include "lib/dplist.h"

#ifndef TIMEOUT
#error "TIMEOUT preprocessor directive not defined\n"
//#define TIMEOUT 10
#endif

static void * handle_client_connection(void*);

tcpsock_t * server_socket;
extern sbuffer_t * buffer;

void setup_server(int port) {
    DEBUG_PRINT("setting up tcp server...\n");
    if (tcp_passive_open(&server_socket, port) != TCP_NO_ERROR)
            exit(EXIT_FAILURE);
    DEBUG_PRINT("server waiting for new client...\n");

    while (1) {
        tcpsock_t * client_socket;
        if ( tcp_wait_for_connection(server_socket, &client_socket) != TCP_NO_ERROR )
            exit(EXIT_FAILURE);
        pthread_t resp;
        if (client_socket != NULL) {
            pthread_create(&resp, NULL, handle_client_connection, client_socket);  //new thread need the client as the parameter
        }
    }

    //sleep(1);	/* to allow socket to drain */
    tcp_close( &server_socket );

    pthread_exit((void*)"tcp connection thread return successfully...");
}

static void * handle_client_connection(void* client) {
    DEBUG_PRINT("new client thread created..\n");
//    send_log_msg(fifo_write_fds, "new sensor node connected");

    sensor_id_t id;
    sensor_value_t temperature;
    sensor_ts_t timestamp;
    sensor_data_t sensor_data;

    int bytes;
    tcpsock_t * client_socket = (tcpsock_t *)client;
    do {
        bytes = tcp_receive( client_socket, (void *)&id, &bytes);
        // bytes == 0 indicates tcp connection teardown
        assert( (bytes == sizeof(id)) || (bytes == 0) );
        bytes = tcp_receive( client_socket, (void *)&temperature, &bytes);
        assert( (bytes == sizeof(temperature)) || (bytes == 0) );
        bytes = tcp_receive( client_socket, (void *)&timestamp, &bytes);
        assert( (bytes == sizeof(timestamp)) || (bytes == 0) );
        if (bytes) {
          printf("\nserver received new data:\nsensor id = %" PRIu16 " - temperature = %g - timestamp = %ld\n",
                  id, temperature, (long int)timestamp );
        }
//        if(temperature<=min_tmp || temperature>=max_tmp) {
//            char* msg;
//            MALLOC(msg,80);
//            snprintf(msg, 80, "Warning: abnormal temperature %4.2f @ Sensor %d",
//                     temperature, id);
//            send_log_msg(fifo_write_fds, msg);
//            free(msg);
//        }

        sensor_data.id = id;
        sensor_data.value = temperature;
        sensor_data.ts = timestamp;

        sbuffer_data_t buffer_data = {sensor_data};
        if ( sbuffer_insert(buffer, &buffer_data) != SBUFFER_SUCCESS )
            ERROR_PRINT("sbuffer_insert ERROR");
//
        sleep(TIMEOUT);
      } while (bytes);

    tcp_close( &client_socket );
//    send_log_msg(fifo_write_fds,"sensor node disconnected");
    DEBUG_PRINT("client thread with sensor_id %d stopped...\n",id);
    pthread_exit((void*)("client closed...")); //better to describe client exit status
}


void close_server() {
    if (server_socket != NULL)
        tcp_close(&server_socket);
//    if (conn_list != NULL)
//        dpl_free(&conn_list);
}
