#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

int main(int argc, char *argv[])
{
    uint8_t v = 0;
    v |= 0x80;
    printf("%d\n",v);
    v |= 0x01;
    printf("%d\n",v);

    int8_t p = 0;
    p |= 0x80;
    printf("%d\n",p);
    p |= 0x01;
    printf("%d\n",p);

    return 0;
}
