all: sensor_server  sensor_client  log_server  
QUEUE_SIZE=200

#SET_ID1=10


sensor_server:data_server.c  gateway.c my_queue.c linkedlist.c statistic.c tcpsocket.c thread.c
	gcc -Wall -DSET_QUEUE_SIZE=$(QUEUE_SIZE) -o server data_server.c my_queue.c  `mysql_config --cflags --libs`  gateway.c linkedlist.c statistic.c tcpsocket.c thread.c  -lpthread
sensor_client:clientsensor.c gateway.c linkedlist.c statistic.c tcpsocket.c thread.c 
	gcc -Wall -o sensorNode clientsensor.c  gateway.c linkedlist.c statistic.c tcpsocket.c thread.c  -lpthread

log_server:log-server.c
	gcc -Wall -o log log-server.c

run_server : server
	./server 1234
run_client1 : sensorNode
	./sensorNode 127.0.0.1 1234 11
run_client2 : sensorNode
	./sensorNode 127.0.0.1 1234 22
run_client3 : sensorNode
	./sensorNode 127.0.0.1 1234 33
run_log : log
	./log
clean :
	rm server
	rm sensorNode
	rm log

#sensor_client1:clientsensor.c gateway.c linkedlist.c statistic.c tcpsocket.c thread.c 
#	gcc -Wall -DSET_ID=$(SET_ID1)  -o sensorNode clientsensor.c  gateway.c linkedlist.c #statistic.c tcpsocket.c thread.c  -lpthread


