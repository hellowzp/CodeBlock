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

#include "err_handler.h"

/*------------------------------------------------------------------------------
        definitions (defines, typedefs, ...)
------------------------------------------------------------------------------*/

ERR_NUM err_num = NORMAL;

/*------------------------------------------------------------------------------
        implementation code
------------------------------------------------------------------------------*/

void ErrHandler(){

    FILE *logfile = fopen("bridge_err_message","a+");
    switch(err_num){
    case NORMAL:
        break;
    case MESSAGE_DB_NULL_ERR:
        fprintf(logfile,"%s\n","the message_db pointer you get is null.");
        break;
    case MESSAGE_INFO_NULL_ERR:
        fprintf(logfile,"%s\n","the message_info pointer you get is null.");
        break;
    case DATA_INVALID_ERR:
        fprintf(logfile,"%s\n","the data in message_info is not valid.");
        break;
    case MALLOC_ERR:
        fprintf(logfile,"%s\n","Error happens when malloc a piece of memory.");
        break;
    case NO_DEVICE_ERR:
        fprintf(logfile,"%s\n","the device is not connected, but you want to get information from it.");
        break;
    case UNEXPECTED_ERR:
        fprintf(logfile,"%s\n","the error nomally shouldn't happen, something wrong with the enviroment.");
        break;
    case ADD_IN_FAILURE:
        fprintf(logfile,"%s\n","you try to add new device in the list_in, but failed.");
        break;
    case LIST_EMPTY:
        fprintf(logfile,"%s\n","Can't execute this operation while the list is empty." );
        break;
    case LIST_NOT_EXIST:
        fprintf(logfile,"%s\n","Can't execute this operation while the list is not exist." );
        break;
    case DEVICE_RECONNECT:
        fprintf(logfile,"%s\n","this device already connected, and you want to reconnect, please check." );
        break;
    case DEVICE_NOTCONNECT:
        fprintf(logfile,"%s\n","this device is not connected but receive message. please check." );
        break;
    case SASET_ERR:
        fprintf(logfile,"%s\n","error happens with function sigaction.");
        break;
    case SETITIMER_ERR:
        fprintf(logfile,"%s\n","error happens when you set itimer.");
        break;
    case DEVICE_NOTCONNECT_ERR:
        fprintf(logfile,"%s\n","the device is not in the list but you want to updata it.");
        break;
    case PUT_IN_ERR:
        fprintf(logfile,"%s\n","trying to put new device in list_in is not successful.");
        break;
    case DATA_NULL_ERR:
        fprintf(logfile,"%s\n","the data in the message_info is not valid.");
        break;
    case INVALID_DATA_ERR:
        fprintf(logfile,"%s\n","Either message_info or data is not valid.");
        break;
    case MESSAGE_NULL_ERR:
        fprintf(logfile,"%s\n","the message in the message_db is not valid.");
        break;
    case CHILD_EXIT_ERR:
        fprintf(logfile,"%s\n","receive unexpected exiting child pid.");
        break;
    case IO_JAVA_WRITE_ERR:
        fprintf(logfile,"%s\n","I/O error while sending data from bridge to Java through FIFO.");
        break;
    case IO_JAVA_READ_ERR:
        fprintf(logfile,"%s\n","I/O error while receiving data from Java to bridge through FIFO.");
        break;
    case WRITE_DB_ERR:
        fprintf(logfile,"%s\n","error when write to database.");
        break;
    case WRITE_COORDINATOR_ERR:
        fprintf(logfile,"%s\n","error when write to coordinator.");
        break;
    case READ_DB_ERR:
        fprintf(logfile,"%s\n","error when read from database.");
        break;
    case MESSAGE_IN_ERR:
        fprintf(logfile,"%s\n","error: the message type for incoming message is not valid.");
        break;
    default:
        fprintf(logfile,"%s\n","something strange happens in this system.");
        break;
    }
    fclose(logfile);
}
