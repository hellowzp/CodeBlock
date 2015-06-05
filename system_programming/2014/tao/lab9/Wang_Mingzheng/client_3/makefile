client_3: client_3.o
	gcc -Wall -o client_3 client_3.o -L./ -Wl,-rpath=./ -lclient

client_3.o: client_3.c tcpsocket.c
	gcc -Wall -c client_3.c

clean:
	rm client_3.o client_3