EXE=listDemo
LDIR=lib
DBJS=main.o list.o
CC=gcc
CFLAGS=-Wall -c
LFLAGS=-Wall
CDFLAGS=-Wall -c -fPIC
LDFLAGS=-Wall -shared

mkdir -p $(LDIR)

ar rs S(LDIR)/liblist.a.list.o

      S(CC) S(LFLAGS) main.o -llist -L./S(LDIR) -o S(EXE)

dynamic list main.o d-list.o
      
      $(CC) S(LFLAGS)  main.o  -llist -L./$(LDIR) -wL,-rpath=./$(LDIR) -o $(EXE)
      
clean:
       rm -f *.o $(EXE)*~
clean -libs:clean
       rm -rf S(LDIR)       
       
       
main.o:main.c list.h
        
       S(CC) SC(FLAGS) main.c

list.o:list.c list.h
      
       S(CC) SC(FLAGS) list.c
       
d-list.o:list.c list.h
     
        S(CC) S(CDFLAGS) list.c
        mkdir -p S(LDIR)    
        S(CC) SC(LDFLASS) list.o -o $(LDIR)/liblist.so               
