#include <stdio.h> 
#include <stdlib.h>
#include <math.h>

#define DEBUG
#define BYTES_PER_ROW  10

typedef unsigned char Byte;

int main() {
    int a = 1;
	printf("THe address of the main function is %p %ld\n",&main,(long int)&main);
	printf("Address of first local variable: %p\n\n",&a);
	
	Byte* ptr_start; 
	int size;	
	#ifndef DEBUG
	printf("Enter the start address in hex-notation: ");
	scanf("%x",&ptr_start);
	printf("\nEnter the bytes of the address(can be negative or positive): ");
	scanf("%d",&size);
	#else 
	ptr_start = (Byte*)&a;
	size = -13;
	//printf("%p %d\n",ptr_start,size);
	#endif
	
	printf("Address\t\tBytes\t\t\t\tChars\n"); //print the first row
	int i,j, rows = (int) ceil(fabs((double)size)/(double)BYTES_PER_ROW);        
    int inc = (size>0)?1:-1, length = (int)fabs((double)size);
    //printf("%d %d",rows,inc);
    
    for(i=0;i<rows;i++) {
      printf("%p  ",ptr_start + i*BYTES_PER_ROW*inc );
      
      //print the content in hex-decimal format
      for(j=0;j<BYTES_PER_ROW;j++) {
         int index = i*BYTES_PER_ROW + j;
         Byte* tp = ptr_start + index*inc;  //get the pointer to the current location
        
         //initialize and print the current location
         if(index < length) {
            if(*tp<32||*tp>127) 
               *tp = 46;            //replace unprintable char with period
         }
         else *tp = 32;             //stuff with space                
         printf("%2x ",*tp); 
      }
      printf("  ");
      
      //print the content as charater
      for(j=0;j<BYTES_PER_ROW;j++) {
         int index = i*BYTES_PER_ROW + j;
         Byte* tp = ptr_start + index*inc;  //get the pointer to the current location
        
         //initialize and print the current location
         if(index < length) {
            if(*tp<32||*tp>127) 
               *tp = 46;            //replace unprintable char with period
         }
         else *tp = 32;             //stuff with space                
         printf("%c ",*tp); 
      }
      printf("\n");
    }
    
	
    /*
    for(i=0;i<rows*BYTES_PER_ROW;i++) {
        printf("%p %2x %c\n",ptr_start+i,*(ptr_start+i),*(ptr_start+i));
    }
    for(i=0;i<size;i++) {
	    Byte* tp = ptr_start+i;
        if(*tp<32||*tp>127) 
		*tp = 46;            //replace unprintable char with period
        }
	for(i=size;i<rows*BYTES_PER_ROW;i++) {  //stuff with space
	    Byte* tp = ptr_start+i;
        *tp = 32;
        } 
	for(i=0;i<rows*BYTES_PER_ROW;i++) {
	    printf("%p %2x %c\n",ptr_start+i,*(ptr_start+i),*(ptr_start+i));
    }
	
	
	
	printf("%p %ld %c %d\n",ptr_start,(long int)ptr_start,*(ptr_start+3),*(ptr_start+3));
	Byte* p = 129;
	if(*p>0)
	printf("129>0 is false!!\n");

	int i,inc,rows = 2;//(int) ceil(size/BYTES_PER_ROW);
	inc = (size>0)?1:-1;
	for(i=0;i<size;i++) {
	    if(*(ptr_start+i)<32||*(ptr_start+i)>127) 
		*(ptr_start+i) = 32;
        }
	for(i=size;i<rows*BYTES_PER_ROW;i++) {
	    *(ptr_start+i) = 46;
        }
	for(i=0;i<rows*BYTES_PER_ROW;i++) {
	    printf("%p %c\n",ptr_start+i,*(ptr_start+i));
        }

	
	ptr_start = (Byte*)malloc(2*BYTES_PER_ROW*sizeof(Byte));
	printf("%p %ld %d\n",ptr_start,(long int)ptr_start,size);
        */
	return 0;
}

