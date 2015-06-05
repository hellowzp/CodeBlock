client_2: client_2.o
	gcc -Wall -o client_2 client_2.o -L./ -Wl,-rpath=./ -lclient

client_2.o: client_2.c tcpsocket.c
	gcc -Wall -c client_2.c

clean:
	rm client_2.o client_2