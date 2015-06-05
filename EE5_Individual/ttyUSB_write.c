#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

#include <string.h>
#include "ttyUSB.h"

int main() {
	tty_open(MODEMDEVICE);
	tty_serial_config(tty_fds);
	
	/* char test
	char a[2] = "01";
	char b[2] = {0,1};
	printf("%c %c %d %d\n",a[1],b[1],a[1],b[1]);
	*/
	char c[2] = "c";
	char outbuf[] = {0x7E,0x00,0x04,0x08,0x01,0x4E,0x49,0x5F};
	char* inbuf;
	//int sl = strlen(outbuf);  //==>1 0x00 will end counting
	//int len = write(STDOUT_FILENO,outbuf,8);
	//printf("%d\n",len);
	while(c[0]!='q') {
        read(STDIN_FILENO,c,2);
        if(c[0]=='w') {
            int l = write(tty_fds,outbuf,8);
            printf("%c %d %d\n",c[1],c[1],l);
        }
	}
	return 0;
}
