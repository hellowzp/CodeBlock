#ifndef OUR_PROTOCOL_H
#define OUR_PROTOCOL_H

/*******************************************************************************
*
* FILENAME: mqtt_sn_bridge.h
*
* Description:
* Specifies the MQTT protocol as used by the bridge
*
*******************************************************************************/

/*-----------------------------------------------------------------------------
        include files
------------------------------------------------------------------------------*/
#include <stdint.h>
#include "../mqtt_sn_db/mqtt_sn_db.h"


/*------------------------------------------------------------------------------
        definitions (defines, typedefs, ...)
------------------------------------------------------------------------------*/
#define protocol_v1          (0x01)

//define gateway id, gateway address and advertise duration.
#define   gwID          (0x11)
#define   gwAddress      (0x0013A20040795BCE)

#define   adDuration     (0x78)
#define   CONTROLLER_ADDRESS  (0x0000000000000001)


//type define some field type.
typedef      uint16_t           LENGTH_PROTOCOL;
typedef      uint16_t           TOPIC_ID;
typedef      uint8_t            BYTE;
typedef      uint64_t           DADDRESS;
typedef      uint32_t           GWDADDRESS;
typedef      uint8_t            GW_ID;
typedef      uint8_t            PROTOCOL_ID;  // our version 0x01;
typedef      uint16_t            TIME_PROTOCOL;
typedef      int                TYPE;
typedef      char*              TOPIC_NAME;
typedef      uint8_t            TOPIC_TYPE ;
typedef      char*              CLIENT_ID;  // not correct here
typedef      uint8_t            RETURN_CODE;     
typedef      uint8_t            FLAG;
typedef      uint16_t           MESSAGE_ID;
typedef      char*              DATA; //need to find out what exactly the data type is
typedef      uint16_t           DATA_LENGTH;


#define MAX_SIZE_TOPIC_NAME 30
#define MAX_MESSAGE_SIZE    100

#define  size_message_info   sizeof(message_info)
#define  size_advertise_packet   sizeof(advertise_packet)
#define  size_searchgw_packet   sizeof(searchgw_packet)
#define  size_gwinfo_packet   sizeof(gwinfo_packet)
#define  size_register_packet   sizeof(register_packet)
#define  size_regack_packet   sizeof(regack_packet)
#define  size_publish_packet   sizeof(publish_packet)
#define  size_puback_packet   sizeof(puback_packet)
#define  size_connect_packet   sizeof(connect_packet)
#define  size_connack_packet   sizeof(connack_packet)
#define  size_s_disconnect_packet   sizeof(s_disconnect_packet)
#define  size_disconnect_packet   sizeof(disconnect_packet)
#define  size_willtopicreq_packet   sizeof(willtopicreq_packet)
#define  size_willtopic_packet   sizeof(willtopic_packet)
#define  size_willmessage_packet   sizeof(willmessage_packet)
#define  size_pingreq_packet   sizeof(pingreq_packet)
#define  size_pingresp_packet   sizeof(pingresp_packet)
#define  size_subscribe_packet   sizeof(subscribe_packet)
#define  size_suback_packet   sizeof(suback_packet)
#define  size_unsuback_packet   sizeof(unsuback_packet)
#define  size_pubrec_packet   sizeof(pubrec_packet)
#define  size_willtopicUPD_packet   sizeof(willtopicUPD_packet)
#define  size_willmessageUPD_packet   sizeof(willmessageUPD_packet)
#define  size_MESSAGE               sizeof(MESSAGE)


//define different message type struct.
typedef struct{
   DADDRESS    device_address;
   void*       data;
}message_info;

typedef struct{
   DADDRESS    device_address;
   LENGTH_PROTOCOL      length_of_real_data;
   DATA        *real_data;
}data_info;

typedef struct {
  LENGTH_PROTOCOL       length;
  TYPE         type;
  GW_ID        gwid;
  TIME_PROTOCOL         duration;
} advertise_packet;

typedef struct {
  LENGTH_PROTOCOL       length;
  TYPE         type;
} searchgw_packet;

typedef struct {
  LENGTH_PROTOCOL       length;
  TYPE         type;
  GW_ID        gwid;
  GWDADDRESS   gwaddress;
} gwinfo_packet;

typedef struct {
  LENGTH_PROTOCOL       length;
  TYPE         type;
  TOPIC_ID     topic_id;
  MESSAGE_ID   message_id;
  TOPIC_NAME  topic_name;
} register_packet;	


typedef struct {
  LENGTH_PROTOCOL       length;
  TYPE         type;
  TOPIC_ID     topic_id;
  MESSAGE_ID   message_id;
  RETURN_CODE  return_code;
} regack_packet;


typedef struct {
  LENGTH_PROTOCOL      length;
  TYPE         type;
  FLAG         flags;
  TOPIC_ID     topic_id;
  MESSAGE_ID   message_id;
  DATA         p_data;
} publish_packet;

typedef struct {
  LENGTH_PROTOCOL       length;
  TYPE         type;
  TOPIC_ID     topic_id;
  MESSAGE_ID    message_id;
  RETURN_CODE  return_code;
} puback_packet;

typedef struct {
  LENGTH_PROTOCOL       length;
  TYPE         type;
  FLAG         flags;
  PROTOCOL_ID  protocol_id;
  TIME_PROTOCOL         duration;
  CLIENT_ID    client_id;
} connect_packet;

typedef struct {
  LENGTH_PROTOCOL       length;
  TYPE         type;
  RETURN_CODE  return_code;
} connack_packet;

typedef struct {
  LENGTH_PROTOCOL       length;
  TYPE         type;
  TIME_PROTOCOL         duration;
} s_disconnect_packet;

typedef struct {
  LENGTH_PROTOCOL       length;
  TYPE         type;
}disconnect_packet;

typedef struct {
  LENGTH_PROTOCOL       length;
  TYPE         type;
} willtopicreq_packet;

typedef struct {
  LENGTH_PROTOCOL       length;
  TYPE         type;
  FLAG         flags;
  TOPIC_NAME     topic_name;
} willtopic_packet;

typedef struct {
  LENGTH_PROTOCOL       length;
  TYPE         type;
  DATA         will_message;
} willmessage_packet;

typedef struct {
  LENGTH_PROTOCOL       length;
  TYPE         type;
  CLIENT_ID    client_id;
} pingreq_packet;

typedef struct {
  LENGTH_PROTOCOL       length;
  TYPE         type;
} pingresp_packet;

typedef struct {
  LENGTH_PROTOCOL length;
 TYPE type;
 FLAG flags;
  MESSAGE_ID message_id;
  union {
       TOPIC_NAME topic_name;
       TOPIC_ID topic_id;
   };
} subscribe_packet;

typedef struct {
  LENGTH_PROTOCOL length;
  TYPE type;
  FLAG flags;
  TOPIC_ID topic_id;
  MESSAGE_ID message_id;
  RETURN_CODE return_code;
} suback_packet;

typedef struct {
  LENGTH_PROTOCOL length;
  TYPE type;
  MESSAGE_ID message_id;
} unsuback_packet;

typedef struct {
  LENGTH_PROTOCOL length;
  TYPE type;
  MESSAGE_ID message_id;
} pubrec_packet;

typedef struct {
  LENGTH_PROTOCOL length;
  TYPE type;
  FLAG flags;
  TOPIC_NAME topic_name;
} willtopicUPD_packet;

typedef struct {
  LENGTH_PROTOCOL length;
  TYPE type;
  DATA willmessage;
} willmessageUPD_packet;

#endif
