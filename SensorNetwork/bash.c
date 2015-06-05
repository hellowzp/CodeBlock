#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define src "~/.bash_history"
#define des "~/bash_history"

int main() {
	char* buf = malloc(100);
	memset(buf,0,100);
	FILE* fsrc = fopen(src,"r");
	FILE* fdes = fopen(src,"a");
	while(fgets(buf,100,fsrc)) {
		if(!strstr(buf,"gcc") && !strstr(buf,"cd") && !strstr(buf,"ls") && !strstr(buf,"./") && !strstr(buf,"free"))
			fputs(buf,fdes);
	}
	free(buf);
	return 0;
}
