#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>
#include "ttyUSB.h"

void tty_open(char* tty_name) {
	tty_fds = open(tty_name,O_RDWR | O_NOCTTY | O_NONBLOCK);
	if(tty_fds<0) {
        perror("open tty error");
        exit(EXIT_FAILURE);
	} else {
	    //tty_fptr = fdopen(tty_fds,"r+");
	}
}

void tty_serial_config(int fds) {
	/*
	 * http://www.opensource.apple.com/source/xnu/xnu-1456.1.26/bsd/sys/termios.h
	 * typedef unsigned long	tcflag_t;
	   typedef unsigned char	cc_t;
	   typedef unsigned long	speed_t;

	   struct termios {
			tcflag_t	c_iflag;	input flags 
			tcflag_t	c_oflag;	output flags 
			tcflag_t	c_cflag;	control flags 
			tcflag_t	c_lflag;	local flags 
			cc_t		c_cc[NCCS];	control chars 
			speed_t		c_ispeed;	input speed 
			speed_t		c_ospeed;	output speed 
        };   
     */
	struct termios tio_old, tio_new;
	tcgetattr(tty_fds,&tio_old); /* save the current serial port settings */
	//bzero(&newtio, sizeof(newtio));
	tio_new=tio_old;
	
	//input option:ignore parity check, map CR(Enter,carriage return,\r) to NL
	tio_new.c_iflag |= (IGNPAR | ICRNL);

	//output option: raw output
	tio_new.c_oflag &= ~OPOST;
	
	//control flags
	//8N1:8 bits, no parity check, 1 stop bit 
	tio_new.c_cflag &= ~PARENB;
	tio_new.c_cflag &= ~CSTOPB;
	tio_new.c_cflag &= ~CSIZE;
	tio_new.c_cflag |= CS8;
	//disable hardware control
	tio_new.c_cflag &= ~CRTSCTS;
	
	tio_new.c_cflag |= (CLOCAL | CREAD);	
	
	//line option: raw input mode
	tio_new.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
	
	//special control characters
	tio_new.c_cc[VMIN]  = 1;
	tio_new.c_cc[VTIME] = 0;
	
	//baud rate
	cfsetispeed(&tio_new, B115200);
	cfsetospeed(&tio_new, B115200);
	
	tcsetattr(tty_fds, TCSANOW, &tio_new);//Make changes now without waiting for data to complete
}

void tty_init() {
	
}

char* tty_header_create(Header h) {
	char seqBuf[4];
	int bits = snprintf(seqBuf,4,"%d",(int)h.seq);	
	char* header = (char*)malloc(19+bits);
	char chars[6] = {60,61,62,h.frame_type,h.fieldcnt,SEPARATOR};
	char nmbrs[13+bits];
	int i;
	for(i=0;i<10;i++) {
		nmbrs[i]=(h.id)[i];
	}
	nmbrs[10]=nmbrs[11]=nmbrs[12+bits]=SEPARATOR;
	for(i=0;i<bits;i++) {
		nmbrs[12+i]=seqBuf[i];
	}
	for(i=0;i<6;i++) header[i]=chars[i];
	for(i=6;i<19+bits;i++) header[i]=nmbrs[i-6];
	return header;
}

int tty_serial_read(int fds,char* buf, int len) {
	ssize_t ret;
	while (len != 0 && (ret = read (fds, buf, len)) != 0) {
		if(ret == −1) {
			if(errno == EINTR)   continue;	
			perror ("read");
			break;
		}
		len -= ret;
		buf += ret;
	}
}

int  tty_serial_write(int fds,char* buf, int len) {
	ssize_t ret;
	while (len != 0 && (ret = read (fds, buf, len)) != 0) {
		if(ret == −1) {
			if(errno == EINTR)   continue;	
			perror ("read");
			break;
		}
		len -= ret;
		buf += ret;
	}
}

int main() {
	/*
	Header h = {	
	.frame_type=50,
	.fieldcnt=51,
	.id="1023456789",
	.seq=129 };
	char* header = tty_header_create(h);
	printf("%s\n",header);
	*/
	
	
	
	return 0;
}

	

