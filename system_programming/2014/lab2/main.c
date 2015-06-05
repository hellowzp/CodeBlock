#include <stdio.h> 
#include <stdlib.h>
#include <math.h>

//#define DEBUG
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
	scanf("%12p",&ptr_start);
	printf("Enter the bytes of the address(can be negative or positive): ");
	scanf("%10d",&size);
	#else 
	ptr_start = (Byte*)&a;
	size = -13;
	//printf("%p %d\n",ptr_start,size);
	#endif
	
	printf("Address\t      Bytes\t\t\t\tChars\n"); //print the first row
	int i,j, rows = (int)ceil(fabs((double)size)/(double)BYTES_PER_ROW);        
    int inc = (size>0)?1:-1, length = (int)fabs((double)size);
    //printf("%d %d",rows,inc);
    
    for(i=0;i<rows;i++) {
      printf("%p    ",ptr_start + i*BYTES_PER_ROW*inc );
      
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
      printf("    ");
      
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
    
	return 0;
}

