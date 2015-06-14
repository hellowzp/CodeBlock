SRC = tcpsocket.c myqueue.c sensor_db.c 
OBJ = $(SRC:.c=.o)
HDR = $(SRC:.c=.h) config.h
MODULE = libmylist.so 

CFLAGS = -Wall
USER_LIB = /usr/lib

MYSQL_CFLAGS = -I/usr/include/mysql -DBIG_JOINS=1  -fno-strict-aliasing -g -DNDEBUG
MYSQL_LIBS = -L/usr/lib/x86_64-linux-gnu -lmysqlclient -lpthread -lz -lm -ldl

#MYSQL_CFLAGS := $(mysql_config --cflags)
#MYSQL_LIBS := $(mysql_config --libs)

#MYSQL_CFLAGS = 'mysql_config --cflags'
#MYSQL_LIBS   = 'mysql_config --libs'

all : GATEWAY SENSOR1 SENSOR2 SENSOR3

#build
GATEWAY : gateway.c $(OBJ) $(MODULE)
	gcc $^ -DDEBUG -DSET_MIN_TEMP=0 -DSET_MAX_TEMP=30 $(MYSQL_CFLAGS) $(MYSQL_LIBS) -L$(USER_LIB) -lmylist -o $@.out

SENSOR1 : sensor_node.c tcpsocket.o
	gcc $^ -DSET_ID=23 -o $@.out	

SENSOR2 : sensor_node.c tcpsocket.o
	gcc $^ -DSET_ID=47 -o $@.out

SENSOR3 : sensor_node.c tcpsocket.o
	gcc $^ -DSET_ID=77 -o $@.out

#run
server :
	./GATEWAY.out 1234
client1 :
	./SENSOR1.out 127.0.0.1 1234
client2 :
	./SENSOR2.out 127.0.0.1 1234
client3 :
	./SENSOR3.out 127.0.0.1 1234

$(OBJ) : $(SRC) $(HDR)

tcpsocket.o : tcpsocket.c tcpsocket.h 
	gcc $< -c -o $@
myqueue.o : myqueue.c myqueue.h
	gcc $< -lpthread -c -o $@ 
sensor_db.o : sensor_db.c sensor_db.h
	gcc $< -lpthread $(MYSQL_CFLAGS) $(MYSQL_LIBS) -c -o $@

$(MODULE) : list.c list.h
	gcc -fPIC -shared $^ -o $@ 
	cp $@ $(USER_LIB)
	sudo ldconfig

clean :
	rm *.o GATEWAY SENSOR1 SENSOR2 SENSOR3 *.msg *.log 

#gcc gateway.c tcpsocket.c list.c myqueue.c sensor_db.c -DSET_MIN_TEMP=0 -DSET_MAX_TEMP=30 -lpthread `mysql_config --cflags` `mysql_config --libs` -o gateway -lpthread -DDEBUG 
#./gateway 1234

#gcc sensor_node.c tcpsocket.c -o sensor -DSET_ID=777
#./sensor 127.0.0.1 1234 



