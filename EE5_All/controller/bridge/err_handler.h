#ifndef ERR_HANDLER_H
#define ERR_HANDLER_H

/*******************************************************************************
*
* FILENAME: err_handler.c
*
* Description:
* ... Handles errors
*
*******************************************************************************/

/*-----------------------------------------------------------------------------
        include files
------------------------------------------------------------------------------*/

#include <stdio.h>

/*------------------------------------------------------------------------------
        definitions (defines, typedefs, ...)
------------------------------------------------------------------------------*/

enum ERR {NORMAL = 0, MESSAGE_DB_NULL_ERR ,MESSAGE_INFO_NULL_ERR, DATA_INVALID_ERR, MALLOC_ERR, NO_DEVICE_ERR, UNEXPECTED_ERR,\
          ADD_IN_FAILURE,LIST_EMPTY,LIST_NOT_EXIST,DEVICE_RECONNECT,DEVICE_NOTCONNECT,SASET_ERR ,SETITIMER_ERR,\
          DEVICE_NOTCONNECT_ERR, PUT_IN_ERR ,DATA_NULL_ERR, INVALID_DATA_ERR,MESSAGE_NULL_ERR,CHILD_EXIT_ERR,\
          IO_JAVA_WRITE_ERR, IO_JAVA_READ_ERR,WRITE_DB_ERR,WRITE_COORDINATOR_ERR,READ_DB_ERR,MESSAGE_IN_ERR};

typedef enum ERR ERR_NUM;

/*------------------------------------------------------------------------------
        function prototypes
------------------------------------------------------------------------------*/

void ErrHandler(void);

#endif
