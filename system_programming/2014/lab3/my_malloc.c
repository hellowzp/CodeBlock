#include <stdio.h>
#include <stdlib.h>
#include "my_malloc.h"

#define HEADER_SIZE 16   //fixed size of 16 byte
#define INIT_TOTAL_SIZE 500

#define DEBUG

typedef unsigned int Uint;
typedef unsigned char Byte;

struct header {
	Uint msize;
	Byte isAllocated;
	struct header* previous;
	struct header* next;
};
typedef struct header Header;


Header *ptr_FirstHeader = NULL;
Uint lastAvailableMemory, nrOfChunk=0;
Byte a[INIT_TOTAL_SIZE];

void my_init() {
	
	printf("Starting memoey address: %p %p\n",&my_init,a);
	ptr_FirstHeader = (Header*)a;
    ptr_FirstHeader->msize = INIT_TOTAL_SIZE - HEADER_SIZE;
    ptr_FirstHeader->isAllocated = 'F';
    ptr_FirstHeader->previous = NULL;
    ptr_FirstHeader->next = NULL;  
    lastAvailableMemory = ptr_FirstHeader->msize;
    nrOfChunk = 1;
    printf("init finished: chunk=%d size=%d\n",nrOfChunk,lastAvailableMemory);
    
    
	#ifndef DEBUG   //just for pointer test
	int i;
	for(i=0;i<30;i++)
	    printf("%d",*(ptr_FirstHeader+i));
	Header hd;
	printf("Size of header: %d\n",sizeof(hd));   
	ptr_FirstHeader ++ ;  
	printf("Starting memoey address: %p\n",ptr_FirstHeader);
	#endif
}   

void* my_malloc(size tsize) {
	Header *ptr_CurrentHeader = ptr_FirstHeader;
	int i;
	for(i=1;i<=nrOfChunk;i++) {
		if(ptr_CurrentHeader->isAllocated=='F'&&ptr_CurrentHeader->msize>=tsize) {
			ptr_CurrentHeader->isAllocated = 'A';
			ptr_CurrentHeader->msize = tsize;
			break;
		}else if(ptr_CurrentHeader->next) {
			ptr_CurrentHeader = ptr_CurrentHeader->next;
		}		
	}
	
	if(i>nrOfChunk) {
		printf("Unable to allocate this memory size.. return NULL\n");
		return NULL;
	} else {
		if(i==nrOfChunk) {
			lastAvailableMemory = lastAvailableMemory-HEADER_SIZE-tsize;
		    ptr_CurrentHeader->next = (Header*)((int)ptr_CurrentHeader + tsize + HEADER_SIZE);
		    ptr_CurrentHeader->next->msize = lastAvailableMemory;
		    ptr_CurrentHeader->next->isAllocated = 'F';
		    ptr_CurrentHeader->next->previous = ptr_CurrentHeader;
		    ptr_CurrentHeader->next->next = NULL;
	    }	
	    nrOfChunk++;
		printf("New memory allocated from %p to %p\n",
		         (Header*)((int)ptr_CurrentHeader + HEADER_SIZE), ptr_CurrentHeader->next);
	    return ++ptr_CurrentHeader;
	}
}


void my_free(void* ptr) {
	Header* h = (Header*)ptr;
	Header *ptr_CurrentHeader = ptr_FirstHeader;
	while(ptr_CurrentHeader) {
		printf("current address:%p\n",ptr_CurrentHeader);
		printf("%d %d\n",(int)ptr_CurrentHeader + ptr_CurrentHeader->msize,(int)h);
		if((int)ptr_CurrentHeader + ptr_CurrentHeader->msize >= (int)h
		          &&ptr_CurrentHeader->isAllocated=='A') {
		    Header *pre = NULL, *nex = NULL;
		    if(ptr_CurrentHeader->previous) 
		        pre = ptr_CurrentHeader->previous;
		    if(ptr_CurrentHeader->next)
		        nex = ptr_CurrentHeader->next;
		    printf("%p %p\n",pre,nex);
		    
		    if(pre&&nex) {
				while(nex) {
				  if(nex->isAllocated=='F') {
				    nex = nex->next;
				    nrOfChunk--;
				  }else break;
		        }
		        printf("chunk:%d\n",nrOfChunk);
		        while(pre) {
				  if(pre->isAllocated=='F') {
				    pre = pre->previous;
				    nrOfChunk--;
				  }else break;
		        }
		        printf("chunk:%d\n",nrOfChunk);
		        
		        pre->next->msize = (int)nex-(int)pre->next-HEADER_SIZE;
		        pre->next->isAllocated = 'F';
		        pre->next->next = nex;
		        
		    }else if(pre&&!nex) {
				while(pre) {
				  if(pre->isAllocated=='F') {
				    pre = pre->previous;
				    nrOfChunk--;
				  }else break;
		        }
		        printf("chunk:%d\n",nrOfChunk);		        
				pre->next->msize = (int)ptr_CurrentHeader + ptr_CurrentHeader->msize - pre->next->msize;
				pre->next->isAllocated = 'F';
		        pre->next->next = nex;
		    }else if(!pre&&nex) {
				while(nex) {
				  if(nex->isAllocated=='F') {
				    nex = nex->next;
				    nrOfChunk--;
				  }else break;
		        }
		        printf("chunk:%d\n",nrOfChunk);
				ptr_CurrentHeader->msize = (int)nex - (int)ptr_CurrentHeader - HEADER_SIZE;
				ptr_CurrentHeader->isAllocated = 'F';
		        ptr_CurrentHeader->next = nex;
		    }else {
				ptr_CurrentHeader->isAllocated = 'F';
		    }
		    printf("free memory succeed..");
		    break;
		 }else{
			ptr_CurrentHeader = ptr_CurrentHeader->next;
		 }
     }			  
}


int main() {
	printf("Starting memoey address of main function: %p\n",&main);
	my_init();
    printf("Starting address of allocated memory: %p\n",ptr_FirstHeader);
    printf("%p\n",my_malloc(300));
    printf("%p\n",my_malloc(300));
    
    my_init();
    void* ptr[] = {NULL,NULL,NULL};
    my_malloc(600);
    my_malloc(200);
    my_init();
    ptr[0] = my_malloc(400);   
    my_free(ptr[0]);
    ptr[0] = my_malloc(200);
    ptr[1] = my_malloc(100);
    ptr[2] = my_malloc(100);
    my_free(ptr[1]);
    my_free(ptr[2]);
    my_malloc(250);
    
	return 0;
}
