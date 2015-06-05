/*******************************************************************************
*
* FILENAME: handle_message_out.c
*
* Description:
* Implement the functions that deal with the outgoing message.
*
*******************************************************************************/

/*-----------------------------------------------------------------------------
        include files
------------------------------------------------------------------------------*/
#include "handle_message_out.h"


/*------------------------------------------------------------------------------
        definitions (defines, typedefs, ...)
------------------------------------------------------------------------------*/
typedef FLAG DUP;
typedef FLAG RETAIN;
typedef FLAG WILL;
typedef FLAG CLEAN_SESSION;
typedef FLAG TOPIC_TYPE;

extern list_ptr_t  list_out;


#define DEBUG_HANDLE_OUT_MESSAGE_PRINT(...) \
  do{                                                     \
      printf("In %s in function %s at line %d:", __FILE__,__func__,__LINE__);\
      printf(__VA_ARGS__);\
  } while(0)

#define DEBUG_HANDLE_OUT_MESSAGE

/*------------------------------------------------------------------------------
        function prototypes(call back functions)
------------------------------------------------------------------------------*/

int set_dup(FLAG f_in, FLAG *flag);

int set_qos(FLAG f_in, FLAG *flag);

int send_connection(message_info *reply);

int Free_MESSAGE(MESSAGE *message);



extern ERR_NUM err_num;


 /*------------------------------------------------------------------------------
         implementation code
 ------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------*/
int handle_out_message(MESSAGE *message){
/*------------------------------------------------------------------------------*/
    printf("enter the first line of handle out message\n");
  //  if(message == NULL){
//        err_num = MESSAGE_NULL_ERR;
//        ErrHandler();
//        return -1;
//    }

    //if the message is for server, send to java.

    if (message->address == CONTROLLER_ADDRESS){

        send_controller(message);
        return 0;
    }

   message_type m_type = message->type;

   printf("the message type i am going to send out is %d\n",m_type);

    switch (m_type) {
    case CONNACK:
    case WILLTOPICRESP:
    case WILLMSGRESP:
        handle_out_connack(message);
        break;
    case WILLTOPICREQ:
    case WILLMSGREQ:
        handle_out_willtopicreq(message);
        break;
    case REGACK:
        handle_out_regack(message);
        break;
    case PUBLISH:
        handle_out_publish(message);
        break;
    case PUBACK:
        handle_out_puback(message);
        break;
//    case PUBREL_DB:
//       handle_out_pubrel(message);
//        break;
//    case PUBREC_DB:
//        handle_out_pubrec(message);
//        break;
//    case PUBCOMP_DB:
//        handle_out_pubcomp(message);
//        break;
    case SUBACK:
        handle_out_suback(message);
        break;
    case UNSUBACK:
        handle_out_unsuback(message);
        break;
    case PINGRESP:
        handle_out_pingresp(message);
        break;
    default:
        DEBUG_HANDLE_OUT_MESSAGE_PRINT("ERROR in out message type in handle output message.\n\n");
        break;
    }

    return 0;
}

int handle_out_connack(MESSAGE *message){

    printf("I am in handle_out_connack message.\n");

    message_info *reply_message = (message_info *)malloc(size_message_info);
    if(reply_message==NULL){
        err_num = MALLOC_ERR;
        ErrHandler();
        return 1;}

    memset(reply_message, 0, size_message_info);

    reply_message->device_address = message->address;

    void *reply = (void *)malloc(3);
    if(reply==NULL){
        err_num = MALLOC_ERR;
        ErrHandler();
        return 1;}

    memset(reply, 0, 3);

    reply_message->data = reply;

    char *s = reply;
    *s = 0x03;
    s++;
    *s = 0x05;
    s++;
    *s = message->return_value;

    Free_MESSAGE(message);
    printf("I am ready to send out message \n");
    send_connection(reply_message);// to output queue

    return 0;
}

int handle_out_willtopicreq(MESSAGE *message){

    message_info *reply_message = (message_info *)malloc(size_message_info);
    if(reply_message==NULL){
        err_num = MALLOC_ERR;
        ErrHandler();
        return 1;}

    memset(reply_message, 0, size_message_info);

    reply_message->device_address = message->address;

    willtopicreq_packet *reply = (willtopicreq_packet *)malloc(size_willtopicreq_packet);
    if(reply==NULL){
        err_num = MALLOC_ERR;
        ErrHandler();
        return 1;}

    memset(reply, 0, size_willtopicreq_packet);

    reply_message->data = (void*)reply;

    reply->length = 2;
    reply->type = (TYPE) (message->type);

    Free_MESSAGE(message);
    send_connection(reply_message);

//    list_add_out(&list_out, (TIME_PROTOCOL)(message->duration),(DADDRESS)(message->address), message);

    return 0;
}

int handle_out_regack(MESSAGE *message){

    message_info *reply_message = (message_info *)malloc(size_message_info);
    if(reply_message==NULL){
        err_num = MALLOC_ERR;
        ErrHandler();
        return 1;}

    memset(reply_message, 0, size_message_info);
    reply_message->device_address = message->address;

    void *reply = (void *)malloc(7);
    if(reply==NULL){
        err_num = MALLOC_ERR;
        ErrHandler();
        return 1;}

    memset(reply, 0, 7);

    reply_message->data = reply;


    char *s = (char *)reply;
    *s = 0x07;
    s++;
    *s = 0x0b;
    s++;
    uint16_t topic_id = (uint16_t)(message->short_topic_id);
    printf("the topic_id i get from database before i swap %d \n",topic_id);
    topic_id = (topic_id>>8)|(topic_id<<8);
    printf("the topic_id i get from database after i swap %d \n",topic_id);
    uint16_t *temp  = (uint16_t *)s;
    *temp = topic_id;
    printf("the topic_id I put in regack %d \n", *temp);
    s = s+2;
    *s = 0x00;
    s++;
    *s = 0x00;
    s++;
    *s = message->return_value;

    int i;
    s = reply;
    printf("the packet i send out in regack\n");
    for(i = 0; i<7;i++){

        printf("%x\n",*s);
        s++;
    }

//    reply->length = 7;
//    reply->type = (TYPE) (message->type);
//    reply->message_id = message->extern_message_id;
//    reply->return_code = message->return_value;
//    reply->topic_id = message->short_topic_id;

    Free_MESSAGE(message);
    send_connection(reply_message);
    return 0;
}

int handle_out_publish(MESSAGE *message){

    message_info *reply_message = (message_info *)malloc(size_message_info);
    if(reply_message==NULL){
        err_num = MALLOC_ERR;
        ErrHandler();
        return 1;}

    memset(reply_message, 0, size_message_info);

    reply_message->device_address = message->address;

    publish_packet *reply = (publish_packet *)malloc(size_publish_packet);
    if(reply==NULL){
        err_num = MALLOC_ERR;
        ErrHandler();
        return 1;}

    memset(reply, 0, size_publish_packet);

//    reply_message->data = (void*)reply;

    reply->length = 7+(LENGTH_PROTOCOL)(message->length); //assume the length is not
    reply->message_id = message->extern_message_id;
    reply->topic_id = message->short_topic_id;
    reply->type = (TYPE) (message->type);

    if(message->message == NULL){
        err_num = MESSAGE_NULL_ERR;
        ErrHandler();
        return 1;
    }




//    snprintf((reply->p_data),message->length,(char*)(message->message));
    set_qos((FLAG)(message->qos),&(reply->flags));
    set_dup((FLAG)(message->duplicate),&(reply->flags));

    char * to_coordinator = (char *)malloc(reply->length);
    if(to_coordinator == NULL){
        err_num = MALLOC_ERR;
        ErrHandler();
        return 1;
    }

    char *s1,*s2, *s3;
    int i;

    s1 = (char *)(message->message);

    s2 = (char *)reply_message;

    s3 = to_coordinator;

    for(i = 0; i<7; i++){
        *s3 = *s2;
        s3++;
        s2++;
    }
    for(i = 0; i<(message->length);i++){
        *s3 = *s1;
        s3++;
        s1++;
    }

    free(reply);

    reply_message->data = (void *)to_coordinator;

    send_connection(reply_message);

    if(message->qos){
        list_add_out(&list_out, (TIME_PROTOCOL)(message->duration),(DADDRESS)(message->address), message);

    }

     Free_MESSAGE(message);
     return 0;
}

int handle_out_puback(MESSAGE *message){

    message_info *reply_message = (message_info *)malloc(size_message_info);
    if(reply_message==NULL){
        err_num = MALLOC_ERR;
        ErrHandler();
        return 1;}

    memset(reply_message, 0, size_message_info);

    reply_message->device_address = message->address;

    puback_packet *reply = (puback_packet *)malloc(size_puback_packet);
    if(reply==NULL){
        err_num = MALLOC_ERR;
        ErrHandler();
        return 1;}

    memset(reply, 0, size_puback_packet);
    reply_message->data = (void*)reply;

    reply->length = 7;
    reply->message_id = message->extern_message_id;
    reply->return_code = message->return_value;
    reply->topic_id = message->short_topic_id;
    reply->type = (TYPE) (message->type);

    Free_MESSAGE(message);
    send_connection(reply_message);
    return 0;
}

int handle_out_suback(MESSAGE *message){
    message_info *reply_message = (message_info *)malloc(size_message_info);
    if(reply_message==NULL){
        err_num = MALLOC_ERR;
        ErrHandler();
        return 1;}

    memset(reply_message, 0, size_message_info);

    reply_message->device_address = message->address;

    suback_packet *reply = (suback_packet *)malloc(size_suback_packet);
    if(reply==NULL){
        err_num = MALLOC_ERR;
        ErrHandler();
        return 1;}

    memset(reply, 0, size_suback_packet);

    reply_message->data = (void*)reply;

    reply->length = 8;
    reply->message_id = message->extern_message_id;
    reply->return_code = message->return_value;
    reply->topic_id = message->short_topic_id;
    reply->type = (TYPE) (message->type);
  //  set_qos((FLAG)(message->qos),&(reply->flags));

    Free_MESSAGE(message);
    send_connection(reply_message);
    return 0;
}

int handle_out_unsuback(MESSAGE *message){
    message_info *reply_message = (message_info *)malloc(size_message_info);
    if(reply_message==NULL){
        err_num = MALLOC_ERR;
        ErrHandler();
        return 1;}

    memset(reply_message, 0, size_message_info);

    reply_message->device_address = message->address;

    unsuback_packet *reply = (unsuback_packet *)malloc(size_unsuback_packet);
    if(reply==NULL){
        err_num = MALLOC_ERR;
        ErrHandler();
        return 1;}

    memset(reply, 0, size_unsuback_packet);

    reply_message->data = (void*)reply;

    reply->length = 4;
    reply->message_id = message->extern_message_id;
    reply->type = (TYPE) (message->type);

    Free_MESSAGE(message);
    send_connection(reply_message);
    return 0;
}

int handle_out_pingresp(MESSAGE *message){
    message_info *reply_message = (message_info *)malloc(size_message_info);
    if(reply_message==NULL){
        err_num = MALLOC_ERR;
        ErrHandler();
        return 1;}

    memset(reply_message, 0, size_message_info);

    reply_message->device_address = message->address;

    pingresp_packet *reply = (pingresp_packet *)malloc(size_pingresp_packet);
    if(reply==NULL){
        err_num = MALLOC_ERR;
        ErrHandler();
        return 1;}

    memset(reply, 0, size_pingresp_packet);

    reply_message->data = (void*)reply;

    reply->length = 2;
    reply->type = (TYPE)(message->type);

    Free_MESSAGE(message);
    send_connection(reply_message);
    return 0;
}

/*------------------------------------------------------------------------------*/
int set_dup(FLAG f_in, FLAG *flag){
/*------------------------------------------------------------------------------*/
    if(f_in){
        *flag = (*flag)|0x80;
    }
    return 0;
}

/*------------------------------------------------------------------------------*/
int set_qos(FLAG f_in, FLAG *flag){
/*------------------------------------------------------------------------------*/
    int qos = (int)f_in;
    switch(qos){
    case 0:
        *flag = (*flag)|0x00;
        break;
    case 1:
        *flag = (*flag)|0x20;
        break;
    case 2:
        *flag = (*flag)|0x60;
        break;
     default:
        DEBUG_HANDLE_OUT_MESSAGE("ERROR IN QOS LEVEL");
    }
    return 0;
}


/*------------------------------------------------------------------------------*/
int Free_MESSAGE(MESSAGE *message){
/*------------------------------------------------------------------------------*/
//free the memory of type MESSAGE.
    if(message->length){
        if(message->message == NULL){
            err_num = MESSAGE_NULL_ERR;
            ErrHandler();
        }
        else{
    free((void*) message->message);
        }
        }
    free(message);
    message = NULL;
    return 0;
}


