CC=gcc

CFLAGS= -Wall


server: server.c tcpsocket.c gateway.c D_list.c
	$(CC) $(CFLAGS) server.c tcpsocket.c gateway.c D_list.c -lpthread -o server

client1: client1.c tcpsocket.c
	$(CC) $(CFLAGS) client1.c tcpsocket.c -o client1
all: server client1

D_list.o: D_list.c
	$(CC) -fPIC -c D_list.c
gateway.o:gateway.c
	$(CC) -fPIC -c gateway.c
liblist.so:D_list.o gateway.o
	$(CC) -shared -o liblist.so D_list.o gateway.o

