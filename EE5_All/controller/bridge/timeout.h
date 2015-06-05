#ifndef TIMEOUT_H
#define TIMEOUT_H

/*******************************************************************************
*
* FILENAME: timeout.h
*
* Description:
* 
*
*******************************************************************************/

/*-----------------------------------------------------------------------------
		include files
------------------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <pthread.h>
#include <sys/time.h>
#include "interface.h"
#include "sorted_double_linked_list/sorted_double_linked_list.h"
#include "err_handler.h"

/*------------------------------------------------------------------------------
		definitions (defines, typedefs, ...)
------------------------------------------------------------------------------*/

extern ERR_NUM err_num;

/*------------------------------------------------------------------------------
		function prototypes
------------------------------------------------------------------------------*/


/*
* initialize linkedlist.
*
* Parameters:
*
* none
*
* return value: the pointer point to the start of the list.
*/

list_ptr_t list_initialization(void);

/*
* destroy the list.
*
* Parameters:
*
* **list: the address of the pointer pointing to the start of the list.
*
* return value: none.
*/
void list_destroy(list_ptr_t *list);

/*
* when the device connect, we add the related infomation to the list.
*
* Parameters:
* 
* duration: the time interval for the device to keep alive or the sleep duration.
*
* address: the address of the connecting device.
*
* return value: 1 if add to the map successfully, 0 otherwise.
*/

int list_add_in(list_ptr_t *list, TIME_PROTOCOL duration,  DADDRESS address);

int list_add_out(list_ptr_t *list, TIME_PROTOCOL duration,  DADDRESS address, MESSAGE *message);

/*
* when the device disconnect, delete the element in the list.
* 
* Parameter:
*
* address:the address of the disconnecting device.
* 
*return value: 1 if set the state successfully, 0 otherwise.
*/

int list_delete_device_timeout(list_ptr_t *list);

/*
* when a message coming, we update the timeout value in the list
*
* 
* Parameter:
*
* address:the address of the device which send the incoming message.
* 
*return value: 1 if update successfully, 0 otherwise.
*/

int list_update_device(list_ptr_t *list,DADDRESS address);

int list_delete_device(list_ptr_t *list,DADDRESS address);

int list_device_sleep(list_ptr_t list, DADDRESS address, TIME_PROTOCOL duration);

void init_timer1(void);
void init_timer2(void);

#endif







