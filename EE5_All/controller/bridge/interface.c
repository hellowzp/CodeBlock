/*******************************************************************************
*
* FILENAME: interface.c
*
* Description:
* Implements the bridge's pipes and FIFOs
*
*******************************************************************************/

/*-----------------------------------------------------------------------------
        include files
------------------------------------------------------------------------------*/

#include "interface.h"

/*------------------------------------------------------------------------------
        definitions (defines, typedefs, ...)
------------------------------------------------------------------------------*/

//file descriptor for database pipes
extern int fd_write_db;
extern int fd_read_db;

//file descriptor for coordinator pipes
extern int up_stream_pipe[2];
extern int down_stream_pipe[2];

extern pthread_mutex_t mutex_write_db;

//Set the name of fifos between the bridge and Java controller
#define FIFO_TO_JAVA 	"tmp/bridgetojava"
#define FIFO_FROM_JAVA	"tmp/javatobridge"

#define size_MQTT sizeof(MQTT)

#define DEBUG_INTERFACE_PRINT(...) \
  do{                                                     \
      printf("In %s in function %s at line %d:", __FILE__,__func__,__LINE__);\
      printf(__VA_ARGS__);\
  } while(0)

#define DEBUG_INTERFACE

/*------------------------------------------------------------------------------
        implementation code
------------------------------------------------------------------------------*/

int send_controller(MESSAGE *m){
    //Open FIFO (write sync)
    FILE* fifo;
    fifo = fopen(FIFO_TO_JAVA, "w");

    //Write data to FIFO
    fwrite(&(m->short_topic_id), sizeof(EXTERN_ID), 1, fifo);
    fwrite(&(m->type), sizeof(MESSAGE_TYPE), 1, fifo);
    fwrite(&(m->return_value), 1, 1, fifo); //Manually use length of 1 byte
    fwrite(&(m->length), sizeof(LENGTH), 1, fifo);
    if((m->length) > 0){
	    fwrite(m->message, m->length, 1, fifo);
    }
    fflush(fifo);

    //Error handling
    if(ferror(fifo)){
        err_num = IO_JAVA_WRITE_ERR;
        ErrHandler();
        DEBUG_INTERFACE_PRINT("I/O error while sending data from bridge to Java through FIFO.\n");
        fclose(fifo);
        return 1;
    }

    //Close FIFO and clean up
    fclose(fifo);
    free(m);
    return 0;
}

void read_java(){
    DEBUG_INTERFACE_PRINT("in thread read java.\n");

    MESSAGE *m_java = read_controller();

    m_java->address = CONTROLLER_ADDRESS;

    pthread_mutex_lock(&mutex_write_db);

    int written = write(fd_write_db, m_java, size_MESSAGE);
    if(written != sizeof(MESSAGE)){
        err_num = WRITE_DB_ERR;
        ErrHandler();
        DEBUG_INTERFACE_PRINT("error when write to database.\n");
    } else {
        if((m_java->length) != 0){
            written = write(fd_write_db, m_java->message, m_java->length);
            if(written != m_java->length){
                err_num = WRITE_DB_ERR;
                ErrHandler();
                DEBUG_INTERFACE_PRINT("error when write to database.\n");
            }
			free((void*) m_java->message);
        }
    }
    pthread_mutex_unlock(&mutex_write_db);
    free(m_java);
}


MESSAGE *read_controller(){
    //Open FIFO (read sync)
    FILE* fifo;
    fifo = fopen(FIFO_FROM_JAVA, "r");

    //Read data from FIFO
    MESSAGE* m = (MESSAGE*) malloc(sizeof(MESSAGE));
    fread(&(m->address), sizeof(ADDRESS), 1, fifo);
    fread(&(m->short_topic_id), sizeof(EXTERN_ID), 1, fifo);
    fread(&(m->type), sizeof(MESSAGE_TYPE), 1, fifo);
    fread(&(m->duplicate), sizeof(FLAG), 1, fifo);
    fread(&(m->will), sizeof(FLAG), 1, fifo);
    fread(&(m->clean), sizeof(FLAG), 1, fifo);
    fread(&(m->retain), sizeof(FLAG), 1, fifo);
    fread(&(m->return_value), 1, 1, fifo); //Manually use length of 1 byte
    fread(&(m->length), sizeof(LENGTH), 1, fifo);
    if(m->length > 0){
    	//CONNECT, SUBSCRIBE and UNSUBSCRIBE messages have to be NULL terminated (calloc 1 additional byte)
    	switch(m->type){
    		case CONNECT:
    		case SUBSCRIBE:
    		case UNSUBSCRIBE:
	    		m->message = (void*) calloc(1, (m->length)+1);
	    		break;
	    	default:
				m->message = (void*) calloc(1, (m->length));
				break;
		}
    	
        fread(m->message, m->length, 1, fifo);
        
        //If NULL terminated, increase length with 1 byte
        switch(m->type){
    		case CONNECT:
    		case SUBSCRIBE:
    		case UNSUBSCRIBE:
	    		(m->length)++; //Make NULL terminated
	    		break;
    	}
    	
	}
	
    //Error handling
    if(ferror(fifo) || feof(fifo)){
        err_num = IO_JAVA_READ_ERR;
        ErrHandler();
        DEBUG_INTERFACE_PRINT("I/O error while receiving data from Java to bridge through FIFO.\n");
        fclose(fifo);
        return NULL;
    }

    //Close FIFO and return message pointer
    fclose(fifo);
    return m;
}

int send_connection(message_info *message){
    if(message == NULL){
        err_num = MESSAGE_INFO_NULL_ERR;
        ErrHandler();
        return 1;
    }

    if((message->data) == NULL){
            err_num = DATA_INVALID_ERR;
            ErrHandler();
            return 1;
      }

    //for debug
    TYPE type = find_message_type(message);
    printf("the message type is %d:\n", (int)(type));


    mqtt_ptr_t out = (mqtt_ptr_t)malloc(size_MQTT);
    if(out == NULL){
        err_num = MALLOC_ERR;
        ErrHandler();
        return 1;
    }

    memset(out, 0, size_MQTT);

    out->addr = message->device_address;
    out->len = (unsigned char)find_length_of_data(message);
    out->msg = message->data;
    out->ts = time(NULL);

    Byte nbytes = sizeof(*out) + out->len;   //actual data bytes
            void* bytestream = malloc(nbytes+1);
           *(Byte*)bytestream = nbytes;
           memcpy(bytestream+1, out, sizeof(*out));
           memcpy(bytestream+sizeof(*out)+1, out->msg, out->len);
          //  DEBUG_PRINT("coordinator write %d bytes to pipe...\n",nbytes);
            nbytes = write(down_stream_pipe[1], bytestream, nbytes+1);
            free(out->msg);
            free(out);
            free(bytestream);

            printf("\t finish send to bridge\n");
//    int written = write(down_stream_pipe[1], out, size_MQTT);
//    if(written == -1){
//        err_num = WRITE_COORDINATOR_ERR;
//        ErrHandler();
//        DEBUG_INTERFACE_PRINT("WRITE TO COORDINATOR ERROR!\n\n");
//    } else {
//            written = write(down_stream_pipe[1], out->msg, out->len);
//            if(written == -1){
//                err_num = WRITE_COORDINATOR_ERR;
//                ErrHandler();
//                DEBUG_INTERFACE_PRINT("WRITE TO COORDINATOR ERROR!\n\n");
//            }
//        }

//    DEBUG_INTERFACE_PRINT("finish send to coordinator.\n");
//    free(out->msg);
//    free(out);

 //   free(message->data);
    free(message);
    message = NULL;

    printf("\tfinish send to connection\n");
//        mqtt_ptr_t mqtt = malloc(sizeof(*mqtt));
//        mqtt->addr = 12345;
//        mqtt->len = 6;
//        mqtt->ts = time(NULL);
//        mqtt->msg = malloc(6);
//        *(Byte*)mqtt->msg = 6;
//        memcpy(mqtt->msg+1, "hello", 5);

//        Byte nbytes = sizeof(*mqtt) + mqtt->len;   //actual data bytes
//        void* bytestream = malloc(nbytes+1);
//        *(Byte*)bytestream = nbytes;
//        memcpy(bytestream+1, mqtt, sizeof(*mqtt));
//        memcpy(bytestream+sizeof(*mqtt)+1, mqtt->msg, mqtt->len);
//      //  DEBUG_PRINT("coordinator write %d bytes to pipe...\n",nbytes);
//        nbytes = write(down_stream_pipe[1], bytestream, nbytes+1);
//        free(mqtt->msg);
//        free(mqtt);
//        free(bytestream);
    return 0;
}

void read_connection(){

    DEBUG_INTERFACE_PRINT("%s %d\n","bridge reading pipe: ",up_stream_pipe[0]);
    Byte bytes = 0;
    read(up_stream_pipe[0],&bytes,1);
    if(bytes<=sizeof(MQTT)) {
        DEBUG_INTERFACE_PRINT("%s\n","mqtt data too short...");
        return;
    }
    DEBUG_INTERFACE_PRINT("brdge is going to receive another %d bytes..",bytes);
    mqtt_ptr_t mqtt = malloc(sizeof(*mqtt));
    void* data =  malloc(bytes-sizeof(*mqtt));
    read(up_stream_pipe[0],mqtt,sizeof(*mqtt));
    read(up_stream_pipe[0], data, bytes-sizeof(*mqtt));
    mqtt->msg = data;

//    char *s1 = (void *)mqtt;
//    int i;
//    for(i = 0;i<(sizeof(*mqtt));i++){
//        printf("received first part from coordinator:\n");
//        printf("%x\n",*s1);
//        s1++;
//    }
//    s1 = data;

//    for(i = 0;i<(bytes-sizeof(*mqtt));i++){
//        printf("received second part from coordinator:\n");
//        printf("%x\n",*s1);
//    }

    message_info *message = (message_info *)malloc(size_message_info);
    if(message == NULL){
        err_num = MALLOC_ERR;
        ErrHandler();
        return;
    }

//      DEBUG_INTERFACE_PRINT("i am finishing reading from coordinator111111111111.\n");
    message->data = mqtt->msg;
    message->device_address=(DADDRESS)(mqtt->addr);

    free(mqtt);

    handle_in_message(message);

    DEBUG_INTERFACE_PRINT("address: %ld  message: %s",mqtt->addr, (char*)mqtt->msg);
}


int send_db(MESSAGE *mes){
    if(mes == NULL){
        err_num = MESSAGE_NULL_ERR;
        ErrHandler();
        return 1;
    }

    printf("the message type is IN SENDDB FUNCTION %d:\n", (int)(mes->type));
    if((mes->length) > 0){
        if(mes->message == NULL){
            err_num = MESSAGE_NULL_ERR;
            ErrHandler();
        }
        else
        {
 //          pthread_mutex_lock(&mutex_write_db);
//printf("after mutex_lock\n");
            int written = write(fd_write_db, mes, size_MESSAGE);
            if(written != size_MESSAGE){
                err_num = WRITE_DB_ERR;
                ErrHandler();
                DEBUG_INTERFACE_PRINT("error when write to database.\n");
            } else {
                if(mes->length != 0){
                    written = write(fd_write_db, mes->message, mes->length);
                    if(written != mes->length){
                        err_num = WRITE_DB_ERR;
                        ErrHandler();
                        DEBUG_INTERFACE_PRINT("error when write to database.\n");
                    }

  //                  free((mes->message));

                }
            }
        }
      }

 //    pthread_mutex_unlock(&mutex_write_db);
 //    printf("after mutex_unlock\n");
database_request_message();
    free(mes);
    mes = NULL;
//    printf("after free the message that already send to database\n");
    return 0;
}

void read_db(){

    DEBUG_INTERFACE_PRINT("i am start read from database.\n");


    MESSAGE* m = (MESSAGE*) malloc(sizeof(MESSAGE));
    if(m == NULL){
        err_num = MALLOC_ERR;
        ErrHandler();
        return;
    }

    int bytes_read = 0;

    database_request_message();

 //   printf("before read from database\n");

    bytes_read = read(fd_read_db, m, sizeof(MESSAGE));

 //   printf("aaaaaaaaaaaaaaaaaaaaaaaaaaaaa");
    if(bytes_read == -1){
        err_num = READ_DB_ERR;
        ErrHandler();
        DEBUG_INTERFACE_PRINT("error when read from database! maybe pipe closed....\n");
    }

    if(m->length != 0){
        void* data = malloc(m->length);
        if(data == NULL){
            err_num = MALLOC_ERR;
            ErrHandler();
            free(m);
            return;
        }
   //     printf("read from database for the second time \n");
        bytes_read = read(fd_read_db, data, m->length);
        if(bytes_read == -1){
            err_num = READ_DB_ERR;
            ErrHandler();
            free(data);
            free(m);
            DEBUG_INTERFACE_PRINT("error when read from database!\n");
            return;
        }
        m->message = data;
    }

//    DEBUG_INTERFACE_PRINT("i am start handle output message.\n");
    handle_out_message(m);
    DEBUG_INTERFACE_PRINT("i am finish handle output message in function read from database.\n");
}
