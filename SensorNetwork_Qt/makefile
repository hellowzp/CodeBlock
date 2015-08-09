SRC = tcpsocket.c queue.c sensor_db.c 
OBJ = $(addprefix $(BUILDDIR)/, $(SRC:.c=.o))
HDR = $(SRC:.c=.h) config.h

SHRLIB = libmylist.so 
SHRLIBDIR = /usr/lib
BUILDDIR = build
TEMPDIR = temp

CFLAGS = -Wall

MYSQL_CFLAGS = -I/usr/include/mysql -DBIG_JOINS=1  -fno-strict-aliasing -g -DNDEBUG
MYSQL_LIBS = -L/usr/lib/x86_64-linux-gnu -lmysqlclient -lpthread -lz -lm -ldl

#MYSQL_CFLAGS := $(mysql_config --cflags)
#MYSQL_LIBS := $(mysql_config --libs)

.PHONY: all clean

all : INIT GATEWAY SENSOR1 SENSOR2 SENSOR3

#INIT
INIT:
	mkdir -p $(BUILDDIR)
	mkdir -p $(TEMPDIR)

#BUILDDIR
tcpsocket.o : tcpsocket.c tcpsocket.h 
	gcc $< $(CFLAGS) -c -o $(BUILDDIR)/$@
queue.o : queue.c queue.h
	gcc $< $(CFLAGS) -lpthread -c -o $(BUILDDIR)/$@ 
sensor_db.o : sensor_db.c sensor_db.h
	gcc $< $(CFLAGS) -lpthread $(MYSQL_CFLAGS) $(MYSQL_LIBS) -c -o $(BUILDDIR)/$@

$(SHRLIB) : list.c list.h
	gcc $(CFLAGS) -fPIC -shared $^ -o $(BUILDDIR)/$@ 
	sudo cp $(BUILDDIR)/$@ $(SHRLIBDIR)
	sudo ldconfig

GATEWAY : gateway.c $(OBJ)
	gcc $^ $(CFLAGS) -DDEBUG -DSET_MIN_TEMP=0 -DSET_MAX_TEMP=30 $(MYSQL_CFLAGS) $(MYSQL_LIBS) -L$(SHRLIBDIR) -lmylist -o $(BUILDDIR)/$@

SENSOR1 : sensor_node.c $(BUILDDIR)/tcpsocket.o
	gcc $^ $(CFLAGS) -o $(BUILDDIR)/$@

SENSOR2 : sensor_node.c $(BUILDDIR)/tcpsocket.o
	gcc $^ $(CFLAGS) -o $(BUILDDIR)/$@

SENSOR3 : sensor_node.c $(BUILDDIR)/tcpsocket.o
	gcc $^ $(CFLAGS) -o $(BUILDDIR)/$@

#run
run_server : ./$(BUILDDIR)/GATEWAY
	./$(BUILDDIR)/GATEWAY 1234 $(mysql_server)
client1 :
	./$(BUILDDIR)/SENSOR1 47 2 $(mysql_server) 1234
client2 :
	./$(BUILDDIR)/SENSOR2 77 2 $(mysql_server) 1234
client3 :
	./$(BUILDDIR)/SENSOR3 74 2 $(mysql_server) 1234

#clean
clean :
	rm -r $(BUILDDIR)
	rm -r $(TEMPDIR)

#gcc gateway.c tcpsocket.c list.c queue.c sensor_db.c -DSET_MIN_TEMP=0 -DSET_MAX_TEMP=30 -lpthread `mysql_config --cflags` `mysql_config --libs` -o build/gateway -lpthread -DDEBUG -DHOST=192.168.1.102
#./build/gateway 1234

#gcc sensor_node.c tcpsocket.c -o build/sensor -DSET_ID=777
#./build/sensor 127.0.0.1 1234 
