client: client.o
	gcc -Wall -o client -L./ -Wl,rpath=./ -lclient

client.o: client.c tcpsocket.c
	gcc -Wall -c client

clean:
	rm client.o client