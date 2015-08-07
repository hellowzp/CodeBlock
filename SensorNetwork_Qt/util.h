#ifndef UTIL_H
#define UTIL_H

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#ifndef DEBUG
#define DEBUG_PRINTF(...) do { fprintf( stderr, __VA_ARGS__); } while(0)
#else
#define DEBUG_PRINTF(msg)
#endif

#define MALLOC(ptr, size) do {  \
    ptr = malloc(size);         \
    if(ptr==NULL) {             \
        fprintf( stderr, "malloc error in function %s\n", __func__); \
        assert(0);              \
    }                           \
} while(0);

#endif // UTIL_H
