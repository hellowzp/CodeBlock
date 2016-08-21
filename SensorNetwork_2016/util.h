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
    #define DEBUG_PRINT(fmt, args...) do { \
            printf("\n[Debug] %s:%d:%s(): " fmt, \
            __FILE__, __LINE__, __func__, ##args);   \
    } while(0)
#else
    #define DEBUG_PRINT(fmt, args...) do {} while(0)
#endif

#define ERROR_PRINT(fmt, args...) do { \
        fprintf(stderr, "\n[ERROR] %s:%d:%s(): " fmt, \
        __FILE__, __LINE__, __func__, ##args);     \
} while(0)

/**
#ifdef DEBUG
    #define DEBUG_PRINT(fmt, args...)  \
       fprintf(stderr, "%s:%d:%s(): " fmt, __FILE__, __LINE__, __FUNCTION__, ##args)
#else
    #define DEBUG_PRINT(fmt, args...)
#endif
*/

#endif // UTIL_H
