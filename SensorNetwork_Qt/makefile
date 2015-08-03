#pass parameters to makefile
#make mysql_server=192.168.1.100 run_server

QUEUE_SIZE=200
#SET_ID1=10

MYSQL_CFLAGS = -I/usr/include/mysql -DBIG_JOINS=1  -fno-strict-aliasing -g -DNDEBUG
MYSQL_LIBS = -L/usr/lib/x86_64-linux-gnu -lmysqlclient -lpthread -lz -lm -ldl

#MYSQL_CFLAGS := $(mysql_config --cflags)
#MYSQL_LIBS := $(mysql_config --libs)

all: init sensor_server sensor_client log_server

init:
	mkdir -p build
	mkdir -p temp

sensor_server:data_server.c  gateway.c my_queue.c linkedlist.c statistic.c tcpsocket.c thread.c
	gcc -Wall -DSET_QUEUE_SIZE=$(QUEUE_SIZE) -o build/server data_server.c my_queue.c  $(MYSQL_CFLAGS) $(MYSQL_LIBS)  gateway.c linkedlist.c statistic.c tcpsocket.c thread.c  -lpthread

sensor_client:clientsensor.c gateway.c linkedlist.c statistic.c tcpsocket.c thread.c 
	gcc -Wall -o build/sensorNode clientsensor.c  gateway.c linkedlist.c statistic.c tcpsocket.c thread.c  -lpthread
	
log_server:log-server.c
	gcc -Wall -o build/log log-server.c

run_server : build/server
	echo $(mysql_server)
	./build/server 1234 $(mysql_server)
run_client1 : build/sensorNode
	./build/sensorNode 127.0.0.1 1234 11
run_client2 : build/sensorNode
	./build/sensorNode 127.0.0.1 1234 22
run_client3 : build/sensorNode
	./build/sensorNode 127.0.0.1 1234 33
run_log : build/log
	./build/log
clean :
	rm -r build
	rm -r temp

#sensor_client1:clientsensor.c gateway.c linkedlist.c statistic.c tcpsocket.c thread.c 
#	gcc -Wall -DSET_ID=$(SET_ID1)  -o sensorNode clientsensor.c  gateway.c linkedlist.c #statistic.c tcpsocket.c thread.c  -lpthread


