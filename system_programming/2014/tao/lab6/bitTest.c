#include<stdio.h>
#include<stdlib.h>

struct bits{
            unsigned char b1:1 b2:1 b3:1 b4:1 b5:1 b6:1 b7:1 b8:1 
}
union myByte{
            unsigned char byte;
            struct bits bit;
};
union myByte by;
by.bit.b1=0;
by.bit.b2=0;
