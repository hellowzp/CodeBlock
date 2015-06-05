server: server.o
	gcc -Wall -o server server.o -L./ -Wl,-rpath=./ -lserver

server.o: server.c tcpsocket.h double_list.h
	gcc -Wall -c server.c

clean: 
	rm server.o server