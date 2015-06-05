#gcc gateway.c tcpsocket.c list.c myqueue.c -DSET_MIN_TEMP=-20 -DSET_MAX_TEMP=40 -lpthread -o GATE
#./GATE 1234
gateway : list.o tcpsocket.o myqueue.o
	gcc -DSET_MIN_TEMP=-20 -DSET_MAX_TEMP=40 -lpthread -o GATE
server : 
	./GATE 1234

#gcc gateway.c tcpsocket.c list.c myqueue.c -DSET_MIN_TEMP=-20 -DSET_MAX_TEMP=40 -lpthread -o GATE
#sudo ./SENSOR 127.0.0.1 1234 
sensor : tcpsocket.o
	gcc -DSET_ID=111 -o SENSOR
client :
	./SENSOR
list.0 : list.c
	gcc list.c -c list.o
tcpsocket.o : tcpsocket.c
	gcc tcpsocket.c -c tcpsocket.o
myqueue.o : myqueue.c
	gcc myqueue.c -lpthread -c myqueue.o
