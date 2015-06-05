EXE = zigbee_network
CC = gcc
CFLAGS = gcc -c  
LFALGS = -Wall  

all: zigbee_network

$(EXE) :sorted_double_linked_list.o timeout.o process_monitor.o coordinator.o handle_message_in.o  handle_message_out.o err_handler.o interface.o  prioritized_list.o parser.o stack.o mqtt_sn_db.o
	$(CC)  sorted_double_linked_list.o timeout.o process_monitor.o handle_message_in.o handle_message_out.o err_handler.o  interface.o  mqtt_sn_db.o -DDEBUG -DLOCKING  -l sqlite3 prioritized_list.o -lpthread parser.o coordinator.o stack.o -o zigbee_network
	
sorted_double_linked_list.o:sorted_double_linked_list/sorted_double_linked_list.c
	$(CFLAGS) sorted_double_linked_list/sorted_double_linked_list.c

timeout.o:timeout.c
	$(CFLAGS) timeout.c 

process_monitor.o:process_monitor.c
	$(CFLAGS) process_monitor.c

handle_message_in.o:handle_message_in.c
	$(CFLAGS) handle_message_in.c

handle_message_out.o:handle_message_out.c
	$(CFLAGS) handle_message_out.c

err_handler.o:err_handler.c
	$(CFLAGS) err_handler.c

interface.o:interface.c
	$(CFLAGS) interface.c

mqtt_sn_db.o: ../mqtt_sn_db/mqtt_sn_db.c
	$(CFLAGS) ../mqtt_sn_db/mqtt_sn_db.c -DDEBUG -DLOCKING -l sqlite3

prioritized_list.o:../mqtt_sn_db/prioritized_list/prioritized_list.c 
	$(CFLAGS) ../mqtt_sn_db/prioritized_list/prioritized_list.c -lpthread

parser.o:../mqtt_sn_db/parser/parser.c
	$(CFLAGS) ../mqtt_sn_db/parser/parser.c 

stack.o:../mqtt_sn_db/stack/stack.c
	$(CFLAGS) ../mqtt_sn_db/stack/stack.c 

coordinator.o:../connection/coordinator.c
	$(CFLAGS) ../connection/coordinator.c
	
clean:
	rm -f  $(EXE) *~  db.db child_exit_status 

clear:
	clear 






