#ifndef MY_MALLOC_H_
#define MY_MALLOC_H_

typedef int size;

/*
** Allocates array of bytes (memory pool) and initializes the memory allocator. 
** If some bytes have been used after calling my_malloc(size), calling to my_init() will result in clearing up the memory pool.
*/
void my_init();

/*
** Requesting for the tsize bytes from memory pool. 
** If the request is possible, the pointer to the first possible address byte (right after its header) in memory pool is returned.
*/
void* my_malloc(size tsize);

/*
** Releasing the bytes in memory pool which was hold by ptr, meaning mykes those bytes available for other uses. 
** Implement also the coalescing behavior.
*/
void my_free(void* ptr);


#endif //MY_MALLOC_H_ 
