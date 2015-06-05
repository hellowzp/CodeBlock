#ifndef HANDLE_MESSAGE_IN_H
#define HANDLE_MESSAGE_IN_H

/*******************************************************************************
*
* FILENAME: handle_message_in.h
*
* Description:
* Implements the functions that deal with the incoming message.
*
*******************************************************************************/

/*-----------------------------------------------------------------------------
        include files
------------------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>
#include "mqtt_sn_bridge.h"
#include "../mqtt_sn_db/mqtt_sn_db.h"
#include "timeout.h"
#include "interface.h"
#include "err_handler.h"

/*------------------------------------------------------------------------------
        definitions (defines, typedefs, ...)
------------------------------------------------------------------------------*/
extern ERR_NUM err_num;

/*------------------------------------------------------------------------------
        function prototypes
------------------------------------------------------------------------------*/

/*
* upon receiving a message, this method will deal with the raw incoming info and
* find out the message time. Then call the corresponding methods to deal with the
* messages respectively.
*
* Parameters:
*
* message:the pointer point to the infomation of the incoming message which contain device_address and
* the pointer pointing to the package.
*
* return value: 0 if handle successfully, 1 otherwise.
*/

int handle_in_message(message_info *message);

/*
* response to search gate way message type. reply with gwinfo message.
*
* Parameters:
*
* message:the pointer point to the infomation of the incoming message which is a structure containing the
* device_address, length of the real data in bytes and the pointer pointing to the
* real data.
*
* return value: 0 if handle successfully, 1 otherwise.
*/

int handle_searchgw(message_info *message);

/*
* response to pingreq message type. reply with pingack message. update the timer
* if the device is in active mode. send the reserved message to device if the
* device is in asleep mode.
*
* Parameters:
*
* message:the pointer point to the infomation of the incoming message which is a structure containing the
* device_address, length of the real data in bytes and the pointer pointing to the
* real data.
*
* return value: 0 if handle successfully, 1 otherwise.
*/

int handle_pingreq(message_info *message);

/*
* response to register message type. reply with regack message. if the device
* hasn't registered before, generate new topic id for it and save the info to
* database. then update the timer.
*
*
* Parameters:
*
* message:the pointer point to the infomation of the incoming message which is a structure containing
* the device_address, length of the real data in bytes and the pointer pointing
* to the real data.
*
* return value: 0 if handle successfully, 1 otherwise.
*/

int handle_register(message_info *message);

/*
* response to subscribe message type. save the subscribe info in database. send
* the last retained message of the subscribed topic to the device if there is
* such retained message. reply with suback message. update the timer.
*
* Parameters:
*
* message:the pointer point to the infomation of the incoming message which is a structure containing
* the device_address, length of the real data in bytes and the pointer pointing
* to the real data.
*
* return value: 0 if handle successfully, 1 otherwise.
*/

int handle_subscribe(message_info *message);

/*
* response to unsubscribe message type. reply with unsuback message. save the
* unsubscribe info in database. then update the timer.
*
*
* Parameters:
*
* message:the pointer point to the infomation of the incoming message which is a structure containing
* the device_address, length of the real data in bytes and the pointer pointing
* to the real data.
*
* return value: 0 if handle successfully, 1 otherwise.
*/

int handle_unsubscribe(message_info *message);

/*
* response to willtopicupd message type. reply with willtopicresp message. update
* the willtopic info in database. then update the timer.
*
*
* Parameters:
*
* message:the pointer point to the infomation of the incoming message which is a structure containing
* the device_address, length of the real data in bytes and the pointer pointing
* to the real data.
*
* return value: 0 if handle successfully, 1 otherwise.
*/

int handle_willtopicUPD(message_info *message);


/*
* response to willmessageupd message type. reply with willmessageresp message. update
* the willmessage info in database. then update the timer.
*
*
* Parameters:
*
* message:the pointer point to the infomation of the incoming message which is a structure containing
* the device_address, length of the real data in bytes and the pointer pointing
* to the real data.
*
* return value: 0 if handle successfully, 1 otherwise.
*/

int handle_willmessageUPD(message_info *message);

/*
* response to publish message type. reply with puback message if the qos level is higher
* then 0. take out the info in the package. send them to any interested parties if qos
* level is 0. save them in database and send them to any interested parties if qos level
* is 1. if qos level is 2, save them in database and send pubrec. waiting for pubrel and
* then send them to any interested parties as well as send pubcomp to the device. then
* update the timer.
*
* Parameters:
*
* message:the pointer point to the infomation of the incoming message which is a structure containing
* the device_address, length of the real data in bytes and the pointer pointing
* to the real data.
*
* return value: 0 if handle successfully, 1 otherwise.
*/

int handle_publish(message_info *message);

/*
* response to disconnect message type. if the disconnect package contain duration, set the
* device to sleep mode and update the timer and set the client id as asleep in database. if
* there is no duration field and the cleansession flag is set during connect, delete the time
* info in the hashmap and all the related data in data base. if there is no duration field and
* the cleansession flag is not set during connect, delete the time info in the hashmap and set
* the client id as disconnect in data base. keep all the related data in the database for this
* device.
*
*
* Parameters:
*
* message:the pointer point to the infomation of the incoming message which is a structure containing
* the device_address, length of the real data in bytes and the pointer pointing
* to the real data.
*
* return value: 0 if handle successfully, 1 otherwise.
*/

int handle_disconnect(message_info *message);

/*
* response to connect message type. get protocol id in the package and check whether the protocol
* is supported. if not, send connack with return code reject. get client id in the packet. check
* whether there already exists such client id before. if not exists, create new tables neccessary
* in database. get flag value in the package and deal with the flag value. update timer and send
* connack.
*
* Parameters:
*
* message:the pointer point to the infomation of the incoming message which is a structure containing
* the device_address, length of the real data in bytes and the pointer pointing
* to the real data.
*
* return value: 0 if handle successfully, 1 otherwise.
*/

int handle_connect(message_info *message);

/*
* response to pubrel message type. get message id in the package and reliese the data to interested
* parties. send pubcomp reply.
*
* Parameters:
*
* message:the pointer point to the infomation of the incoming message which is a structure containing
* the device_address, length of the real data in bytes and the pointer pointing
* to the real data.
*
* return value: 0 if handle successfully, 1 otherwise.
*/

int handle_pubrel(message_info *message);

/*
* response to willtopic message type. save will topic in database and reply to the device.
*
* Parameters:
*
* message:the pointer point to the infomation of the incoming message which is a structure containing
* the device_address, length of the real data in bytes and the pointer pointing
* to the real data.
*
* return value: 0 if handle successfully, 1 otherwise.
*/

int handle_willtopic(message_info *message);

/*
* response to willmessage message type. save will message in database and reply to the device.
*
* Parameters:
*
* message:the pointer point to the infomation of the incoming message which is a structure containing
* the device_address, length of the real data in bytes and the pointer pointing
* to the real data.
*
* return value: 0 if handle successfully, 1 otherwise.
*/

int handle_willmessage(message_info *message);

TYPE find_message_type(message_info *message);

uint16_t find_length_of_data(message_info *message);

#endif
