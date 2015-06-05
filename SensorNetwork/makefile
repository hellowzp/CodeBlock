all: sensor_server  sensor_client  log_server  

SET_QUEUE_SIZE=200

#SET_ID1=10

#mysql_cflags = -ID:\application_programs\Perl\c\bin\..\include\mysql_5
#mysql_libs   = -LD:\application_programs\Perl\c\bin\..\lib -lmysql

sensor_server:data_server.c  gateway.c my_queue.c linkedlist.c statistic.c tcpsocket.c thread.c
	gcc -Wall -DSET_QUEUE_SIZE=$(SET_QUEUE_SIZE) 'mysql_config --cflags --libs' -o server data_server.c my_queue.c gateway.c linkedlist.c statistic.c tcpsocket.c thread.c  -lpthread

sensor_client:clientsensor.c gateway.c linkedlist.c statistic.c tcpsocket.c thread.c 
	gcc -Wall -o sensorNode clientsensor.c  gateway.c linkedlist.c statistic.c tcpsocket.c thread.c  -lpthread

log_server:log-server.c
	gcc -Wall -o log log-server.c

#sensor_client1:clientsensor.c gateway.c linkedlist.c statistic.c tcpsocket.c thread.c 
#	gcc -Wall -DSET_ID=$(SET_ID1)  -o sensorNode clientsensor.c  gateway.c linkedlist.c #statistic.c tcpsocket.c thread.c  -lpthread


