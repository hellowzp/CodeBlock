#ifndef MQTT_SN_DB_H_
#define MQTT_SN_DB_H_

#define _GNU_SOURCE /* See feature_test_macros(7) */

#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <sqlite3.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/types.h>
#include "parser/parser.h"
#include "prioritized_list/prioritized_list.h"

/* IMPORTANT

--	warning: sqlite3 will bind parameters starting with parameter 1, 
	returning a result and getting the right column will start form 0.

--	important input rule: incomming connect messages must have the lowest priority.
	in that way chanses are as low as posible that the address is reused and the messages
	from the old owner come in and look as if they are from the new owner.

--	important timers: the database will not contain any timers. The user should implement
	his own timers.
	*	for qos 1 and 2: if you send or receive a message, store it IN DATABASE FORMAT and start
  		a timer, if the time expires, give the message to the database and tell it is expired.
	*	for sleep of a device: if you get a disconnect with sleep, start the sleep timer and remember
		address. Reset this timer if needed. If the timer expires, calll the disconnect function with
		the flag unexpected.

--	important note for end nodes: If you request a disconnect (or sleep) or unsubscribe, no new
	messages will be send to you. Old messages that are not finished yet will be send. If you choose
	to ignore them, you will lose them. If QoS is 1 or 2 and a timeout takes place, they will be set
	as being not acknowledged and will wait until you reconnect. You will not loose them. Please handle
	all incomming messages first until completed before going to sleep.

--	auto-disconnect disconnect a device using the address requested without an error

// TODO always close a prepared statement before a return
// TODO when exiting the program unexpected, first call the close function
*/

#define AUTO_DISCONNECT
#define TIMEOUT 600
#define ERROR_TOPIC_INVALID_INPUT "error/invalid_input"
#define ERROR_TOPIC_INTERNAL_ERRORS "error/internal_error"
#define ERROR_FILE "mqtt_sn_db_error_file.txt"
#define REQUEST_TOPIC "request"
#define RESPONSE_TOPIC "result"

//include the header late on

typedef uint64_t ADDRESS ;
typedef uint8_t FLAG ;
typedef int8_t QOS ;
typedef uint32_t DATABASE_ID ;
typedef uint32_t LENGTH ;
typedef uint16_t SLEEP ;
typedef uint8_t TOPIC_TYPE ;
typedef uint16_t EXTERN_ID ;
typedef uint8_t STATE ;
typedef uint16_t TOPIC_ID ;
typedef char* DATABASE ;
typedef char* CLIENT_ID;
typedef time_t TIME;
typedef uint32_t SOFTWARE_VERSION;
typedef char* TOPIC_NAME;
typedef uint8_t MESSAGE_TYPE;

/**
 * enums
 **/
 
typedef enum Extern_error{
	DATABASE_OK = 0,
	DATABASE_INITIALIZATION_ERROR,
	DATABASE_INVALID_INPUT,
	DATABASE_SEMAPHORE_ERROR
} extern_error;
 


typedef enum Message_type{
	ADVERTISE = 0,
	SEARCHGW,
	GWINFO,
	RESERVED_3,
	CONNECT,
	CONNACK,
	WILLTOPICREQ,
	WILLTOPIC,
	WILLMSGREQ,
	WILLMSG,
	REGISTER,
	REGACK,
	PUBLISH,
	PUBACK,
	PUBCOMP,
	PUBREC,
	PUBREL,
	RESERVED_17,
	SUBSCRIBE,
	SUBACK,
	UNSUBSCRIBE,
	UNSUBACK,
	PINGREQ,
	PINGRESP,
	DISCONNECT,
	RESERVED_25,
	WILLTOPICUPD,
	WILLTOPICRESP,
	WILLMSGUPD,
	WILLMSGRESP
} message_type;

typedef enum Return_code{
	ACCEPTED,
	CONGESTION,
	INVALID_TOPIC_ID,
	NOT_SUPPORTED
} return_code;

enum MESSAGE_STATE{
    ACK = 0,
    REC,
    REL,
    COMP,
    CANCELLED,
};

/* defines of error codes, only here for quick reference (28-4-2014) */

// SQLITE_OK           0   /* Successful result */

/* beginning-of-error-codes */

// SQLITE_ERROR        1   /* SQL error or missing database */
// SQLITE_INTERNAL     2   /* Internal logic error in SQLite */
// SQLITE_PERM         3   /* Access permission denied */
// SQLITE_ABORT        4   /* Callback routine requested an abort */
// SQLITE_BUSY         5   /* The database file is locked */
// SQLITE_LOCKED       6   /* A table in the database is locked */
// SQLITE_NOMEM        7   /* A malloc() failed */
// SQLITE_READONLY     8   /* Attempt to write a readonly database */
// SQLITE_INTERRUPT    9   /* Operation terminated by sqlite3_interrupt()*/
// SQLITE_IOERR       10   /* Some kind of disk I/O error occurred */
// SQLITE_CORRUPT     11   /* The database disk image is malformed */
// SQLITE_NOTFOUND    12   /* Unknown opcode in sqlite3_file_control() */
// SQLITE_FULL        13   /* Insertion failed because database is full */
// SQLITE_CANTOPEN    14   /* Unable to open the database file */
// SQLITE_PROTOCOL    15   /* Database lock protocol error */
// SQLITE_EMPTY       16   /* Database is empty */
// SQLITE_SCHEMA      17   /* The database schema changed */
// SQLITE_TOOBIG      18   /* String or BLOB exceeds size limit */
// SQLITE_CONSTRAINT  19   /* Abort due to constraint violation */
// SQLITE_MISMATCH    20   /* Data type mismatch */
// SQLITE_MISUSE      21   /* Library used incorrectly */
// SQLITE_NOLFS       22   /* Uses OS features not supported on host */
// SQLITE_AUTH        23   /* Authorization denied */
// SQLITE_FORMAT      24   /* Auxiliary database format error */
// SQLITE_RANGE       25   /* 2nd parameter to sqlite3_bind out of range */
// SQLITE_NOTADB      26   /* File opened that is not a database file */
// SQLITE_NOTICE      27   /* Notifications from sqlite3_log() */
// SQLITE_WARNING     28   /* Warnings from sqlite3_log() */
// SQLITE_ROW         100  /* sqlite3_step() has another row ready */
// SQLITE_DONE        101  /* sqlite3_step() has finished executing */

/* end-of-error-codes */

/**
 * define the data structures
 **/
//TODO change the function 
typedef struct Message{
	ADDRESS address;
	DATABASE_ID message_id;
	EXTERN_ID extern_message_id;
	EXTERN_ID short_topic_id;
	MESSAGE_TYPE type;
	TOPIC_TYPE topic_type;
	QOS qos;
	time_t duration;
	uint32_t connection_id;
	FLAG duplicate;
	FLAG will;
	FLAG clean;
	FLAG retain;
	return_code return_value;
	LENGTH length;
	const void* message;
 } MESSAGE;

typedef struct Topic{
	TOPIC_TYPE topic_type;
	LENGTH topic_length;
	void* topic_name;
 } topic;

typedef struct Blob{
	const void* data;
	uint32_t size;
}blob;
 

	

/*********************************************************************************
**		prototypes extern header file
**********************************************************************************/

extern_error database_open(char* database_name, int* child_pid, char* output_file, int* file_descriptor_write, int* file_descriptor_read);
extern_error database_request_message( void );

#endif
