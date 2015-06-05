#ifndef TTYUSB_H
#define TTYUSB_H

#define MODEMDEVICE "/dev/ttyUSB0"
#define MAX_FRAME_SIZE 100
#define SEPARATOR 35 //ASC value of the separator #

typedef unsigned char Byte;
typedef struct {
	Byte frame_type;
	Byte fieldcnt;
	char id[10];
	Byte seq;
} Header;

/*
typedef struct {
	...
} Payload;
*/	
	
int tty_fds;
FILE* tty_fptr;
char inbuf[MAX_FRAME_SIZE],outbuf[MAX_FRAME_SIZE];

void tty_open(char* tty_name);
void tty_serial_config(int fds);
void tty_init();
char* tty_header_create(Header h);
void tty_header_read(Header h);
char* tty_payload_create();
int  tty_serial_read(int fds,char* buf, int len);
int  tty_serial_write(int fds,char* buf, int len);

#endif
