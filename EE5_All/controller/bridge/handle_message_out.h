#ifndef HANDLE_OUTPUT_MESSAGE_H
#define HANDLE_OUTPUT_MESSAGE_H

/*******************************************************************************
*
* FILENAME: handle_message_out.h
*
* Description:
* Implement the functions that deal with the outgoing message.
*
*******************************************************************************/

/*-----------------------------------------------------------------------------
        include files
------------------------------------------------------------------------------*/
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include "mqtt_sn_bridge.h"
#include "../mqtt_sn_db/mqtt_sn_db.h"
#include "timeout.h"
#include "interface.h"
#include "err_handler.h"

/*------------------------------------------------------------------------------
        definitions (defines, typedefs, ...)
------------------------------------------------------------------------------*/
extern ERR_NUM err_num;

#define size_MESSAGE sizeof(MESSAGE)
/*------------------------------------------------------------------------------
        function prototypes
------------------------------------------------------------------------------*/

int handle_out_message(MESSAGE *message);


int handle_out_connack(MESSAGE *message);


int handle_out_willtopicreq(MESSAGE *message);


int handle_out_regack(MESSAGE *message);


int handle_out_publish(MESSAGE *message);


int handle_out_puback(MESSAGE *message);


int handle_out_suback(MESSAGE *message);


int handle_out_unsuback(MESSAGE *message);


int handle_out_pingresp(MESSAGE *message);


#endif
