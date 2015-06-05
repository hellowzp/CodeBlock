CFLAGS = -Wall -c
LFLAGS = -Wall
CC = gcc
OBJS = list.o
TOBJS = list_test.o
EXE = list

all: $(OBJS) main.o
	$(CC) $(FLAGS) $(OBJS) main.o -o $(EXE)
	./$(EXE)

test: $(OBJS) $(TOBJS)
	$(CC) -g $(LFLAGS) $(OBJS) $(TOBJS) -lcheck `pkg-config --cflags --libs check`-o $(EXE)_test
	./$(EXE)_test

main.o: main.c
	$(CC) $(CFLAGS) main.c

list.o: list.h list.c
	$(CC) $(CFLAGS) list.c

list_test.o: list_test.c list.h
	$(CC) $(CFLAGS) list_test.c

clean:
	rm -f *~ *.o $(EXE) $(EXE)_test
