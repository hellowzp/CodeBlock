#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <inttypes.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/select.h>
#include "config.h"
#include "errmacros.h"
#include "sbuffer.h"
#include "connmgr.h"
#include "./lib/tcpsock.h"
#include "./lib/dplist.h"

#ifndef TIMEOUT
#error "TIMEOUT not defined\n"
#endif


struct connection{
	tcpsock_t *client;
	time_t timestamp;
	sensor_id_t sensor_id;
} connection;

dplist_t *conn_list = NULL;
tcpsock_t * server, * client;
fd_set readfds;
int dplist_errno;
extern FILE * fifo_wr;

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

void handle_new_connection()
{			
	if (tcp_wait_for_connection(server,&client)!=TCP_NO_ERROR) exit(EXIT_FAILURE);
	connection_t * conn = NULL;

	conn = malloc(sizeof(*conn));
	MALLOC_ERROR(conn);

	conn->client = client;
	conn->timestamp = time(NULL);
	conn->sensor_id = 0;    //sensor_id not known yet.

	dpl_insert_at_index(conn_list, conn, dpl_size(conn_list), true); /* save client */
  assert(dplist_errno == DPLIST_NO_ERROR);
	
	printf("Peer has opened a connection\n");

	free(conn);
}
#ifdef DEBUG //save sensor data in binary format 
void write_data_to_file(FILE *fp_bin, sensor_id_t id, sensor_value_t value, sensor_ts_t ts)
{
  fwrite(&id, sizeof(id), 1, fp_bin);
	fwrite(&value, sizeof(value), 1, fp_bin);
 	fwrite(&ts, sizeof(ts), 1, fp_bin);
	if (ferror(fp_bin) != 0)
	{
		printf("failed to write_data_to_file\n");
		exit(EXIT_FAILURE);
	}	
}
#endif
void handle_new_data(sbuffer_t *buffer)
{
  sensor_data_t data;
  int bytes, result;
	int client_sd;

#ifdef DEBUG // save sensor data in text format and binary format for test purposes
		FILE *fp_text, *fp_bin;
    fp_text = fopen("sensor_data_text", "a");
    FILE_ERROR(fp_text,"Couldn't create sensor_data in text\n");
    fp_bin = fopen("sensor_data_recv", "a");
    FILE_ERROR(fp_text,"Couldn't create sensor_data in binary\n");
#endif

	dplist_node_t * ref = dpl_get_first_reference(conn_list);
  assert(dplist_errno == DPLIST_NO_ERROR);

	while(ref != NULL)/* check all the clients  for data */
	{
		connection_t *conn = NULL;
		conn = dpl_get_element_at_reference(conn_list, ref);
  	assert(dplist_errno == DPLIST_NO_ERROR);

		if (tcp_get_sd(conn->client, &client_sd) != TCP_NO_ERROR) exit(EXIT_FAILURE);
		if(FD_ISSET(client_sd, &readfds))
		{
			// read sensor ID
			bytes = sizeof(data.id);
			tcp_receive(conn->client,(void *)&data.id,&bytes);
			// read temperature
			bytes = sizeof(data.value);
			tcp_receive(conn->client,(void *)&data.value,&bytes);
			// read timestamp
			bytes = sizeof(data.ts);
			result = tcp_receive(conn->client, (void *)&data.ts,&bytes);
			if ((result==TCP_NO_ERROR) && bytes) 
			{
				conn->timestamp = time(NULL);
				if(conn->sensor_id == 0)
				{
					//flockfile(fifo_wr);
					fprintf(fifo_wr, "A sensor node with %" PRIu16 " has opened a new connection\n", data.id);
					fflush(fifo_wr);
					//funlockfile(fifo_wr);

#ifdef DEBUG
					fprintf(stderr, "DEBUG massge, A sensor node with %" PRIu16 " has opened a new connection\n", data.id);
#endif
				}

				conn->sensor_id = data.id;

				sbuffer_data_t sbuffer_data;
				sbuffer_data.sensor_data = data;
				if (sbuffer_insert(buffer, &sbuffer_data) != SBUFFER_SUCCESS) exit(EXIT_FAILURE); //insert into sbuffer
#ifdef DEBUG
				write_data_to_file(fp_bin, data.id, data.value, data.ts);
				fprintf(fp_text,"%" PRIu16 " %g %ld\n", data.id,data.value, data.ts); 	//save sensor data in text format.
#endif      
			}
			else
			{
				if(tcp_close(&conn->client) != TCP_NO_ERROR) exit(EXIT_FAILURE);
				FD_CLR(client_sd, &readfds);

				//flockfile(fifo_wr);
				fprintf(fifo_wr, "The sensor node with %" PRIu16 " has closed the connection\n", conn->sensor_id);
				fflush(fifo_wr);
				//funlockfile(fifo_wr);

#ifdef DEBUG
				fprintf(stderr, "DEBUG log_mssge, The sensor node with %" PRIu16 " has closed the connection\n", conn->sensor_id);
#endif

				dplist_node_t *tmp = ref;
				ref = dpl_get_previous_reference(conn_list, ref);
  			assert(dplist_errno == DPLIST_NO_ERROR);
				dpl_remove_at_reference(conn_list, tmp, true);
  			assert(dplist_errno == DPLIST_NO_ERROR);

				if (result==TCP_CONNECTION_CLOSED) 
					printf("Peer has closed connection\n");
				else
					printf("Error occured on connection to peer\n");
			}
		}
		ref = dpl_get_next_reference(conn_list, ref);	
  	assert(dplist_errno == DPLIST_NO_ERROR);
	}
#ifdef DEBUG
	FILE_CLOSE_ERROR(fclose(fp_bin));
	FILE_CLOSE_ERROR(fclose(fp_text));
#endif
}

void build_select_readfds(int * max_fd)
{
	int client_sd;
	int listen_fd;

	if (tcp_get_sd(server, &listen_fd) != TCP_NO_ERROR) exit(EXIT_FAILURE);

	FD_ZERO(&readfds);
	FD_SET(listen_fd, &readfds);

	dplist_node_t * ref = dpl_get_first_reference(conn_list);
  assert(dplist_errno == DPLIST_NO_ERROR);

	while(ref != NULL)/* check all the clients  for data */
	{
		long int t = 0;
		connection_t *conn = NULL;

		conn = dpl_get_element_at_reference(conn_list, ref);
  	assert(dplist_errno == DPLIST_NO_ERROR);

		if ((t = time(NULL) - conn->timestamp) >= TIMEOUT)
		{
			//flockfile(fifo_wr);
			fprintf(fifo_wr, "The sensor node with %" PRIu16 " has closed the connection\n", conn->sensor_id);
			fflush(fifo_wr);
			//funlockfile(fifo_wr);

#ifdef DEBUG
			fprintf(stderr, "DEBUG log_mssge, The sensor node with %" PRIu16 " has closed the connection\n", conn->sensor_id);
#endif

			if (tcp_close(&conn->client)!=TCP_NO_ERROR) exit(EXIT_FAILURE);
			printf("connection timout, disconnect peer %ld\n", t);

			dplist_node_t *tmp = ref;
			ref = dpl_get_next_reference(conn_list, ref);
  		assert(dplist_errno == DPLIST_NO_ERROR);
			dpl_remove_at_reference(conn_list, tmp, true);
  		assert(dplist_errno == DPLIST_NO_ERROR);
		}
		else
		{
			if(tcp_get_sd(conn->client, &client_sd) != TCP_NO_ERROR) exit(EXIT_FAILURE);
			FD_SET(client_sd, &readfds); /* add new client descriptor into readfds */
			*max_fd = ((*max_fd) > client_sd ? *max_fd : client_sd);
			ref = dpl_get_next_reference(conn_list, ref);
  		assert(dplist_errno == DPLIST_NO_ERROR);
		}
	}
}

void connmgr_listen(int port_number, sbuffer_t **buffer)
{
	struct timeval tv;
	int listen_fd;
	int max_fd;
	
  printf("Sensor gateway is started\n");
  if (tcp_passive_open(&server,port_number)!=TCP_NO_ERROR) exit(EXIT_FAILURE);
		
	if (tcp_get_sd(server, &listen_fd) != TCP_NO_ERROR) exit(EXIT_FAILURE);
	max_fd = listen_fd;

	conn_list = dpl_create(connmgr_element_copy, connmgr_element_free, connmgr_element_compare);
  assert(dplist_errno == DPLIST_NO_ERROR);

	do
	{
		int nready;
		build_select_readfds(&max_fd);
	 	tv.tv_sec = TIMEOUT;
		tv.tv_usec = 0;
		nready = select(max_fd+1, &readfds, NULL, NULL, &tv);
		SYSCALL_ERROR(nready);

		if (nready == 0)
		{
			printf("connection time out\n");
			if (tcp_close( &server )!=TCP_NO_ERROR) exit(EXIT_FAILURE);
 		 	printf("Sensor gatway is shutting down\n");

			break;
		}
		else
		{
			if (FD_ISSET(listen_fd, &readfds)) /* new client connection */
				handle_new_connection();
			handle_new_data(*buffer);
		}
	}while(1);

}

void connmgr_free()
{
	dpl_free(&conn_list);
  assert(dplist_errno == DPLIST_NO_ERROR);
}
