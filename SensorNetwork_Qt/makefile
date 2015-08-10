SRC = tcpsocket.c queue.c sensor_db.c 
OBJ = $(addprefix $(BUILDDIR)/, $(SRC:.c=.o))
HDR = $(SRC:.c=.h) config.h

SHRLIB = libmylist.so 
SHRLIBDIR = /usr/lib
BUILDDIR = build
TEMPDIR = temp

CFLAGS = -DDEBUG -Wall -Wno-unused-variable -Wno-unused-function

MYSQL_CFLAGS = -I/usr/include/mysql -DBIG_JOINS=1  -fno-strict-aliasing -g -DNDEBUG
MYSQL_LIBS = -L/usr/lib/x86_64-linux-gnu -lmysqlclient -lpthread -lz -lm -ldl

#MYSQL_CFLAGS := $(mysql_config --cflags)
#MYSQL_LIBS := $(mysql_config --libs)

.PHONY: all clean

all : INIT $(OBJ) $(BUILDDIR)/$(SHRLIB) GATEWAY SENSORNODE

#init
INIT:
	mkdir -p $(BUILDDIR)
	mkdir -p $(TEMPDIR)

#build
$(BUILDDIR)/tcpsocket.o : tcpsocket.c tcpsocket.h 
	gcc $< $(CFLAGS) -c -o $@
$(BUILDDIR)/queue.o : queue.c queue.h
	gcc $< $(CFLAGS) -lpthread -c -o $@ 
$(BUILDDIR)/sensor_db.o : sensor_db.c sensor_db.h
	gcc $< $(CFLAGS) $(MYSQL_CFLAGS) $(MYSQL_LIBS) -c -o $@

$(BUILDDIR)/$(SHRLIB) : list.c list.h
	gcc $(CFLAGS) -fPIC -shared $^ -o $@ 
	sudo cp $@ $(SHRLIBDIR)
	sudo ldconfig

GATEWAY : gateway.c $(OBJ)
	gcc $^ $(CFLAGS) -DSET_MIN_TEMP=17 -DSET_MAX_TEMP=23 $(MYSQL_CFLAGS) $(MYSQL_LIBS) -L$(SHRLIBDIR) -lmylist -o $(BUILDDIR)/$@

SENSORNODE : sensor_node.c $(BUILDDIR)/tcpsocket.o
	gcc $^ $(CFLAGS) -DLOOPS=5 -o $(BUILDDIR)/$@

#run
run_server : ./$(BUILDDIR)/GATEWAY
	./$(BUILDDIR)/GATEWAY 1234 $(mysql_server)
run_client1 :
	./$(BUILDDIR)/SENSORNODE 47 2 127.0.0.1 1234
run_client2 :
	./$(BUILDDIR)/SENSORNODE 77 2 127.0.0.1 1234
run_client3 :
	./$(BUILDDIR)/SENSORNODE 74 2 127.0.0.1 1234

#clean
clean :
	rm -r $(BUILDDIR)
	rm -r $(TEMPDIR)

#gcc gateway.c tcpsocket.c list.c queue.c sensor_db.c -DSET_MIN_TEMP=0 -DSET_MAX_TEMP=30 -lpthread `mysql_config --cflags` `mysql_config --libs` -o build/gateway -lpthread -DDEBUG
#./build/gateway 1234 mysql_server=192.168.1.102
#make run_server mysql_server=192.168.1.102

#gcc sensor_db.c -lpthread `mysql_config --cflags` `mysql_config --libs` -c -o build/sensor_db.o

#gcc sensor_node.c tcpsocket.c -o build/sensor -DSET_ID=777
#./build/sensor 127.0.0.1 1234 
