CC -gcc
CFLAGS =-Wall -c
LFLAGS = -Wall


gateway :gateway.o
           S(CC) S(LFLAGS) S^ -o$@               //s<
simulator:simulator.o
                  $(CC) $(LFLAGS) s^-o$@            //s<
*.o;*.c


                                              //make simulator ;make gateway
clean:
rm -f *.o $EXE *~