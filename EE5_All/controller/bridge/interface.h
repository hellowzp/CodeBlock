#ifndef INTERFACE_H
#define INTERFACE_H

/*******************************************************************************
*
* FILENAME: interface.h
*
* Description:
* Implements the bridge's pipes and FIFOs
*
*******************************************************************************/

/*-----------------------------------------------------------------------------
        include files
------------------------------------------------------------------------------*/

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <pthread.h>
#include "handle_message_in.h"
#include "mqtt_sn_bridge.h"
#include "../mqtt_sn_db/mqtt_sn_db.h"
#include "../connection/coordinator.h"
#include "../connection/mymqtt.h"
#include "err_handler.h"

/*------------------------------------------------------------------------------
        function prototypes
------------------------------------------------------------------------------*/

int send_controller(MESSAGE *m);

MESSAGE *read_controller();

int send_connection(message_info *message);

void read_connection();

int send_db(MESSAGE *mes);

void read_db();

extern ERR_NUM err_num;


#endif
