CC=gcc

CFLAGS= -Wall


server: server.c tcpsocket.c 
	$(CC) $(CFLAGS) server.c tcpsocket.c -o server

client1: client1.c tcpsocket.c
	$(CC) $(CFLAGS) client1.c tcpsocket.c -o client1

client2: client2.c tcpsocket.c
	$(CC) $(CFLAGS) client2.c tcpsocket.c -o client2

client3: client3.c tcpsocket.c
	$(CC) $(CFLAGS) client3.c tcpsocket.c -o client3


