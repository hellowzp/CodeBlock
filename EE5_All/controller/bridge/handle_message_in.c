/*******************************************************************************
*
* FILENAME: handle_message_in.c
*
* Description:
* Implements the functions that deal with the incoming message.
*
*******************************************************************************/

/*-----------------------------------------------------------------------------
        include files
------------------------------------------------------------------------------*/
#include "handle_message_in.h"

/*------------------------------------------------------------------------------
        definitions (defines, typedefs, ...)
------------------------------------------------------------------------------*/
typedef FLAG DUP;
typedef FLAG RETAIN;
typedef FLAG WILL;
typedef FLAG CLEAN_SESSION;
typedef FLAG TOPIC_TYPE;

extern list_ptr_t list_in;
extern list_ptr_t list_out;


#define size_MESSAGE sizeof(MESSAGE)

#define DEBUG_HANDLE_INCOMING_MESSAGE_PRINT(...) \
  do{                                                     \
      printf("In %s in function %s at line %d:", __FILE__,__func__,__LINE__);\
      printf(__VA_ARGS__);\
  } while(0)

#define DEBUG_HANDLE_INCOMING_MESSAGE

/*------------------------------------------------------------------------------
        function prototypes(call back functions)
------------------------------------------------------------------------------*/

DUP get_dup(FLAG flag);

QOS get_qos(FLAG flag);

RETAIN get_retain(FLAG flag);

WILL get_will(FLAG flag);

CLEAN_SESSION get_clean_session(FLAG flag);

TOPIC_TYPE get_topic_type(FLAG flag);

uint8_t protocol_support(PROTOCOL_ID protocol_id_check);

int Free_message_info(message_info *message);

uint8_t find_length_of_lengthField(message_info *message);

DATA find_meaningful_data(message_info *message);





/*------------------------------------------------------------------------------
        implementation code
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------*/
int handle_in_message(message_info *message){
/*------------------------------------------------------------------------------*/

    DEBUG_HANDLE_INCOMING_MESSAGE_PRINT("in function handle_in_message.\n");
    TYPE type = find_message_type(message);

    DEBUG_HANDLE_INCOMING_MESSAGE_PRINT("THE TYPE IN FUNCTION HANDLE_IN_MESSAGE %d.\n",(int)type);

    if(type == (TYPE)-1){
        err_num = INVALID_DATA_ERR;
        ErrHandler();
        return 1;
    }
//if((message->device_address) == (list_out->data->address)){
//    if(((list_out->data->message->type == WILLTOPICREQ)&&(type == WILLTOPIC))||\
//            ((list_out->data->message->type == WILLMSGREQ)&&(type == WILLMSG))||\
//             ((list_out->data->message->type == PUBLISH)&&(type == PUBACK))){
//        list_delete_device_timeout(&list_out);
//    }
//}
    switch (type) {
    case SEARCHGW:
        handle_searchgw(message);
        break;
    case PINGREQ:
        handle_pingreq(message);
        break;
    case REGISTER:
        handle_register(message);
        break;
    case SUBSCRIBE:
        handle_subscribe(message);
        break;
    case UNSUBSCRIBE:
        handle_unsubscribe(message);
        break;
    case WILLTOPICUPD:
        handle_willtopicUPD(message);
        break;
    case WILLMSGUPD:
        handle_willmessageUPD(message);
        break;
    case PUBLISH:
        handle_publish(message);
        break;
    case PUBREL:
       handle_pubrel(message);
        break;
    case DISCONNECT:
        handle_disconnect(message);
        break;
    case CONNECT:
        handle_connect(message);
        break;
    case WILLTOPIC:
        handle_willtopic(message);
        break;
    case WILLMSG:
        handle_willmessage(message);
        break;
    default:
        err_num = MESSAGE_IN_ERR;
        ErrHandler();
        DEBUG_HANDLE_INCOMING_MESSAGE_PRINT("ERROR: the message type for incoming message is not valid\n");
        break;
    }
    return 0;
}

/*------------------------------------------------------------------------------*/
int handle_searchgw(message_info *message){
/*------------------------------------------------------------------------------*/

     DEBUG_HANDLE_INCOMING_MESSAGE_PRINT("SEARCH_GW\n");

     //malloc a memory of type message_info to store the reply message.
     message_info *reply_message = (message_info *)malloc(size_message_info);
     if(reply_message==NULL){
         err_num = MALLOC_ERR;
         ErrHandler();
         return 1;}

     memset(reply_message, 0, size_message_info);
     reply_message->device_address = message->device_address;

     //malloc a memory of type gwinfo_packet.
     void *reply = (void *)malloc(11);
     if(reply==NULL){
         err_num = MALLOC_ERR;
         ErrHandler();
         return 1;}

     memset(reply, 0, 11);

     //assign values to the fields of gwinfo_packet.
     char *s = reply;
     *s = 0x0b;
     s++;
     *s = 0x02;
     s++;
     *s = 0x11;
     s++;
     *((DADDRESS *)(s)) = gwAddress;

     reply_message->data = (void *)reply;

     //free incoming message and send the reply.
     Free_message_info(message);
     char *s1 = (void *)reply_message;
     int i;

 //    printf("the size of gwinfo_packet %d\n",size_gwinfo_packet);

     printf("the packet I send out : \n\n");
     for(i = 0;i<(size_message_info);i++){

         printf("%x\n",*s1);
         s1++;
     }
     s1 = (char *)reply;
     printf("the message I send for the second part\n");
     for(i = 0;i<11;i++){
         printf("%x\n",*s1);
         s1++;
     }


     send_connection(reply_message); //store this message in the output queue.
     return 0;
}


/*------------------------------------------------------------------------------*/
int handle_pingreq(message_info *message){
/*------------------------------------------------------------------------------*/

        DEBUG_HANDLE_INCOMING_MESSAGE_PRINT("pringreq\n");

     MESSAGE *mes = (MESSAGE *)malloc(size_MESSAGE);
     if(mes==NULL){
         err_num = MALLOC_ERR;
         ErrHandler();
         return 1;}

     memset(mes, 0, size_MESSAGE);
     mes->address = message->device_address;

       // this message should on priority 2. send the stored message in the database in time.

       uint16_t length = find_length_of_data(message);

       if(length > 2){

           int i;

           length = length - find_length_of_lengthField(message) - 1;
           CLIENT_ID client_id = (char *)malloc(length);
           if(client_id == NULL){
               err_num = MALLOC_ERR;
               ErrHandler();
               return 1;}

           memset(client_id, 0, length);

           char *s1, *s2;

           s1 = (char *)client_id;
           s2 = (char *)find_meaningful_data(message);

           for(i=0;i<length;i++){

               *s1 = *s2;
               s1++;
               s2++;
           }

 //          snprintf(client_id, length, find_meaningful_data(message));
           mes->length = (LENGTH)length;
           mes->message = (void *)client_id;
       }
       else{
           mes->length = 0;
           mes->message = NULL;
       }

       list_ptr_t item;
       list_get_by_address(list_in,message->device_address, &item);

       if(item == NULL){
           err_num = NO_DEVICE_ERR;
           ErrHandler();
           return 1;
       }
       mes->duration = item->data->duration;

       mes->duplicate = 0;
       mes->return_value = 0;
       mes->extern_message_id = -1;
       mes->message_id = 0;
       mes->qos = -2;
       mes->short_topic_id = -1;
       mes->connection_id = 0;
       mes->type = PINGREQ;
       Free_message_info(message);

  //      if(list_update_device(&list_in,(DADDRESS)(mes->address))){
  //          err_num = UNEXPECTED_ERR;
  //          ErrHandler();
  //          return 1;
  //      }
        send_db(mes); //stored in the priority list prepared for database.
        return 0;
}


/*------------------------------------------------------------------------------*/
int handle_register(message_info *message){
/*------------------------------------------------------------------------------*/

    DEBUG_HANDLE_INCOMING_MESSAGE_PRINT("register_\n");

    MESSAGE *mes = (MESSAGE *)malloc(size_MESSAGE);
    if(mes==NULL){
        err_num = MALLOC_ERR;
        ErrHandler();
        return 1;}
    memset(mes, 0, size_MESSAGE);

     mes->address = message->device_address;

     DADDRESS ad = (mes->address);

     char *adress = &ad;
     int i;
     for(i = 0;i<8;i++){
         printf("%x\n",*adress);
         adress++;
     }
     printf("the address of the device in register packet %16x \n",(mes->address));

     uint16_t length = find_length_of_data(message);
     printf("the length of register packege %d \n", (int)(length));
     length = length - find_length_of_lengthField(message) - 5;

     printf("the length of topic name %d \n",length);
     DATA dataIn = find_meaningful_data(message);
    TOPIC_NAME topic_name = (TOPIC_NAME)(dataIn+4);
    char * topic_n = (char *)malloc(length);
    if(topic_n==NULL){
        err_num = MALLOC_ERR;
        ErrHandler();
        return 1;}
    memset(topic_n, 0, length);

   i;

    char *s1, *s2;

    s1 = (char *)topic_n;
    s2 = (char *)topic_name;

    for(i=0;i<length;i++){

        *s1 = *s2;
        s1++;
        s2++;
    }

//    snprintf(topic_n,length,topic_name);
    printf("the topic name I stored in database %s\n",topic_n);
    mes->message = (void *)topic_n;

    mes->length = (LENGTH)length;

    MESSAGE_ID message_id = *((MESSAGE_ID *)(dataIn+2));
    printf("the message_id of register packet %d \n",(int)(message_id));
    mes->extern_message_id = (EXTERN_ID)message_id;

//    list_ptr_t item;
//    list_get_by_address(list_in,message->device_address, &item);

//    if(item == NULL){
//        err_num = NO_DEVICE_ERR;
//        ErrHandler();
//        return 1;
//    }
 //    mes->duration = item->data->duration;

    mes->duplicate = 0;
    mes->message_id = 0;
    mes->return_value = 0;
    mes->qos = 0;
    mes->short_topic_id = 0;
    mes->connection_id = 0;
    mes->type = REGISTER;

    Free_message_info(message);

//    if(list_update_device(&list_in,(DADDRESS)(mes->address))){
 //       err_num = UNEXPECTED_ERR;
 //       ErrHandler();
 //       return 1;
 //   }

    send_db(mes);

     return 0;
}

/*------------------------------------------------------------------------------*/
int handle_subscribe(message_info *message){
/*------------------------------------------------------------------------------*/

    DEBUG_HANDLE_INCOMING_MESSAGE_PRINT("handle_subscribe\n");

    DATA dataIn = find_meaningful_data(message);

    FLAG flag = *((FLAG *)(dataIn));

    QOS qos = get_qos(flag);

    DUP dup = get_dup(flag);

    TOPIC_TYPE topic_type = get_topic_type(flag);

    MESSAGE_ID message_id = *((MESSAGE_ID *)(dataIn+1));

    MESSAGE *mes = (MESSAGE *)malloc(size_MESSAGE);
    if(mes==NULL){
        err_num = MALLOC_ERR;
        ErrHandler();
        return 1;}
    memset(mes, 0, size_MESSAGE);

     mes->address = message->device_address;
     mes->extern_message_id = message_id;

     if(topic_type == 0)
     {
         TOPIC_ID topic_id = *((TOPIC_ID *)(dataIn+3));
         mes->short_topic_id = topic_id;
         mes->length = 0;
         mes->message = NULL;
     }
     else if(topic_type == 1){
         TOPIC_NAME topic_name = (TOPIC_NAME)(dataIn+3);
         uint16_t length = find_length_of_data(message);
         length = length - find_length_of_lengthField(message) - 4;
         mes->length = (LENGTH)length;

         char * topic_n = (char *)malloc(length);
         if(topic_n==NULL){
             err_num = MALLOC_ERR;
             ErrHandler();
             return 1;}
         memset(topic_n, 0, length);

         int i;

         char *s1, *s2;

         s1 = (char *)topic_n;
         s2 = (char *)topic_name;

         for(i=0;i<length;i++){


             *s1 = *s2;
             s1++;
             s2++;
         }

//         snprintf(topic_n,length,topic_name);
         mes->message = (void *)topic_n;
         mes->short_topic_id = -1;
     }

     mes->duplicate = (FLAG)dup;
     mes->message_id = 0;
     mes->qos = qos;
     mes->return_value = 0;
     mes->type = SUBSCRIBE;
     mes->connection_id = 0;

//     list_ptr_t item;
//     list_get_by_address(list_in,message->device_address, &item);

//     if(item == NULL){
//         err_num = NO_DEVICE_ERR;
//         ErrHandler();
//         return 1;
//     }
//     mes->duration = item->data->duration;

    Free_message_info(message);

 //   if(list_update_device(&list_in,(DADDRESS)(mes->address))){
 //       err_num = UNEXPECTED_ERR;
 //       ErrHandler();
 //       return 1;
 //   }

    send_db(mes);
    return 0;
}

/*------------------------------------------------------------------------------*/
int handle_unsubscribe(message_info *message){
/*------------------------------------------------------------------------------*/

    DEBUG_HANDLE_INCOMING_MESSAGE_PRINT("handle_unsubscribe\n");

    DATA dataIn = find_meaningful_data(message);

    FLAG flag = *((FLAG *)(dataIn));
    TOPIC_TYPE topic_type = get_topic_type(flag);

    MESSAGE_ID message_id = *((MESSAGE_ID *)(dataIn+1));

    MESSAGE *mes = (MESSAGE *)malloc(size_MESSAGE);
    if(mes==NULL){
        err_num = MALLOC_ERR;
        ErrHandler();
        return 1;}
    memset(mes, 0, size_MESSAGE);

     mes->address = message->device_address;
     mes->extern_message_id = message_id;

     if(topic_type == 0)
     {
         TOPIC_ID topic_id = *((TOPIC_ID *)(dataIn+3));
         mes->short_topic_id = topic_id;
         mes->length = 0;
         mes->message = NULL;
     }
     else if(topic_type == 1){
         TOPIC_NAME topic_name = (TOPIC_NAME )(dataIn+3);
         uint16_t length = find_length_of_data(message);
         length = length - find_length_of_lengthField(message) - 4;
         mes->length = (LENGTH)length;

         char * topic_n = (char *)malloc(length);
         if(topic_n==NULL){
             err_num = MALLOC_ERR;
             ErrHandler();
             return 1;}
         memset(topic_n, 0, length);

         int i;

         char *s1, *s2;

         s1 = (char *)topic_n;
         s2 = (char *)topic_name;

         for(i=0;i<length;i++){


             *s1 = *s2;
             s1++;
             s2++;
         }

//         snprintf(topic_n,length,topic_name);
         mes->message = (void *)topic_n;
         mes->short_topic_id = -1;
     }

     mes->duplicate = 0;
     mes->message_id = 0;
     mes->qos = -2;
     mes->return_value = 0;
     mes->type = UNSUBSCRIBE;
     mes->connection_id = 0;

//     list_ptr_t item;
//     list_get_by_address(list_in,message->device_address, &item);

//     if(item == NULL){
//         err_num = NO_DEVICE_ERR;
//         ErrHandler();
//         return 1;
//     }
//     mes->duration = item->data->duration;

    Free_message_info(message);

//    if(list_update_device(&list_in,(DADDRESS)(mes->address))){
//        err_num = UNEXPECTED_ERR;
//        ErrHandler();
//        return 1;
 //   }
    send_db(mes);

    return 0;
}

/*------------------------------------------------------------------------------*/
int handle_willtopicUPD(message_info *message){
/*------------------------------------------------------------------------------*/

    DEBUG_HANDLE_INCOMING_MESSAGE_PRINT("handle_willtopicUPD\n");

    MESSAGE *mes = (MESSAGE *)malloc(size_MESSAGE);
    if(mes==NULL){
        err_num = MALLOC_ERR;
        ErrHandler();
        return 1;}
    memset(mes, 0, size_MESSAGE);

     mes->address = message->device_address;

    DATA dataIn = find_meaningful_data(message);

    FLAG flag = *((FLAG *)(dataIn));

    RETAIN retain = get_retain(flag);

    QOS qos = get_qos(flag);

    TOPIC_NAME topic_name = (TOPIC_NAME )(dataIn+1);

    LENGTH_PROTOCOL length = find_length_of_data(message);
    length = length - find_length_of_lengthField(message) - 2;

    mes->length = (LENGTH)length;

    char * topic_n = (char *)malloc(length);
    if(topic_n==NULL){
        err_num = MALLOC_ERR;
        ErrHandler();
        return 1;}
    memset(topic_n, 0, length);

    int i;

     char *s1, *s2;
    s1 = (char *)topic_n;
    s2 = (char *)topic_name;

    for(i=0;i<length;i++){


        *s1 = *s2;
        s1++;
        s2++;
    }

//    snprintf(topic_n,length,topic_name);
    mes->message = (void *)topic_n;

    mes->duplicate = 0;
    mes->message_id = 0;
    mes->extern_message_id = -1;
    mes->short_topic_id = -1;
    mes->qos = qos;
    mes->return_value = 0;
    mes->type = WILLTOPICUPD;
    mes->connection_id = 0;

    //still lack retain.

//    list_ptr_t item;
//    list_get_by_address(list_in,message->device_address, &item);

//    if(item == NULL){
//        err_num = NO_DEVICE_ERR;
//        ErrHandler();
//        return 1;
//    }
//    mes->duration = item->data->duration;

    Free_message_info(message);

//    if(list_update_device(&list_in,(DADDRESS)(mes->address))){
//        err_num = UNEXPECTED_ERR;
//        ErrHandler();
//        return 1;
//    }
    send_db(mes);

    return 0;
}

/*------------------------------------------------------------------------------*/
int handle_willmessageUPD(message_info *message){
/*------------------------------------------------------------------------------*/

    DEBUG_HANDLE_INCOMING_MESSAGE_PRINT("handle_willmessageUPD\n");


    MESSAGE *mes = (MESSAGE *)malloc(size_MESSAGE);
    if(mes==NULL){
        err_num = MALLOC_ERR;
        ErrHandler();
        return 1;}
    memset(mes, 0, size_MESSAGE);

     mes->address = message->device_address;

    DATA dataIn = find_meaningful_data(message);
    LENGTH_PROTOCOL length = find_length_of_data(message);
    length = length - find_length_of_lengthField(message) - 1;

    mes->length = (LENGTH)length;

    char * willmessage = (char *)malloc(length);
    if(willmessage == NULL){
        err_num = MALLOC_ERR;
        ErrHandler();
        return 1;}
    memset(willmessage, 0, length);

    int i;

     char *s1, *s2;
    s1 = (char *)willmessage;
    s2 = (char *)dataIn;

    for(i=0;i<length;i++){


        *s1 = *s2;
        s1++;
        s2++;
    }

//    snprintf(willmessage,length,dataIn);
    mes->message = (void *)willmessage;

    mes->duplicate = 0;
    mes->message_id = 0;
    mes->extern_message_id = -1;
    mes->short_topic_id = -1;
    mes->qos = -2;
    mes->return_value = 0;
    mes->type = WILLMSGUPD;
    mes->connection_id = 0;

//    list_ptr_t item;
//    list_get_by_address(list_in,message->device_address, &item);

//    if(item == NULL){
//        err_num = NO_DEVICE_ERR;
//        ErrHandler();
//        return 1;
//    }
//    mes->duration = item->data->duration;



    Free_message_info(message);

//    if(list_update_device(&list_in,(DADDRESS)(mes->address))){
//        err_num = UNEXPECTED_ERR;
 //       ErrHandler();
//        return 1;     DATA dataIn = find_meaningful_data(message);
//    }

    send_db(mes);

    return 0;
}

/*------------------------------------------------------------------------------*/
int handle_publish(message_info *message){
/*------------------------------------------------------------------------------*/

    DEBUG_HANDLE_INCOMING_MESSAGE_PRINT("handle_publish\n");

    MESSAGE *mes = (MESSAGE *)malloc(size_MESSAGE);
    if(mes==NULL){
        err_num = MALLOC_ERR;
        ErrHandler();
        return 1;}
    memset(mes, 0, size_MESSAGE);

     mes->address = message->device_address;

    DATA dataIn = find_meaningful_data(message);

    TOPIC_ID  topic_id = *((TOPIC_ID *)(dataIn+1));

    topic_id = (topic_id>>8)|(topic_id<<8);

    printf("the topic_id i stored in database is %d\n",topic_id);

    MESSAGE_ID message_id = *((MESSAGE_ID *)(dataIn+3));

    printf("the message_id i stored in database is %d \n",message_id);
    DATA valueData = (DATA )(dataIn+5);
    uint16_t length = find_length_of_data(message);

    char * publish;

    if(length == 11){
        length = 4;

        uint32_t *data = (uint32_t *)malloc(4);
        data = (uint32_t *)valueData;
      //  publish  = (float *)valueData;
        *data = (((*data)>>24)&0xff)|(((*data)<<8)&0xff0000)|(((*data)>>8)&0xff00)|(((*data)<<24)&0xff000000);
        publish = data;
        printf("the value data i stored in database if length = 11 is %ud \n",*((uint32_t *)(publish)));
        int i;
        char *w = (char *)publish;
        for(i = 0;i<4;i++){
            printf("%x\n",*w);
            w++;
        }
    }else{
        length = length - find_length_of_lengthField(message) - (uint16_t)6;//the useful data length.



        char * p = (char *)malloc(length);
        if(p == NULL){
            err_num = MALLOC_ERR;
            ErrHandler();
            return 1;}
        memset(p, 0, length);


        char *s1, *s2;
        s1 = (char *)p;
        s2 = (char *)valueData;

        int i;
        printf("the publish message in publish packet if length is not 11\n");
        for(i=0;i<length;i++){


            *s1 = *s2;
            printf("%x\n",*s1);
            s1++;
            s2++;

        }
        publish = p;
    }

//    snprintf(publish,length,valueData);
    mes->message = (void *)publish;

    mes->length = (LENGTH)length;

    FLAG flag = *((FLAG *)dataIn);

    DUP dup = get_dup(flag);
    QOS qos = get_qos(flag);
    RETAIN retain = get_retain(flag);
    TOPIC_TYPE topic_type = get_topic_type(flag);

    mes->duplicate = (FLAG)dup;
    mes->extern_message_id = (EXTERN_ID)message_id;
    mes->short_topic_id = (EXTERN_ID)topic_id;
    mes->qos = qos;
    mes->message_id = 0;
    mes->return_value = 0;
    mes->type = PUBLISH;
    mes->connection_id = 0;

 //   list_ptr_t item;
 //   list_get_by_address(list_in,message->device_address, &item);

 //   if(item == NULL){
//        err_num = NO_DEVICE_ERR;
//        ErrHandler();
//        return 1;
//    }
//    mes->duration = item->data->duration;

        Free_message_info(message);

 //       if(list_update_device(&list_in,(DADDRESS)(mes->address))){
 //           err_num = UNEXPECTED_ERR;
 //           ErrHandler();
 //           return 1;
 //       }

        send_db(mes);
        return 0;
}


/*------------------------------------------------------------------------------*/
int handle_pubrel(message_info *message){
/*------------------------------------------------------------------------------*/

        DEBUG_HANDLE_INCOMING_MESSAGE_PRINT("handle_pubrel\n");

        DATA dataIn = find_meaningful_data(message);

        MESSAGE_ID message_id = *((MESSAGE_ID *)dataIn);

        MESSAGE *mes = (MESSAGE *)malloc(size_MESSAGE);
        if(mes==NULL){
            err_num = MALLOC_ERR;
            ErrHandler();
            return 1;}
        memset(mes, 0, size_MESSAGE);

         mes->address = message->device_address;

         mes->duplicate = 0;
         mes->extern_message_id = (EXTERN_ID)message_id;
         mes->short_topic_id = -1;
         mes->qos = -2;
         mes->message_id = 0;
         mes->return_value = 0;
         mes->type = PUBREL;
         mes->connection_id = 0;
         mes->length = 0;
         mes->message = NULL;

//         list_ptr_t item;
//         list_get_by_address(list_in, message->device_address, &item);

//         if(item == NULL){
//             err_num = NO_DEVICE_ERR;
//             ErrHandler();
//             return 1;
//         }
//         mes->duration = item->data->duration;

        Free_message_info(message);

 //       if(list_update_device(&list_in,(DADDRESS)(mes->address))){
//            err_num = UNEXPECTED_ERR;
 //           ErrHandler();
 //           return 1;
 //       }

        send_db(mes);
        return 0;
}

/*------------------------------------------------------------------------------*/
int handle_disconnect(message_info *message){
/*------------------------------------------------------------------------------*/

     DEBUG_HANDLE_INCOMING_MESSAGE_PRINT("handle_disconnect\n");

     uint16_t length = find_length_of_data(message);

     MESSAGE *mes = (MESSAGE *)malloc(size_MESSAGE);
     if(mes==NULL){
         err_num = MALLOC_ERR;
         ErrHandler();
         return 1;}
     memset(mes, 0, size_MESSAGE);

      mes->address = message->device_address;

      mes->duplicate = 0;
      mes->extern_message_id = -1;
      mes->short_topic_id = -1;
      mes->qos = -2;
      mes->message_id = 0;
      mes->return_value = 0;
      mes->type = DISCONNECT;
      mes->connection_id = 0;
      mes->length = 0;
      mes->message = NULL;

    if(length > 2){

        DEBUG_HANDLE_INCOMING_MESSAGE_PRINT("detect sleep disconnect message.");

        DATA dataIn = find_meaningful_data(message);
        TIME_PROTOCOL duration = *((TIME_PROTOCOL *)(dataIn));
        mes->duration = (time_t)duration;//TIME SHOULD BE THE SAME TYPE AS TIME_T.

       // list_device_sleep(list_in,message->device_address,duration);

    }else{

      //   list_delete_device(&list_in, message->device_address);
         mes->duration = 0;
    }

    Free_message_info(message);

    send_db(mes);// when debug, this function should changed.

    return 0;
}

/*------------------------------------------------------------------------------*/
int handle_connect(message_info *message){
/*------------------------------------------------------------------------------*/

    DEBUG_HANDLE_INCOMING_MESSAGE_PRINT("handle_connect\n");

     //lack will flag, cleansession.

    DATA dataIn = find_meaningful_data(message);

    PROTOCOL_ID  protocol_id = *((PROTOCOL_ID *)(dataIn+1));
    printf("the protocol id is %x\n",protocol_id);

     //check whether the protocol is supported.
     if(protocol_support(protocol_id)){

         MESSAGE *mes = (MESSAGE *)malloc(size_MESSAGE);
         if(mes==NULL){
             err_num = MALLOC_ERR;
             ErrHandler();
             return 1;}
         memset(mes, 0, size_MESSAGE);

          mes->address = message->device_address;

          DADDRESS ad = (mes->address);
          char *adress = &ad;
          int i;
          for(i = 0;i<16;i++){
              printf("%x\n",*adress);
              adress++;
          }
          printf("the address of the device in register packet %16x \n",(mes->address));


          mes->duplicate = 0;
          mes->extern_message_id = -1;
          mes->short_topic_id = -1;
          mes->qos = -2;
          mes->message_id = 0;
          mes->return_value = 0;
          mes->type = CONNECT;
          mes->connection_id = 0;

       FLAG flag = *((FLAG *)(dataIn));

       printf("the flag is %x \n",flag);

       WILL will = get_will(flag);

       CLEAN_SESSION clean_session = get_clean_session(flag);

       CLIENT_ID client_id = (CLIENT_ID )(dataIn+4);

       DATA_LENGTH length = (DATA_LENGTH)find_length_of_data(message);

       printf("the length of the connect message:%d\n", (int)length);

       length = length - find_length_of_lengthField(message) - (DATA_LENGTH)5;


       char *client_id_char = (char *)malloc(length);
       if(client_id_char == NULL){
           err_num = MALLOC_ERR;
           ErrHandler();
           return 1;}
  //     memset(client_id_char, 0, length);


       strcpy(client_id_char,client_id);


       char *s1, *s2;
       s1 = (char *)client_id_char;

       s2 = (char *)client_id;
       printf("the client id is %s\n",client_id_char);
       printf("the client id is %s\n",client_id);


       for(i=0;i<=length;i++){


           *s1 = *s2;
 //        printf("%x\n",*s1);
           s1++;
           s2++;
       }

 //      snprintf(client_id_char, length, client_id);

       TIME_PROTOCOL duration = *((TIME_PROTOCOL *)(dataIn+2));

       duration = ((duration>>8)|(duration<<8));

//       char *temp =(char *) duration;
//               *temp = *(dataIn+2);
//       temp++;
//       *temp = *(dataIn+3);
       printf("the duration is %x \n",duration);

  //     if(list_add_in(&list_in, duration,message->device_address)){
  //         err_num = ADD_IN_FAILURE;
  //         ErrHandler();
  //         return 1;//Find another way to solve this. just retain is not a good choice.
  //     }
       mes->duration = (time_t)duration;
       mes->length = (LENGTH)length;
       mes->message = (void *)client_id_char;
       mes->will = will;
       mes->clean = clean_session;
       send_db(mes);
       Free_message_info(message);
       return 1;
     }

     else{

         message_info *reply_message = (message_info *)malloc(size_message_info);
         if(reply_message == NULL){
             err_num = MALLOC_ERR;
             ErrHandler();
             return 1;}
         memset(reply_message, 0, size_message_info);

         reply_message->device_address = message->device_address;

         connack_packet *reply = (connack_packet *)malloc(size_connack_packet);
         if(reply == NULL){
             err_num = MALLOC_ERR;
             ErrHandler();
             return 1;}
         memset(reply, 0, size_connack_packet);

         reply_message->data = (DATA )reply;

     reply->return_code = 0x03;

     reply->length = (uint8_t)(size_connack_packet);
     reply->type = CONNACK;

     Free_message_info(message);

 //    if(list_update_device(&list_in,(DADDRESS)(reply_message->device_address))){
  //       err_num = UNEXPECTED_ERR;
  //       ErrHandler();
  //       return 1;
 //    }

     send_connection(reply_message);//this is to the output list.
     return 0;}
}

/*------------------------------------------------------------------------------*/
int handle_willtopic(message_info *message){
/*------------------------------------------------------------------------------*/

    DEBUG_HANDLE_INCOMING_MESSAGE_PRINT("handle_willtopic\n");

    DATA dataIn = find_meaningful_data(message);

    FLAG flag = *((FLAG *)dataIn);
    RETAIN retain = get_retain(flag);
    QOS qos = get_qos(flag);

    MESSAGE *mes = (MESSAGE *)malloc(size_MESSAGE);
    if(mes==NULL){
        err_num = MALLOC_ERR;
        ErrHandler();
        return 1;}
    memset(mes, 0, size_MESSAGE);
     mes->address = message->device_address;

     mes->duplicate = 0;
     mes->extern_message_id = -1;
     mes->short_topic_id = -1;
     mes->qos = qos;
     mes->retain= retain;
     mes->message_id = 0;
     mes->return_value = 0;
     mes->type = WILLTOPIC;
     mes->connection_id = 0;

     LENGTH_PROTOCOL length = find_length_of_data(message);
     length = length - find_length_of_lengthField(message) - 2;

     mes->length = (LENGTH)length;

     char * willTopic = (char *)malloc(length);
     if(willTopic==NULL){
         err_num = MALLOC_ERR;
         ErrHandler();
         return 1;}
     memset(willTopic, 0, length);

         char *s1, *s2;
     int i;

     s1 = (char *)willTopic;
     s2 = (char *)dataIn;

     for(i=0;i<length;i++){


         *s1 = *s2;
         s1++;
         s2++;
     }

//     snprintf(willTopic,length,dataIn);
     mes->message = (void *)willTopic;

     list_ptr_t item;
     list_get_by_address(list_in,message->device_address, &item);

     if(item == NULL){
         err_num = NO_DEVICE_ERR;
         ErrHandler();
         return 1;
     }
     mes->duration = item->data->duration;


    Free_message_info(message);

//    if(list_update_device(&list_in,(DADDRESS)(mes->address))){
//        err_num = UNEXPECTED_ERR;
//        ErrHandler();
//        return 1;
//    }

    send_db(mes);
    return 0;
}

/*------------------------------------------------------------------------------*/
int handle_willmessage(message_info *message){
/*------------------------------------------------------------------------------*/

    DEBUG_HANDLE_INCOMING_MESSAGE_PRINT("handle_willmessage\n");

    DATA dataIn = find_meaningful_data(message);
    uint16_t length = find_length_of_data(message);
    length = length - find_length_of_lengthField(message) - 1;

    MESSAGE *mes = (MESSAGE *)malloc(size_MESSAGE);
    if(mes==NULL){
        err_num = MALLOC_ERR;
        ErrHandler();
        return 1;}
    memset(mes, 0, size_MESSAGE);
     mes->address = message->device_address;

     mes->duplicate = 0;
     mes->extern_message_id = -1;
     mes->short_topic_id = -1;
     mes->qos = -2;
     mes->message_id = 0;
     mes->return_value = 0;
     mes->type = WILLMSG;
     mes->connection_id = 0;

     mes->length = (LENGTH)length;

     char * willMessage = (char *)malloc(length);
     if(willMessage == NULL){
         err_num = MALLOC_ERR;
         ErrHandler();
         return 1;}
     memset(willMessage, 0, length);


  char *s1, *s2;
     int i;

     s1 = (char *)willMessage;
     s2 = (char *)dataIn;

     for(i=0;i<length;i++){


         *s1 = *s2;
         s1++;
         s2++;
     }


     mes->message = (void *)willMessage;

     list_ptr_t item;
     list_get_by_address(list_in,message->device_address, &item);

     if(item == NULL){
         err_num = NO_DEVICE_ERR;
         ErrHandler();
         return 1;
     }
     mes->duration = item->data->duration;

    Free_message_info(message);

//    if(list_update_device(&list_in,(DADDRESS)(mes->address))){
//        err_num = UNEXPECTED_ERR;
//        ErrHandler();
//        return 1;
//    }

    send_db(mes);
    return 0;
}

/*------------------------------------------------------------------------------*/
DUP get_dup(FLAG flag){
/*------------------------------------------------------------------------------*/
//return the value of flag dup.
    DUP dup = flag&0x80;
    dup >> 7;
    return dup;
}

/*------------------------------------------------------------------------------*/
QOS get_qos(FLAG flag){
/*------------------------------------------------------------------------------*/
//return the value of flag qos.
    QOS qos = flag&0x60;
    qos>>5;
    return qos;
}

/*------------------------------------------------------------------------------*/
RETAIN get_retain(FLAG flag){
/*------------------------------------------------------------------------------*/
//return the value of flag retain.
    RETAIN retain = flag&0x10;
    retain>>4;
    return retain;
}

/*------------------------------------------------------------------------------*/
WILL get_will(FLAG flag){
/*------------------------------------------------------------------------------*/
//return the value of flag will.
    WILL will = flag&0x08;
    will>>3;
    return will;
}

/*------------------------------------------------------------------------------*/
CLEAN_SESSION get_clean_session(FLAG flag){
/*------------------------------------------------------------------------------*/
//return the value of flag clean session.
    CLEAN_SESSION clean_session = flag&0x04;
    clean_session>>2;
    return clean_session;
}

/*------------------------------------------------------------------------------*/
TOPIC_TYPE get_topic_type(FLAG flag){
/*------------------------------------------------------------------------------*/
//return the value of flag topic type.
    TOPIC_TYPE topic_type = flag&0x03;
    return topic_type;
}


/*------------------------------------------------------------------------------*/
uint8_t protocol_support(PROTOCOL_ID protocol_id_check){
/*------------------------------------------------------------------------------*/
//check whether the protocol id is supported by this system.
    uint8_t result=0;
    if(protocol_id_check == protocol_v1){
        result = 1;
    }
    return result;
}

/*------------------------------------------------------------------------------*/
int Free_message_info(message_info *message){
/*------------------------------------------------------------------------------*/
//free the memory of type message_info.
    free(message->data);
    free(message);
    message = NULL;
    return 0;
}

/*------------------------------------------------------------------------------*/
uint8_t find_length_of_lengthField(message_info *message){
/*------------------------------------------------------------------------------*/
//the length field can be 1 or 3. return the actual value of length field.
    if(message == NULL){
        err_num = MESSAGE_INFO_NULL_ERR;
        ErrHandler();
        return -1;
    }
    if(message->data == NULL){
        err_num = DATA_NULL_ERR;
        ErrHandler();
        return -1;
    }
    BYTE *data = (BYTE *)(message->data);
    BYTE len1 = *data;
    if(len1 == 0x01){
      return 3;}
    else{return 1;}
}

/*------------------------------------------------------------------------------*/
uint16_t find_length_of_data(message_info *message){
/*------------------------------------------------------------------------------*/
//return the length of the packet.
    uint8_t len_of_lenField = find_length_of_lengthField(message);
    if(len_of_lenField == -1){
        err_num = INVALID_DATA_ERR;
        ErrHandler();
        return -1;
    }
    BYTE *data = (BYTE *)(message->data);
    BYTE len1 = *data;
    if(len_of_lenField==1){
         return (uint16_t)len1;}
    else{
         uint16_t *len = (uint16_t*)(++data);
         return *len;}
}

/*------------------------------------------------------------------------------*/
DATA find_meaningful_data(message_info *message){
/*------------------------------------------------------------------------------*/
//return the pointer point to the field after message type.
    uint8_t len_of_lenField = find_length_of_lengthField(message);
    if(len_of_lenField == -1){
        err_num = INVALID_DATA_ERR;
        ErrHandler();
        return NULL;
    }
    DATA data = (DATA )(message->data);
    data = data + len_of_lenField + 1;
    return data;
}

/*------------------------------------------------------------------------------*/
TYPE find_message_type(message_info *message){
/*------------------------------------------------------------------------------*/
//return the value of type field.
    uint8_t len_of_lenField = find_length_of_lengthField(message);
    if(len_of_lenField == -1){
        err_num = INVALID_DATA_ERR;
        ErrHandler();
        return (TYPE)-1;
    }
    BYTE *data = (BYTE *)(message->data);
    TYPE type = *((BYTE *)(data+len_of_lenField));
    return type;
}

