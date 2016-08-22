#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <inttypes.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/select.h>
#include "config.h"
#include "sbuffer.h"
#include "connmgr.h"
#include "lib/tcpsock.h"
#include "lib/dplist.h"

#ifndef TIMEOUT
#error "TIMEOUT not defined\n"
#endif


typedef struct connection{
    tcpsock_t *client;
    time_t timestamp;
    sensor_id_t sensor_id;
} connection_t;

dplist_t * conn_list;
tcpsock_t * server_socket, * client_socket;

int listen_fd, max_fd;
fd_set readfds;

int dplist_errno;
extern FILE * fifo_wr;

void reset_select_readfds();
void handle_new_connection();
void handle_new_data(sbuffer_t *buffer);

void * connmgr_element_copy(void *element)
{
    connection_t *copy;
    copy = malloc(sizeof(*copy));
    MALLOC_ERROR(copy);

    copy->client = ((connection_t *)element)->client;
    copy->timestamp =((connection_t *)element)->timestamp;
    copy->sensor_id = ((connection_t *)element)->sensor_id;

    return copy;
}

void connmgr_element_free(void **element)
{
    free(*element);
    *element = NULL;
}

int connmgr_element_compare(void *x, void *y)
{
    int client_sdx, client_sdy;
    if (tcp_get_sd(((connection_t *)x)->client, &client_sdx) != TCP_NO_ERROR) exit(EXIT_FAILURE);
    if (tcp_get_sd(((connection_t *)y)->client, &client_sdy) != TCP_NO_ERROR) exit(EXIT_FAILURE);

    if(client_sdx == client_sdy)
        return 0;
    if(client_sdx > client_sdy)
        return 1;
    return -1;
}

void connmgr_listen(int port, sbuffer_t **buffer)
{
    struct timeval tv;
//    int listen_fd;  // listening socket fd
//    int max_fd;

    DEBUG_PRINT("Starting sensor gateway...\n");
    if (tcp_passive_open(&server_socket,port)!=TCP_NO_ERROR) exit(EXIT_FAILURE);

    if (tcp_get_sd(server_socket, &listen_fd) != TCP_NO_ERROR) exit(EXIT_FAILURE);
    max_fd = listen_fd;

    conn_list = dpl_create(connmgr_element_copy, connmgr_element_free, connmgr_element_compare);
    assert(dplist_errno == DPLIST_NO_ERROR);

    while(1) {
        int nready;
        reset_select_readfds();
        tv.tv_sec = TIMEOUT;
        tv.tv_usec = 0;
        nready = select(max_fd+1, &readfds, NULL, NULL, &tv);
        SYSCALL_ERROR(nready);

        if (nready == 0)
        {
            printf("No connection after timeout, closing server...\n");
            if (tcp_close( &server_socket )!=TCP_NO_ERROR) exit(EXIT_FAILURE);
            break;
        }
        else
        {
            if (FD_ISSET(listen_fd, &readfds)) // new client connection
                handle_new_connection();
            handle_new_data(*buffer);   // request on old connection
        }
    }

}

void connmgr_free()
{
    dpl_free(&conn_list);
    assert(dplist_errno == DPLIST_NO_ERROR);
}

void handle_new_connection()
{
    tcpsock_t * client_socket;
    if (tcp_wait_for_connection(server_socket,&client_socket)!=TCP_NO_ERROR) exit(EXIT_FAILURE);

    connection_t * conn = malloc(sizeof(*conn));
    MALLOC_ERROR(conn);

    conn->client = client_socket;
    conn->timestamp = time(NULL);
    conn->sensor_id = 0;    //sensor_id not known yet.

    dpl_insert_at_index(conn_list, conn, dpl_size(conn_list), true); /* save client */
    assert(dplist_errno == DPLIST_NO_ERROR);

    int fd;
    if (tcp_get_sd(client_socket,&fd)!=TCP_NO_ERROR) exit(EXIT_FAILURE);
    printf("New socket connection created with fd %d\n", fd);

}

void handle_new_data(sbuffer_t *buffer)
{
    sensor_data_t data;
    int bytes, result;
    int client_sd;

    dplist_node_t * ref = dpl_get_first_reference(conn_list);
    assert(dplist_errno == DPLIST_NO_ERROR);

    while(ref != NULL)/* check all the clients for data */
    {
        connection_t *conn = dpl_get_element_at_reference(conn_list, ref);
        assert(dplist_errno == DPLIST_NO_ERROR);

        if (tcp_get_sd(conn->client, &client_sd) != TCP_NO_ERROR) exit(EXIT_FAILURE);
        if(FD_ISSET(client_sd, &readfds))
        {
            // read sensor ID
            result = tcp_receive(conn->client,(void *)&data.id, &bytes);
            if (result != TCP_NO_ERROR || bytes < sizeof(data.id))
                fprintf(stderr, "error receiving sensor_id");
            // read temperature
            result = tcp_receive(conn->client,(void *)&data.value, &bytes);
            if (result != TCP_NO_ERROR || bytes < sizeof(data.value))
                fprintf(stderr, "error receiving sensor_value");// read timestamp
            result = tcp_receive(conn->client, (void *)&data.ts, &bytes);
            if (result != TCP_NO_ERROR || bytes < sizeof(data.ts))
                fprintf(stderr, "error receiving sensor_ts");

            DEBUG_PRINT("Received new data at fd %d : id:%d temperature:%4.2f ts:%d",
                        client_sd, data.id, data.value, data.ts );

            conn->timestamp = time(NULL);
            if(conn->sensor_id == 0)
            {
                conn->sensor_id = data.id;
                fprintf(fifo_wr, "Data received from new sensor node with id  %" PRIu16 "\n", data.id);
                fflush(fifo_wr);
            }

            sbuffer_data_t sbuffer_data;
            sbuffer_data.sensor_data = data;
            if (sbuffer_insert(buffer, &sbuffer_data) != SBUFFER_SUCCESS) exit(EXIT_FAILURE); //insert into sbuffer
        }
    }

    ref = dpl_get_next_reference(conn_list, ref);
    assert(dplist_errno == DPLIST_NO_ERROR);

}

void reset_select_readfds()
{
    FD_ZERO(&readfds);
    FD_SET(listen_fd, &readfds);

    dplist_node_t * ref = dpl_get_first_reference(conn_list);
    assert(dplist_errno == DPLIST_NO_ERROR);

    while(ref != NULL)/* check all the clients for data */
    {
        long int t = 0;
        connection_t *conn = dpl_get_element_at_reference(conn_list, ref);
        assert(dplist_errno == DPLIST_NO_ERROR);

        if ((t = time(NULL) - conn->timestamp) >= TIMEOUT)
        {
            //flockfile(fp_FIFO_w);
            fprintf(fifo_wr, "Closing the sensor node with %" PRIu16 " after timeout\n", conn->sensor_id);
            fflush(fifo_wr);

            if (tcp_close(&conn->client)!=TCP_NO_ERROR) exit(EXIT_FAILURE);
            DEBUG_PRINT("The sensor node with %" PRIu16 " has closed the connection\n", conn->sensor_id);

            dplist_node_t * tmp = ref;
            ref = dpl_get_next_reference(conn_list, ref);
            assert(dplist_errno == DPLIST_NO_ERROR);
            dpl_remove_at_reference(conn_list, tmp, true);
            assert(dplist_errno == DPLIST_NO_ERROR);
        }
            else
        {
            int client_sd;
            if(tcp_get_sd(conn->client, &client_sd) != TCP_NO_ERROR) exit(EXIT_FAILURE);
            FD_SET(client_sd, &readfds); /* add new client descriptor into readfds */
            max_fd = max_fd > client_sd ? max_fd : client_sd;
            ref = dpl_get_next_reference(conn_list, ref);
            assert(dplist_errno == DPLIST_NO_ERROR);
        }
    }
}

