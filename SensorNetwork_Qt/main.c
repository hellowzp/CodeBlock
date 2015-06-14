#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(void)
{
    char buf[2];
    //block io will not consume cpu
    read(0, buf, 2);  //the ending enter char
    printf("Hello World \%s!\n",buf);
    return 0;
}

