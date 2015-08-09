#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <assert.h>
#include "tcpsocket.h"

#define PORT 1234

int main( void )
{
  Socket server, client;
  uint16_t sensor_id;
  double temperature;
  time_t timestamp;
  int bytes;

  // open server socket
  server = tcp_passive_open( PORT );
  
  client = tcp_wait_for_connection( server );
  printf("incoming client connection\n");

  do
  {
    bytes = tcp_receive( client, (void *)&sensor_id, sizeof(sensor_id));
    // bytes == 0 indicates tcp connection teardown
    assert( (bytes == sizeof(sensor_id)) || (bytes == 0) );	
    bytes = tcp_receive( client, (void *)&temperature, sizeof(temperature));
    assert( (bytes == sizeof(temperature)) || (bytes == 0) );
    bytes = tcp_receive( client, (void *)&timestamp, sizeof(timestamp));
    assert( (bytes == sizeof(timestamp)) || (bytes == 0) );    
    if (bytes) 
    {
      printf("sensor id = %" PRIu16 " - temperature = %g - timestamp = %ld\n", sensor_id, temperature, (long int)timestamp );
    }
  } while (bytes);

  //sleep(1);	/* to allow socket to drain */
  tcp_close( &client );
  tcp_close( &server );
  return 0;
}

