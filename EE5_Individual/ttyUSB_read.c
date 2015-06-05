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
	char c[2] = "c";
	while(c[0]!='q') {
        int len = read(tty_fds,pkt_temp,MAX_FRAME_SIZE);
        if(len>0) {
            int ret = printf("%d %s ",len,(char*)pkt_temp);
            printf("%d\n",ret);
        } else if (len<0) {
			perror("read tty");
		}
        read(STDIN_FILENO,c,2);
	}
	return 0;
}
