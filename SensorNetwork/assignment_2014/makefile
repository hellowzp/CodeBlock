EXE = sensor gateway log
LDIR = lib
CC = gcc
CFLAGS = -Wall -c $(DEFINES)
LFLAGS = -Wall -L./$(LDIR) -Wl,-rpath=./$(LDIR)
CDFLAGS = -Wall -c -fPIC
LDFLAGS = -Wall -shared
.SUFFIXES:

all: sensor gateway log

$(EXE) : % : %.o $(LDIR)/liblist.so $(LDIR)/libmyqueue.so $(LDIR)/libtcpsocket.so $(LDIR)/libsensor_db.so
	@echo "\n***** LINKING $< *****"
	$(CC) $(LFLAGS) $< -llist -lmyqueue -ltcpsocket -lsensor_db -lpthread -o $@
	
liblist libmyqueue libtcpsocket libsensor_db : % : $(LDIR)/%.so
	
%.o : %.c %.check
	@echo "\n***** COMPILING $< *****"
	$(CC) $(CFLAGS) -o$@ $<
	
$(LDIR)/lib%.so : %.c %.check
	$(eval name = $(basename $<))
	@echo "\n***** COMPILING LIB $< *****"
	$(CC) $(CDFLAGS) $<
	mkdir -p $(LDIR)
	@echo "\n***** LINKING LIB $< *****"
	$(CC) $(LDFLAGS) $(name).o -o $(LDIR)/lib$(name).so

.PHONY : clean clean-all *.check

%.check : %.c
	@echo "\n***** CPPCHECK $< *****"
	cppcheck --enable=all $^
	
clean:
	rm -f *.o $(EXE) *~

clean-all: clean
	rm -rf $(LDIR)
