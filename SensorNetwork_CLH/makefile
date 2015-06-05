all: sensor_server  sensor_client  log_server  
SET_QUEUE_SIZE=200

#SET_ID1=10


sensor_server:data_server.c  gateway.c my_queue.c linkedlist.c statistic.c tcpsocket.c thread.c
	gcc -Wall -DSET_QUEUE_SIZE=$(SET_QUEUE_SIZE) -o server data_server.c my_queue.c  `mysql_config --cflags --libs`  gateway.c linkedlist.c statistic.c tcpsocket.c thread.c  -lpthread
sensor_client:clientsensor.c gateway.c linkedlist.c statistic.c tcpsocket.c thread.c 
	gcc -Wall -o sensorNode clientsensor.c  gateway.c linkedlist.c statistic.c tcpsocket.c thread.c  -lpthread

log_server:log-server.c
	gcc -Wall -o log log-server.c

#sensor_client1:clientsensor.c gateway.c linkedlist.c statistic.c tcpsocket.c thread.c 
#	gcc -Wall -DSET_ID=$(SET_ID1)  -o sensorNode clientsensor.c  gateway.c linkedlist.c #statistic.c tcpsocket.c thread.c  -lpthread


