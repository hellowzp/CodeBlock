#ifndef _CONNMGR_H_
#define _CONNMGR_H_

#include "sbuffer.h"

typedef struct connection connection_t;

void connmgr_free();
/*This method should be called to clean up the connmgr, and to free all used
 * memory. After this no new connections will be accepted*/

void connmgr_listen(int port_number, sbuffer_t **buffer);
/*This method holds the core functionality of your connmgr. It starts listening
 * on the given port and when when a sensor node connects it stores the sensor
 * data in the shared buffer.*/

void build_select_readfds(int * max_fd);
/*This method creates the set of descriptors for select function*/

//void handle_new_data(FILE * fp_bin, sbuffer_t *buffer);
void handle_new_data(sbuffer_t *buffer);
/*This method reads data from client socket and pass them to write_data_to_file().*/

void handle_new_connection();
/*This method waits for new connection, When there is new connection, the info of the
 * connection will be stored in the conn_list.*/
#ifndef DEBUG
void write_data_to_file(FILE *fp_bin, sensor_id_t id, sensor_value_t value, sensor_ts_t ts);
/*This method writes the data to a sensor_data_recv file in binary format */
#endif

#endif /* _CONNMGR_H_*/
