#ifndef _CONFIG_H_
#define _CONFIG_H_

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <inttypes.h>

#define FIFO_NAME 	"logFifo"

typedef uint16_t sensor_id_t;
typedef double sensor_value_t;     
typedef time_t sensor_ts_t;         // UTC timestamp as returned by time() - notice that the size of time_t is different on 32/64 bit machine
typedef uint16_t room_id_t;

typedef struct{
	sensor_id_t id;
	sensor_value_t value;
	sensor_ts_t ts;
} sensor_data_t;
			
struct sbuffer_data{
	sensor_data_t sensor_data;
};

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

#define SYSCALL_ERROR(err) 									\
		do {												\
			if ( (err) == -1 )								\
			{												\
				perror("Error executing syscall");			\
				exit( EXIT_FAILURE );						\
			}												\
		} while(0)
		
#define CHECK_MKFIFO(err) 									\
		do {												\
			if ( (err) == -1 )								\
			{												\
				if ( errno != EEXIST )						\
				{											\
					perror("Error executing mkfifo");		\
					exit( EXIT_FAILURE );					\
				}											\
			}												\
		} while(0)
		
#define FILE_OPEN_ERROR(fp) 								\
		do {												\
			if ( (fp) == NULL )								\
			{												\
				perror("File open failed");					\
				exit( EXIT_FAILURE );						\
			}												\
		} while(0)

#define FILE_CLOSE_ERROR(err) 								\
		do {												\
			if ( (err) == -1 )								\
			{												\
				perror("File close failed");				\
				exit( EXIT_FAILURE );						\
			}												\
		} while(0)

#define ASPRINTF_ERROR(err) 								\
		do {												\
			if ( (err) == -1 )								\
			{												\
				perror("asprintf failed");					\
				exit( EXIT_FAILURE );						\
			}												\
		} while(0)

#define FFLUSH_ERROR(err) 								\
		do {												\
			if ( (err) == EOF )								\
			{												\
				perror("fflush failed");					\
				exit( EXIT_FAILURE );						\
			}												\
		} while(0)

#define PTHREAD_ERROR(err)   \
		do{	\
		if ((err) != 0) 	\
		{\
			perror("pthread_create failed");\
			exit(EXIT_FAILURE);\
		}\
	}while(0)

#define FILE_ERROR(fp,error_msg) 	do { \
					  if ((fp)==NULL) { \
					    printf("%s\n",(error_msg)); \
					    exit(EXIT_FAILURE); \
					  }	\
					} while(0)

#define MALLOC_ERROR(err)   \
		do{	\
		if ((err) == NULL) 	\
		{\
			fprintf(stderr, "\nIn %s - function %s at line %d: failed\n", __FILE__, __func__, __LINE__);\
			perror("malloc failed");\
			exit(EXIT_FAILURE);\
		}\
	}while(0)


#endif /* _CONFIG_H_ */

 
