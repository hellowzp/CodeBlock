#include <WaspXBeeZB.h>
#include <WaspFrame.h>
#include <WaspUtils.h>
#include <string.h>
#include <math.h>
#include <WaspSensorGas_v20.h>
/*
**      Defenitions
 */
#define RETRIES 5

/*
**      Globals
 */
uint8_t  PANID[8]={
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
packetXBee* packet;

const char *TOPIC =  "connect";
const char *MESSAGE = "Waspmote is now offline";
const char *clientID = "Waspmote0";
const char *disconnect = "disconnect";

int topic_temp;
int topic_humid;
int topic_co2;
int topic_will;
int topic_disconnect;

char *str_temp = "wasp/temp";
char *str_humid = "wasp/humid";
char *str_co2 = "wasp/co2";

char written = 0;
int ee_addr_written = 1024;
int ee_addr_temp = 1025;
int ee_addr_humid = 1026;
int ee_addr_co2 = 1027;
int ee_addr_will = 1028;

uint16_t msg_ID = 0;

char *MAC_ADDRESS    = "0013A200406FB1E5";
char *MAC_ADERESS_GW = "0013A20040795BCE";
uint8_t connected;

/*
**      structs, unions & enums
 */
typedef enum  {
ADVERTISE = 0x00,
SEARCHGW = 0x01 ,
GWINFO = 0x02,
CONNECT = 0x04,
WILLTOPICREQ = 0x06,
WILLTOPIC = 0x07,
WILLMSGREQ = 0x08,
WILLMSG = 0x09,
CONNACK = 0x05 ,
PUBLISH = 0x0C ,
PUBACK = 0x0D ,
REGISTER  = 0x0A  ,
UNSUBSCRIBE = 0x14 ,
PINGREQ = 0x16 ,
PUBREC = 0x0F,
PINGRESP = 0x17 ,
DISCONNECT = 0x18,
REGACK = 0x0B,
PUBCOMP = 0x0E,
PUBREL = 0x10,
SUBSCRIBE = 0x12,
SUBACK = 0x13,
UNSUBACK = 0x15,
WILLTOPICUPD = 0x1A,
WILLTOPICRESP = 0x1B,
WILLMSGUPD = 0x1C,
WILLMSGRESP = 0x1D
}pack_t;

typedef struct message
{
  const char *start;
  char *cursor;
  uint16_t length;
  uint8_t msg_type;
} 
msg;

typedef struct measurements
{
  double temp;
  double humid;
  double co2;
} 
meting;


void setup()
{  
  // init USB port
  USB.ON();
  USB.println(F("Test joining available network and send packets"));
  RTC.ON();
  RTC.setTime("14:03:27:04:16:15:00");  
  // init XBee
  xbeeZB.ON();

  delay(1000);

  xbeeZB.setPAN(PANID);

  // 1.2. check AT command flag
  if( xbeeZB.error_AT == 0 ) 
  {
    USB.println(F("PANID set OK"));
  }
  else 
  {
    USB.println(F("Error while setting PANID")); 
  }

  // 1.3. set all possible channels to scan 
  // channels from 0x0B to 0x18 (0x19 and 0x1A are excluded)
  /* Range:[0x0 to 0x3FFF]
   * Channels are scpedified as a bitmap where depending on 
   * the bit a channel is selected --> Bit (Channel): 
   *  0 (0x0B)  4 (0x0F)  8 (0x13)   12 (0x17)
   *  1 (0x0C)  5 (0x10)  9 (0x14)   13 (0x18)
   *  2 (0x0D)  6 (0x11)  10 (0x15)  
   *  3 (0x0E)  7 (0x12)	 11 (0x16)    */
  xbeeZB.setScanningChannels(0x3F, 0xFF);

  // 1.4. check AT command flag  
  if( xbeeZB.error_AT == 0 )
  {
    USB.println(F("scanning channels set OK"));
  }
  else 
  {
    USB.println(F("Error while setting scanning channels")); 
  }

  // 1.5. set channel verification JV=1 in order to make the 
  // XBee module to scan new coordinator
  xbeeZB.setChannelVerification(1);

  // 1.6. check AT command flag    
  if( xbeeZB.error_AT == 0 )
  {
    USB.println(F("verification channel set OK"));
  }
  else 
  {
    USB.println(F("Error while setting verification channel")); 
  }

  // 1.7. write values to XBee memory
  xbeeZB.writeValues();

  // 1.8 reboot XBee module
  xbeeZB.OFF();
  delay(3000); 
  xbeeZB.ON();

  delay(3000);

  /////////////////////////////////////
  // 2. Wait for Association 
  /////////////////////////////////////

  // 2.1. wait for association indication
  xbeeZB.getAssociationIndication();

  while( xbeeZB.associationIndication != 0 )
  { 
    delay(2000);

    // get operating 64-b PAN ID
    xbeeZB.getOperating64PAN();

    USB.print(F("operating 64-b PAN ID: "));
    USB.printHex(xbeeZB.operating64PAN[0]);
    USB.printHex(xbeeZB.operating64PAN[1]);
    USB.printHex(xbeeZB.operating64PAN[2]);
    USB.printHex(xbeeZB.operating64PAN[3]);
    USB.printHex(xbeeZB.operating64PAN[4]);
    USB.printHex(xbeeZB.operating64PAN[5]);
    USB.printHex(xbeeZB.operating64PAN[6]);
    USB.printHex(xbeeZB.operating64PAN[7]);
    USB.println();     

    xbeeZB.getAssociationIndication();
  }


  USB.println(F("\n\nJoined a coordinator!"));
  Utils.blinkLEDs(5000);
  // 2.2. When XBee is associated print all network 
  // parameters unset channel verification JV=0
  xbeeZB.setChannelVerification(0);
  xbeeZB.writeValues();

  // 2.3. get network parameters 
  xbeeZB.getOperating16PAN();
  xbeeZB.getOperating64PAN();
  xbeeZB.getChannel();

  USB.print(F("operating 16-b PAN ID: "));
  USB.printHex(xbeeZB.operating16PAN[0]);
  USB.printHex(xbeeZB.operating16PAN[1]);
  USB.println();

  USB.print(F("operating 64-b PAN ID: "));
  USB.printHex(xbeeZB.operating64PAN[0]);
  USB.printHex(xbeeZB.operating64PAN[1]);
  USB.printHex(xbeeZB.operating64PAN[2]);
  USB.printHex(xbeeZB.operating64PAN[3]);
  USB.printHex(xbeeZB.operating64PAN[4]);
  USB.printHex(xbeeZB.operating64PAN[5]);
  USB.printHex(xbeeZB.operating64PAN[6]);
  USB.printHex(xbeeZB.operating64PAN[7]);
  USB.println();

  USB.print(F("channel: "));
  USB.printHex(xbeeZB.channel);
  USB.println();

  written = Utils.readEEPROM(ee_addr_written);
  if (written == 1)
  {
    topic_temp = Utils.readEEPROM(ee_addr_temp);
    topic_humid = Utils.readEEPROM(ee_addr_humid);
    topic_co2 = Utils.readEEPROM(ee_addr_co2);
  }

  //test metingen:

  //meting *m = (meting *)meet_sensoren();
  //print_meting(m);
  USB.println("");
}


void loop()
{
  /*USB.println(F("Going to sleep: Zzzz..."));
   PWR.deepSleep("00:00:02:00",RTC_OFFSET,RTC_ALM1_MODE1,ALL_OFF);
   
   USB.ON();
   USB.println(F("Waking up: Good Morning world!"));*/
  //send_packet("Adriaan");

  send_searchgw();
  USB.println("SEARCH_GW is send...");

  /*RTC.setAlarm1("00:00:00:60",RTC_OFFSET,RTC_ALM1_MODE2);
   //PWR.sleep(ALL_OFF);
   RTC.getAlarm1();
   RTC.getTime();
   USB.ON();
   RTC.ON(); */

  /*if( intFlag & RTC_INT )
   {
   intFlag &= ~(RTC_INT); // Clear flag
   Utils.blinkLEDs(1000); // Blinking LEDs
   Utils.blinkLEDs(1000); // Blinking LEDs
   USB.println("interrupt");
   }*/

  packetXBee *pack = NULL;
  int type = -1;
  int handle = 3;
  char empty = 1;
  while ((pack == NULL || empty == 1) || type != 1)
   {
     pack = read_pack();
     if (pack != NULL)
     {
       if (pack->data_length != 0)
       {
           empty = 0;
           //USB.println("received packet with size larger then 0");
           //print_packet(pack);
           //USB.println("mss niet juiste type");
           type = get_packet_type(pack);
           if (type == GWINFO)
           {
                USB.print("packet type: ");
                USB.println(type);
                type = 1;
           }
       }
     }
   }
   print_packet(pack);
   free(pack);
   USB.println("GWINFO received");
   

  
  //GWINFO packet received => start Connect
   send_connect();
   USB.println("Connect send...");
   
   //wait for 30 seconds for received packets
  /*pack = NULL;
  type = -1;
  handle = 3;
  empty = 1;
  while ((pack == NULL || empty == 1) || type != 1)
   {
     pack = read_pack();
     if (pack != NULL)
     {
       if (pack->data_length != 0)
       {
         empty = 0;
         type = get_packet_type(pack);
         if (type == WILLTOPICREQ)
         {
              USB.print("packet type: ");
              USB.println(type);
              type = 1;
         }
       }
     }
   }
   print_packet(pack);
   free(pack);
   USB.println("WILLTOPICREQ received");*/
   
   
   //send willtopic to GW
   //send_willtopic();
   //USB.println("Willtopic send...");
   
  //wait for willmsgreq
  /*pack = NULL;
  type = -1;
  handle = 3;
  empty = 1;
  while ((pack == NULL || empty == 1) || type != 1)
   {
     pack = read_pack();
     if (pack != NULL)
     {
       if (pack->data_length != 0)
       {
         empty = 0;
         type = get_packet_type(pack);
         if (type == WILLMSGREQ)
         {
              USB.print("packet type: ");
              USB.println(type);
              type = 1;
         }
       }
     }
   }
   print_packet(pack);
   free(pack);
   USB.println("WILLMSGREQ received");*/
   
   
   //send_willmsg();
   //USB.println("Will message send...");*/
   
   
   //wait for connack
   //wait for willmsgreq
  pack = NULL;
  type = -1;
  handle = 3;
  empty = 1;
  while ((pack == NULL || empty == 1) || type != 1)
   {
     pack = read_pack();
     if (pack != NULL)
     {
       if (pack->data_length != 0)
       {
         empty = 0;
         type = get_packet_type(pack);
         if (type == CONNACK)
         {
           if (pack->data[2] == 0x00)
           {
              USB.print("packet type: ");
              USB.println(type);
              type = 1;
           }
           else
           {
               USB.print("CONNACK with wrong return code:");
               USB.println(pack->data[2],HEX);
           }
         }
       }
     }
   }
   print_packet(pack);
   free(pack);
   USB.println("CONNACK received");
   
   //publish connected
   /*send_publish((char *)clientID, strlen(clientID)+1, topic_will);
   USB.println("publish send...");*/

   
   //register my willtopic_id if i connect
   send_register((char *)TOPIC);
   USB.println("will topic registered...");
   
   //wait for regack
  pack = NULL;
  type = -1;
  handle = 3;
  empty = 1;
  while ((pack == NULL || empty == 1) || type != 1)
   {
     pack = read_pack();
     if (pack != NULL)
     {
       if (pack->data_length != 0)
       {
         empty = 0;
         type = get_packet_type(pack);
         if (type == REGACK)
         {
             if (pack->data[2] == 0x00)
             {
                USB.print("packet type: ");
                USB.println(type);
                type = 1;
                topic_will = ((uint16_t)pack->data[2])<<8 | (uint16_t)pack->data[3];
                USB.print("topic_will: ");
                USB.println(topic_will);
             }
             else
             {
                 USB.print("REGACK with wrong return code:");
                 USB.println(pack->data[2]);
             }
         }
       }
     }
   }
   print_packet(pack);
   free(pack);
   USB.println("REGAK received");
   
   //register my willtopic_id if i disconnect
   send_register((char *)disconnect);
   USB.println("disconnect topic registered...");
   
   //wait for regack
  pack = NULL;
  type = -1;
  handle = 3;
  empty = 1;
  while ((pack == NULL || empty == 1) || type != 1)
   {
     pack = read_pack();
     if (pack != NULL)
     {
       if (pack->data_length != 0)
       {
         empty = 0;
         type = get_packet_type(pack);
         if (type == REGACK)
         {
             if (pack->data[2] == 0x00)
             {
                USB.print("packet type: ");
                USB.println(type);
                type = 1;
                topic_disconnect = ((uint16_t)pack->data[2])<<8 | (uint16_t)pack->data[3];
                USB.print("topic_disconnect: ");
                USB.println(topic_disconnect);
             }
             else
             {
                 USB.print("regack with wrong return code:");
                 USB.println(pack->data[2],HEX);
             }
         }
       }
     }
   }
   print_packet(pack);
   free(pack);
   USB.println("REGAK received");   
   
   //publish connected
   send_publish((char *)str_temp, strlen(clientID)+1, topic_will);
   USB.println("publish send...");
   
   //register temp topic
   send_register(str_temp);
   USB.println("temperature registered...");
   
   //wait for regack
  pack = NULL;
  type = -1;
  handle = 3;
  empty = 1;
  while ((pack == NULL || empty == 1) || type != 1)
   {
     pack = read_pack();
     if (pack != NULL)
     {
       if (pack->data_length != 0)
       {
         empty = 0;
         type = get_packet_type(pack);
         if (type == REGACK)
         {
             if (pack->data[2] == 0x00)
             {
                USB.print("packet type: ");
                USB.println(type);
                type = 1;
                topic_temp = ((uint16_t)pack->data[2])<<8 | (uint16_t)pack->data[3];
                USB.print("topic_temp: ");
                USB.println(topic_temp);
             }
             else
             {
                 USB.print("regack with wrong return code:");
                 USB.println(pack->data[2],HEX);
             }
         }
       }
     }
   }
   print_packet(pack);
   free(pack);
   USB.println("REGACK received");
   
   
   //register  humid topic
   send_register(str_humid);
   USB.println("humidity registered");
   
  //wair for regack
  pack = NULL;
  type = -1;
  handle = 3;
  empty = 1;
  while ((pack == NULL || empty == 1) || type != 1)
   {
     pack = read_pack();
     if (pack != NULL)
     {
       if (pack->data_length != 0)
       {
         empty = 0;
         type = get_packet_type(pack);
         if (type == REGACK)
         {
             if (pack->data[2] == 0x00)
             {
                USB.print("packet type: ");
                USB.println(type);
                type = 1;
                topic_humid = ((uint16_t)pack->data[2])<<8 | (uint16_t)pack->data[3];
                USB.print("topic_humid: ");
                USB.println(topic_humid);
             }
             else
             {
                 USB.print("regack with wrong return code:");
                 USB.println(pack->data[2],HEX);
             }
         }
       }
     }
   }
   print_packet(pack);
   free(pack);
   USB.println("REGACK received");
   
   
   //register  co2 topic
   send_register(str_co2);
   USB.println("CO2 registered...");
   
   //wair for regack
  pack = NULL;
  type = -1;
  handle = 3;
  empty = 1;
  while ((pack == NULL || empty == 1) || type != 1)
   {
     pack = read_pack();
     if (pack != NULL)
     {
       if (pack->data_length != 0)
       {
         empty = 0;
         type = get_packet_type(pack);
         if (type == REGACK)
         {
             if (pack->data[2] == 0x00)
             {
                USB.print("packet type: ");
                USB.println(type);
                type = 1;
                topic_co2 = ((uint16_t)pack->data[2])<<8 | (uint16_t)pack->data[3];
                USB.print("topic_co2: ");
                USB.println(topic_co2);
             }
             else
             {
                 USB.print("regack with wrong return code:");
                 USB.println(pack->data[2],HEX);
             }
         }
       }
     }
   }
   print_packet(pack);
   free(pack);
   USB.println("REGACK received");
   
   
   //check if topic_ids are in EEPROM & write them there if they aren't
   if (written == 0)
   {
   USB.println("writing EEPROM");
   Utils.writeEEPROM(ee_addr_temp, topic_temp);
   Utils.writeEEPROM(ee_addr_humid, topic_humid);
   Utils.writeEEPROM(ee_addr_co2, topic_co2); 
   Utils.writeEEPROM(ee_addr_will, topic_will);   
   }
disconnect:
   //disconnect from network
   send_publish((char *)clientID, strlen(clientID)+1, topic_disconnect);
   send_disconnect();
   USB.println("disconnect send");
   
   //wait for disconnect from GW
  /*pack = NULL;
  type = -1;
  handle = 3;
  empty = 1;
  while ((pack == NULL || empty == 1) || type != 1)
   {
     pack = read_pack();
     if (pack != NULL)
     {
       if (pack->data_length != 0)
       {
         empty = 0;
         type = get_packet_type(pack);
         if (type == DISCONNECT)
         {
              USB.print("packet type: ");
              USB.println(type);
              type = 1;
         }
       }
     }
   }
   print_packet(pack);
   free(pack);
   USB.println("DISCONNECT received");*/
   
   
   //goto sleep
   USB.println("Going to a deep state of sleepiness Zzz...");
   PWR.deepSleep("00:00:00:11",RTC_OFFSET,RTC_ALM1_MODE1,ALL_OFF);
   
   if( intFlag & RTC_INT )
   {
       intFlag &= ~(RTC_INT); // Clear flag
       USB.ON();
       USB.println("RTC interrupt caught! Waking up :( ");
   }  
   reconnect(); 
   
   //publish connected
   send_publish((char *)str_temp, strlen(clientID)+1, topic_will);
   USB.println("publish send...");
   
   //PUBLISH to GW 
   meting *m = (meting *)meet_sensoren();
   
   //publish temp
   send_publish(round((m->temp)*10),topic_temp);
   USB.println("publish send");
   //publish humidity
   send_publish(round((m->humid)*10),topic_humid);
   USB.println("publish send");

   //publish co2
   send_publish(round((m->co2)*10),topic_co2);
   USB.println("publish send");

   //free the metingen package
   free(m); 
   
   goto disconnect;
   //delay(300000);
   
}

void print_meting (void *met)
{
  meting *m = (meting *)met;
  USB.print("Temperature: ");
  USB.print(m->temp);
  USB.println("Â°C");

  USB.print("Humidity: ");
  USB.print(m->humid);
  USB.println("%RH");

  USB.print("CO2: ");
  USB.print(m->co2);
  USB.println("ppm");

}


void send_register(char *topic_name)
{
  msg *reg = (msg *)malloc(sizeof(msg));
  if (reg == NULL)
  {
        USB.println("Out of memory");
        reg = (msg *)malloc(sizeof(msg));
  }
  reg->start = (const char *)malloc(6*sizeof(char) + strlen(topic_name) + 1);
  if (reg->start == NULL)
  {
        USB.println("Out of memory");
        reg->start = (const char *)malloc(6*sizeof(char) + strlen(topic_name) + 1);

  }
  reg->cursor = (char *)(reg->start);
  reg->length = 6*sizeof(char) + strlen(topic_name) + 1;
  reg->msg_type = 0x0A;

  *(reg->cursor) = (uint8_t)(reg->length);
  reg->cursor++;
  *(reg->cursor) = reg->msg_type;
  reg->cursor++;
  *(reg->cursor) = 0x00;
  reg->cursor++;
  *(reg->cursor) = 0x00;
  reg->cursor++;
  *(reg->cursor) = 0x00;
  reg->cursor++;
  *(reg->cursor) = 0x00;
  //*((uint16_t *)(reg->cursor)) = msg_ID;
  //reg->cursor++;
  reg->cursor++;
  strcpy(reg->cursor,topic_name);
  //reg->cursor = topic_name;

  send_packet((char *)reg->start, reg->length);

  free((void *)reg->start);
  free(reg);
}

void send_publish (uint32_t value, int topicID)
{

  msg *publish = (msg *)malloc(sizeof(msg));
  if (publish == NULL)
  {
        USB.println("out of memory");
        msg *publish = (msg *)malloc(sizeof(msg));
        
  }
  publish->start = (const char *)malloc(sizeof(char)*7 + sizeof(double));
  if (publish->start == NULL)
  {
        USB.println("out of memory");
        publish->start = (const char *)malloc(sizeof(char)*7 + sizeof(double));

  }
  publish->cursor = (char *)publish->start;
  publish->length = 7*sizeof(char) + sizeof(double);
  publish->msg_type = 0x0C;
  *(publish->cursor) = (uint8_t)(publish->length);
  publish->cursor++;
  *(publish->cursor) = publish->msg_type;
  publish->cursor++;
  *(publish->cursor) = 0x02;// Flags: DUP QOS Retain Will Clean Session TopicIdType set will prompt request
  publish->cursor++;
  //publish->cursor++;
  *(publish->cursor) = (uint8_t)(((uint16_t)topicID)>>8);
  publish->cursor++;
  *(publish->cursor) = (uint8_t)(((uint16_t)topicID)&0x00FF);
  publish->cursor++;
  *(publish->cursor) = 0x00;
  publish->cursor++;
  *(publish->cursor) = 0x00;
  publish->cursor++;
  *((double *)(publish->cursor)) = value;

  send_packet((char *)publish->start, publish->length);

  free((char *)publish->start);
  free(publish);
}

//returns <0 if disconnect; >0 if ping & ==0 if non of the foregoing
char handle_ping_and_disconnect (packetXBee *pack)
{
  if (get_packet_type(pack) == PINGREQ)
  {
    send_pingresp();
    return  1;
  }
  else if (get_packet_type(pack) == DISCONNECT)
  {
    char recon = try_reconnect();
    if (recon == 1)
      return 0;
    return -1;
  }
  else 
    return 0;

}

void send_publish (char *value, int value_size,int topicID)
{

  msg *publish = (msg *)malloc(sizeof(msg));
  if (publish == NULL)
  {
        USB.println("out of memory");
        msg *publish = (msg *)malloc(sizeof(msg));
        
  }
  publish->start = (const char *)malloc(sizeof(char)*7 + value_size);
  if (publish->start)
  {
        USB.println("out of memory");
        publish->start = (const char *)malloc(sizeof(char)*7 + value_size);
  }
  publish->cursor = (char *)publish->start;
  publish->length = 7*sizeof(char) + value_size;
  publish->msg_type = 0x0C;
  *(publish->cursor) = (uint8_t)(publish->length);
  publish->cursor++;
  *(publish->cursor) = publish->msg_type;
  publish->cursor++;
  *(publish->cursor) = 0x02;// Flags: DUP QOS Retain Will Clean Session TopicIdType set will prompt request
  publish->cursor++;
  //publish->cursor++;
  *(publish->cursor) = (uint8_t)(((uint16_t)topicID)>>8);
  publish->cursor++;
  *(publish->cursor) = (uint8_t)(((uint16_t)topicID)&0x00FF);
  publish->cursor++;
  *(publish->cursor) = 0x00;
  publish->cursor++;
  *(publish->cursor) = 0x00;
  publish->cursor++;
  strcpy(publish->cursor,value);
  //publish->cursor = value;

  send_packet((char *)publish->start, publish->length);

  free((char *)publish->start);
  free(publish);
}



char try_reconnect()
{
  USB.println("trying to reconnect");
  msg *connect = (msg *)malloc(sizeof(msg));
  if (connect == NULL)
  {
        USB.println("out of memory");
        msg *connect = (msg *)malloc(sizeof(msg));  
  }
  connect->start = (const char *)malloc(6*sizeof(char) + strlen(clientID) + 1);
  if (connect->start == NULL)
  {
        USB.println("out of memory");
        connect->start = (const char *)malloc(6*sizeof(char) + strlen(clientID) + 1);
  }
  connect->cursor = (char *)connect->start;
  connect->length = 6 + strlen(clientID) + 1;
  connect->msg_type = 0x04;
  *(connect->cursor) = connect->length;
  connect->cursor++;
  *(connect->cursor) = connect->msg_type;
  connect->cursor++;
  *(connect->cursor) = 0x2C; // Flags: DUP QOS Retain Will Clean Session TopicIdType set will prompt request
  connect->cursor++;
  *(connect->cursor) = 0x01;//protocolID
  connect->cursor++;
  *(connect->cursor) = 60;//seconds for the keep alive timer
  connect->cursor++;
  strcpy(connect->cursor, clientID);
  //connect->cursor = (char *)clientID;

  send_packet((char *)(connect->start), connect->length);
  RTC.setAlarm1("00:00:00:30",RTC_OFFSET,RTC_ALM1_MODE2);

  free((void *)connect->start);
  free(connect);

  packetXBee *pack = NULL;
  int type = -1, empty;
  pack = NULL;
  type = -1;
  empty = 1;
  while ((pack == NULL || empty == 1) || type != 1)
   {
     pack = read_pack();
     if (pack != NULL)
     {
       if (pack->data_length != 0)
       {
         empty = 0;
         type = get_packet_type(pack);
         if (type == CONNACK)
         {
              USB.print("packet type: ");
              USB.println(type);
              type = 1;
         }
       }
     }
   }
   print_packet(pack);
   free(pack);
   return 0;

}

void send_pingresp()
{
  msg *pingresp = (msg *)malloc(sizeof(msg));
  if (pingresp == NULL)
  {
        USB.println("out of memory");
        msg *pingresp = (msg *)malloc(sizeof(msg));
  }
  pingresp->start = (const char *)malloc(sizeof(char)*2);
  if (pingresp->start == NULL)
  {
        USB.println("out of memory");
        pingresp->start = (const char *)malloc(sizeof(char)*2);

  }
  pingresp->cursor = (char *)pingresp->start;
  pingresp->length =  sizeof(char)*2;
  *(pingresp->cursor) = (char)pingresp->length;
  pingresp->cursor++;
  *(pingresp->cursor) = pingresp->msg_type;

  send_packet((char *)pingresp->start, pingresp->length);

  free((void *)pingresp->start);
  free(pingresp);
}

void send_pingreq()
{
  msg *pingreq = (msg *)malloc(sizeof(msg));
  if (pingreq == NULL)
  {
        USB.println("out of memory");
        msg *pingreq = (msg *)malloc(sizeof(msg));
        
  }
  pingreq->start = (const char *)malloc(sizeof(char)*2 + strlen(clientID) + 1);
  if (pingreq->start == NULL)
  {
        USB.println("out of memory");
        pingreq->start = (const char *)malloc(sizeof(char)*2 + strlen(clientID) + 1);

  }
  pingreq->cursor = (char *)pingreq->cursor;
  pingreq->length = sizeof(char)*2 + strlen(clientID) + 1;
  pingreq->msg_type = 0x16;  
  *(pingreq->cursor) = pingreq->length;
  pingreq->cursor++;
  *(pingreq->cursor) = pingreq->msg_type;
  pingreq->cursor++;
  strcpy(pingreq->cursor, clientID);
  //pingreq->cursor = (char *)clientID;

  send_packet((char *)pingreq->start, pingreq->length);

  free((void *)pingreq->start);
  free(pingreq);
}


void *meet_sensoren ()
{
  meting *m = (meting *)malloc (sizeof(meting));

  SensorGasv20.ON();
  SensorGasv20.configureSensor(SENS_CO2, 101);
  SensorGasv20.setSensorMode(SENS_ON, SENS_TEMPERATURE);
  SensorGasv20.setSensorMode(SENS_ON, SENS_HUMIDITY);
  SensorGasv20.setSensorMode(SENS_ON, SENS_CO2);

  delay(10); 

  for(int i = 0;i<80; i++)
  {
    // Read the sensors
    m->temp += SensorGasv20.readValue(SENS_TEMPERATURE);
    m->humid += SensorGasv20.readValue(SENS_HUMIDITY);
    m->co2 = SensorGasv20.readValue(SENS_CO2);

  }
  m->temp /= 80;
  m->temp -= 6;
  m->humid /= 80;
  delay(20000);
  m->co2 = SensorGasv20.readValue(SENS_CO2);
  m->co2 = (m->co2*1000+156.83)/61/84;
  m->co2 = pow(10, m->co2)*100;
  // Turn off the sensor board
  SensorGasv20.OFF();

  return m;
}

void send_disconnect()
{

  msg *disconnect = (msg *)malloc(sizeof(msg));
  if (disconnect == NULL)
  {
        USB.println("out of memory");
        msg *disconnect = (msg *)malloc(sizeof(msg));    
  }
  disconnect->start = (char *)malloc(sizeof(char)*4);
  if (disconnect->start == NULL)
  {
        USB.println("out of memory");
        disconnect->start = (char *)malloc(sizeof(char)*4);
  }
  disconnect->length = 4*sizeof(char);
  disconnect->msg_type = 0x18;
  disconnect->cursor = (char *)disconnect->start;
  *(disconnect->cursor) = (uint8_t)(disconnect->length);
  disconnect->cursor++;
  *(disconnect->cursor) = disconnect->msg_type;
  disconnect->cursor++;
  *(disconnect->cursor) = 0;
  disconnect->cursor++;
  *(disconnect->cursor) = 60; // go 60seconds offline;

  send_packet((char *)disconnect->start, disconnect->length);

  free((void *)disconnect->start);
  free(disconnect);
}

void send_willmsg()
{
  msg *willmsg = (msg *)malloc(sizeof(msg));
  if(willmsg == NULL)
  {
        USB.println("out of memory");
        willmsg == (msg *)malloc(sizeof(msg));
  }
  willmsg->start = (const char *)malloc(sizeof(char)*2 + strlen(MESSAGE) + 1);// +1 for string termination character
  if (willmsg->start == NULL)
  {
        USB.println("out of memory");
        willmsg->start = (const char *)malloc(sizeof(char)*2 + strlen(MESSAGE) + 1);// +1 for string termination character
  }
  willmsg->length = sizeof(char)*2 + strlen(MESSAGE) +1;
  willmsg->msg_type = 0x09;
  willmsg->cursor = (char *)(willmsg->start);
  *(willmsg->cursor) = (uint8_t)willmsg->length;
  willmsg->cursor++;
  *(willmsg->cursor) = willmsg->msg_type;
  willmsg->cursor++;
  strcpy(willmsg->cursor,MESSAGE);
  //willmsg->cursor = (char *)MESSAGE;

  send_packet((char *)willmsg->start, willmsg->length);

  free((void *)(willmsg->start));
  free(willmsg);

}
void send_willtopic()
{
  msg *willtopic = (msg *)malloc(sizeof(msg));
  if (willtopic == NULL)
  {
          USB.println("out of memory");
          willtopic = (msg *)malloc(sizeof(msg));
  }
  willtopic->start = (const char *)malloc(3*sizeof(char) + strlen(TOPIC) + 1); //plus one for string termination character
  if (willtopic->start == NULL)
  {
          USB.println("out of memory");
          willtopic->start = (const char *)malloc(3*sizeof(char) + strlen(TOPIC) + 1); //plus one for string termination character
  }
  willtopic->cursor = (char *)(willtopic->start);
  willtopic->length = sizeof(char)*3 + strlen(TOPIC) + 1; //+1 for string termination character
  willtopic->msg_type = 0x07;
  *(willtopic->cursor) = (uint8_t)(willtopic->length);
  willtopic->cursor++;
  *(willtopic->cursor) = willtopic->msg_type;
  willtopic->cursor++;
  *(willtopic->cursor) = 0x00; // Flags: DUP QOS Retain Will Clean Session TopicIdType set will prompt request
  willtopic->cursor++;
  strcpy(willtopic->cursor, TOPIC);
  //(willtopic->cursor) = (char *)TOPIC;

  send_packet((char *)willtopic->start, willtopic->length);

  free((void *)(willtopic->start));
  free(willtopic);
}


void send_connect ()
{

  msg *connect = (msg *)malloc(sizeof(msg));
  if (connect == NULL)
  {
          USB.println("out of memory");
          connect = (msg *)malloc(sizeof(msg));
  }
  connect->start = (const char *)malloc(6*sizeof(char) + strlen(clientID) + 1);
  if (connect->start == NULL)
  {
        USB.println("out of memory");
        connect->start = (const char *)malloc(6*sizeof(char) + strlen(clientID) + 1);
  }
  connect->cursor = (char *)connect->start;
  connect->length = 6 + strlen(clientID) + 1;
  connect->msg_type = 0x04;
  *(connect->cursor) = connect->length;
  connect->cursor++;
  *(connect->cursor) = connect->msg_type;
  connect->cursor++;
  *(connect->cursor) = 0x00; // Flags: DUP QoS Retain Will Clean Session TopicIdType set will prompt request 0000 0000 0x00
  connect->cursor++;
  *(connect->cursor) = 0x01;//protocolID
  connect->cursor++;
  *(connect->cursor) = 0x00;
  connect->cursor++;  
  *((uint16_t *)(connect->cursor)) = 30;//seconds for the keep alive timer
  connect->cursor++;
  strcpy(connect->cursor, clientID);
  //connect->cursor = (char *)clientID;

  send_packet((char *)(connect->start), connect->length);
  RTC.setAlarm1("00:00:00:30",RTC_OFFSET,RTC_ALM1_MODE2);

  free((void *)connect->start);
  free(connect);
}

void send_packet(char *data, int length)
{
  // 3.1. create new frame
  frame.createFrame(BINARY, "WASPMOTE_XBEE");  


  // 3.2. add frame fields
  frame.addSensor(SENSOR_STR, data, (uint16_t)length);



  // set parameters to packet:
  packet=(packetXBee *) calloc(1,sizeof(packetXBee)); // Memory allocation
  packet->mode=UNICAST; // Choose transmission mode: UNICAST or BROADCAST 

    // 4.2. set destination XBee parameters to packet
  xbeeZB.setDestinationParams( packet, MAC_ADERESS_GW, frame.buffer, frame.length );  

  // 4.3. send XBee packet
  xbeeZB.sendXBee(packet);

  //print packet
  USB.println("Sending: ");
  print_packet(packet);
  USB.println("");
  // 4.4. check TX flag      
  if( xbeeZB.error_TX == 0) 
  {
    USB.println(F("Packet send!"));
  }
  else 
  {
    USB.println(F("error while sending"));
    USB.println(F("retransmitting..."));
    send_packet(data, length);
  }

  // 4.5. free variables
  free(packet);
  packet = NULL;

  // 4.6. wait for five seconds
}

void print_packet (packetXBee *_packet)
{
  USB.print("num_bytes: ");
  USB.println((int)_packet->data_length);

  USB.print("Source MAC: ");
  int i = 0;
  for (i = 0; i < 4; i++)
    USB.print(_packet->macSH[i],HEX);
  for (i = 0; i < 4; i++)
    USB.print(_packet->macSL[i],HEX);   

  USB.print("\nDestination MAC: ");
  for (i = 0; i < 4; i++) 
    USB.print(_packet->macDH[i],HEX);
  for (i = 0; i < 4; i++)
    USB.print(_packet->macDL[i],HEX);  

  char *str = (char *)_packet->data;
  int size = (int)_packet->data_length;
  USB.print("\nContent_hex: ");

  for (i = 0; i<size; i++)
  {
    USB.print(*str,HEX);
    str++;
  }
  /*char *tekst = (char *)malloc (_packet->data_length + 1);
  for (i=0; i<_packet->data_length; i++)
  {
        tekst[i] = str[i];
  }
  tekst[_packet->data_length] = '\0';
  
  USB.print("\nContent_char: ");
  USB.println(tekst);

  free(tekst);*/
  
  USB.println("\n");

}

void receive_packets()
{
  if( xbeeZB.available() > 0 ) 
  {
    xbeeZB.treatData(); 

    if( !xbeeZB.error_RX ) 
    {
      while( xbeeZB.pos>0 )
      {             
        USB.print( F("Data: ") );                    
        for(int i=0;i<xbeeZB.packet_finished[xbeeZB.pos-1]->data_length;i++)          
        { 
          USB.print(xbeeZB.packet_finished[xbeeZB.pos-1]->data[i],BYTE);          
        }
        USB.println("");

        free(xbeeZB.packet_finished[xbeeZB.pos-1]); 

        xbeeZB.packet_finished[xbeeZB.pos-1]= NULL; 

        //Decrement the received packet counter
        xbeeZB.pos--; 
      }
    }
  }
  else
  {
    USB.println("no packets received");
  }
}

//remember to free it!!!!!
packetXBee *read_pack()
{
  //////////////////////////
  // 3. receive packets 
  //////////////////////////

  // check available data in RX buffer
  if( xbeeZB.available()>0 ) 
  {
    // read a packet when XBee has noticed it to us
    xbeeZB.treatData(); 

    // check RX flag after 'treatData'
    if( !xbeeZB.error_RX ) 
    {
      // read available packets
      if( xbeeZB.pos>0 )
      {
        // Available information in 'xbeeZB.packet_finished' structure
        // HERE it should be introduced the User's packet treatment        
        // For example: show DATA field:

        packetXBee *_packet = (packetXBee *)malloc(sizeof(packetXBee));
        memcpy(_packet, xbeeZB.packet_finished[xbeeZB.pos-1], sizeof(packetXBee));

        //free memory
        free(xbeeZB.packet_finished[xbeeZB.pos-1]); 

        //free pointer
        xbeeZB.packet_finished[xbeeZB.pos-1]=NULL; 
        //Decrement the received packet counter

        xbeeZB.pos--; 
        /*if (_packet->macSH[0] == 0x00 && _packet->macSH[1] == 0x13 && _packet->macSH[2] == 0xA2 && _packet->macSH[3] == 0x00 && _packet->macSL[0]==40 && _packet->macSL[1]== 79 && _packet->macSL[2]== 0x5B && _packet->macSL[2]== 0xCE)
         {
         USB.println("Before returning itlooked like this => ");
         print_packet(_packet);
         return _packet;
         }
         else*/
        return _packet;
      }
      else
      {
        return NULL;
      }
    }
    else
    {
      return NULL;
    }
  }
  else
  {
    return NULL;
  }
}

int get_packet_type(packetXBee *pack)
{
  if (pack == NULL)
  {
    USB.println("received a null packet in get packet type");  
    return -1;
  }
  if (pack->data_length == 0)
  {
    USB.println("received an empty packet in get packet type");
    return -1;
  }   
  char data[pack->data_length];
  memcpy(data, pack->data, pack->data_length);

  uint8_t header_length;
  uint16_t msg_length;

  //check if 2 or 4 byte header
  /*if(data[0] == 0x01)//4 byte
   {
   header_length = 4;
   msg_length = data[2] + data[1]<<7;
   }
   else
   {
   header_length = 2;
   msg_length = data[0];
   }*/
  header_length = 2;
  msg_length = pack->data[0];
  int msg_type = pack->data[1];
  //USB.print("packet_type:");
  //USB.println(msg_type);
  /*switch (msg_type)
  {
  case 0x00:
    packet_type = ADVERTISE;
  case 0x01:
    packet_type = SEARCHGW;
  case GWINFO:
    packet_type = GWINFO;
  case 0x04:
    packet_type = CONNECT;
  case 0x05:
    packet_type = CONNACK;
  case 0x06:
    packet_type = WILLTOPICREQ;
  case 0x07:
    packet_type = WILLTOPIC;
  case 0x08:
    packet_type = WILLMSGREQ;
  case 0x09:
    packet_type = WILLMSG;
  case 0x0A:
    packet_type = REGISTER;
  case 0x0B:
    packet_type = REGACK;
  case 0x0C:
    packet_type = PUBLISH;
  case 0x0D:
    packet_type = PUBACK;
  case 0x0E:
    packet_type = PUBCOMP;
  case 0x0F:
    packet_type = PUBREC;
  case 0x10:
    packet_type = PUBREL;
  case 0x12:
    packet_type = SUBSCRIBE;
  case 0x13:
    packet_type = SUBACK;
  case 0x14:
    packet_type = UNSUBSCRIBE;
  case 0x15:
    packet_type = UNSUBACK;
  case 0x16:
    packet_type = PINGREQ;
  case 0x17:
    packet_type = PINGRESP;
  case 0x18:
    packet_type = DISCONNECT;
  case 0x1A:
    packet_type = WILLTOPICUPD;
  case 0x1B:
    packet_type = WILLTOPICRESP;
  case 0x1C:
    packet_type = WILLMSGUPD;
  case 0x1D:
    packet_type = WILLMSGRESP;
  }*/
  //USB.print("packet type: ");
  //USB.println(msg_type);
  return msg_type;
}
void *get_msg_from_type(int type)
{

  switch (type)
  {
  case ADVERTISE:

  case SEARCHGW:

  case GWINFO:

  case CONNECT:

  case CONNACK:

  case WILLTOPICREQ:

  case WILLTOPIC:

  case WILLMSGREQ:

  case WILLMSG:

  case REGISTER:

  case REGACK:

  case PUBLISH:

  case PUBACK:

  case PUBCOMP:

  case PUBREC:

  case PUBREL:

  case SUBSCRIBE://

  case SUBACK:

  case UNSUBSCRIBE:

  case UNSUBACK:

  case PINGREQ:

  case PINGRESP:

  case DISCONNECT:

  case WILLTOPICUPD:

  case WILLTOPICRESP:

  case WILLMSGUPD:

  case WILLMSGRESP:
    delay(2);

  }  

}

void send_searchgw()
{
  msg *search_gw;
  search_gw = (msg *)malloc (sizeof(msg));
  if (search_gw == NULL)
  {
        search_gw = (msg *)malloc (sizeof(msg));
        USB.println("out of memory");
  }
  search_gw->start = (const char *)malloc(3*sizeof(char)); 
  if (search_gw->start == NULL)
  {
        USB.println("out of memory");
        search_gw->start = (const char *)malloc(3*sizeof(char));   
  } 
  search_gw->cursor = (char *)search_gw->start;
  search_gw->length = 3;
  search_gw->msg_type = 0x01;
  *(search_gw->cursor) = (char)search_gw->length;
  search_gw->cursor++;
  *(search_gw->cursor) = search_gw->msg_type;
  search_gw->cursor++;
  *(search_gw->cursor) = 1;

  send_packet((char *)search_gw->start, search_gw->length);

  free((void *)search_gw->start);
  free(search_gw);

}

void reconnect()
{
  USB.ON();

  RTC.ON();

  // init XBee
  xbeeZB.ON();

  delay(500);

  xbeeZB.setPAN(PANID);

  // 1.2. check AT command flag
  if( xbeeZB.error_AT == 0 ) 
  {
    USB.println(F("PANID set OK"));
  }
  else 
  {
    USB.println(F("Error while setting PANID")); 
  }

  // 1.3. set all possible channels to scan 
  // channels from 0x0B to 0x18 (0x19 and 0x1A are excluded)
  /* Range:[0x0 to 0x3FFF]
   * Channels are scpedified as a bitmap where depending on 
   * the bit a channel is selected --> Bit (Channel): 
   *  0 (0x0B)  4 (0x0F)  8 (0x13)   12 (0x17)
   *  1 (0x0C)  5 (0x10)  9 (0x14)   13 (0x18)
   *  2 (0x0D)  6 (0x11)  10 (0x15)  
   *  3 (0x0E)  7 (0x12)	 11 (0x16)    */
  xbeeZB.setScanningChannels(0x3F, 0xFF);

  // 1.4. check AT command flag  
  if( xbeeZB.error_AT == 0 )
  {
    USB.println(F("scanning channels set OK"));
  }
  else 
  {
    USB.println(F("Error while setting scanning channels")); 
  }

  // 1.5. set channel verification JV=1 in order to make the 
  // XBee module to scan new coordinator
  xbeeZB.setChannelVerification(1);

  // 1.6. check AT command flag    
  if( xbeeZB.error_AT == 0 )
  {
    USB.println(F("verification channel set OK"));
  }
  else 
  {
    USB.println(F("Error while setting verification channel")); 
  }

  // 1.7. write values to XBee memory
  xbeeZB.writeValues();

  // 1.8 reboot XBee module
  xbeeZB.OFF();
  delay(3000); 
  xbeeZB.ON();

  delay(3000);

  /////////////////////////////////////
  // 2. Wait for Association 
  /////////////////////////////////////

  // 2.1. wait for association indication
  xbeeZB.getAssociationIndication();

  while( xbeeZB.associationIndication != 0 )
  { 
    delay(2000);

    // get operating 64-b PAN ID
    xbeeZB.getOperating64PAN();

    USB.print(F("operating 64-b PAN ID: "));
    USB.printHex(xbeeZB.operating64PAN[0]);
    USB.printHex(xbeeZB.operating64PAN[1]);
    USB.printHex(xbeeZB.operating64PAN[2]);
    USB.printHex(xbeeZB.operating64PAN[3]);
    USB.printHex(xbeeZB.operating64PAN[4]);
    USB.printHex(xbeeZB.operating64PAN[5]);
    USB.printHex(xbeeZB.operating64PAN[6]);
    USB.printHex(xbeeZB.operating64PAN[7]);
    USB.println();     

    xbeeZB.getAssociationIndication();
  }


  USB.println(F("\n\nJoined a coordinator!"));
  Utils.blinkLEDs(3000);
  // 2.2. When XBee is associated print all network 
  // parameters unset channel verification JV=0
  xbeeZB.setChannelVerification(0);
  xbeeZB.writeValues();

  // 2.3. get network parameters 
  xbeeZB.getOperating16PAN();
  xbeeZB.getOperating64PAN();
  xbeeZB.getChannel();

  USB.print(F("operating 16-b PAN ID: "));
  USB.printHex(xbeeZB.operating16PAN[0]);
  USB.printHex(xbeeZB.operating16PAN[1]);
  USB.println();

  USB.print(F("operating 64-b PAN ID: "));
  USB.printHex(xbeeZB.operating64PAN[0]);
  USB.printHex(xbeeZB.operating64PAN[1]);
  USB.printHex(xbeeZB.operating64PAN[2]);
  USB.printHex(xbeeZB.operating64PAN[3]);
  USB.printHex(xbeeZB.operating64PAN[4]);
  USB.printHex(xbeeZB.operating64PAN[5]);
  USB.printHex(xbeeZB.operating64PAN[6]);
  USB.printHex(xbeeZB.operating64PAN[7]);
  USB.println();

  USB.print(F("channel: "));
  USB.printHex(xbeeZB.channel);
  USB.println();

}



