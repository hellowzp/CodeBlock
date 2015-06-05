client_1: client_1.o
	gcc -Wall -o client_1 client_1.o -L./ -Wl,-rpath=./ -lclient

client_1.o: client_1.c  
	gcc -Wall -c client_1.c

clean:
	rm client_1.o client_1