#ifndef UTIL_H
#define UTIL_H

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

/**
there should be at most one preprocessor in one line
#define DEBUG_PRINT(string) \
    do{ \
        #ifdef DEBUG \
        printf(string); \
        #endif } \
    while(0)
*/

#ifdef DEBUG
//    #define DEBUG_PRINTF(...) do { fprintf( stderr, __VA_ARGS__); } while(0)
    #define DEBUG_PRINTF(fmt, args...) do { \
                fprintf(stderr, "\n[Debug_Info] %s:%d:%s(): " fmt, \
                __FILE__, __LINE__, __func__, ##args);     \
    } while(0)
#else
    #define DEBUG_PRINTF(fmt, args...) do {} while(0)
#endif

/**
#ifdef DEBUG
    #define DEBUG_PRINT(fmt, args...)  \
       fprintf(stderr, "%s:%d:%s(): " fmt, __FILE__, __LINE__, __FUNCTION__, ##args)
#else
    #define DEBUG_PRINT(fmt, args...)
#endif

*/

#define MALLOC(ptr, size) do {  \
    ptr = malloc(size);         \
    if(ptr==NULL) {             \
        fprintf( stderr, "malloc error in function %s\n", __func__); \
        assert(0);              \
    }                           \
} while(0);



#endif // UTIL_H
