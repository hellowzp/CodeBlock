#ifndef COORDINATOR_H
#define COORDINATOR_H

#include "mymqtt.h"

/***********************************
            Zigbee
************************************/
#define ZIGBEE_START_DELIMITER 0x7E

#define AT_COMMAND             0x08
#define AT_COMMAND_QUEUE       0x09
#define TRANSMIT_REQUEST       0x10
#define REMOTE_AT              0x17
#define CREATE_SOURCE_ROUTE    0x21
#define REG_JOINING_DEVICE     0x24
#define AT_RESPONSR            0x88
#define MODEM_STATUS           0x8A
#define TX_STATUS              0x8B
#define RECEIVE_PACKET         0x90
#define EXPLICIT_RX_INDICATOR  0x91
#define REMOTE_RESPONSE        0x97

#define PARAMETER_ERROR(fun) do{ printf("%s() error: invalid parameter...\n",fun); } while(0)
                                
typedef unsigned char Byte;
typedef char* String;


Byte xbee_set_checksum(String xbee_data, int len);
int xbee_txReq_frame_assemble(String mac_addr,String ntwk_addr,char radius,char option,String RFData,int len);
//read the data section of the zigbee frame and call the corresponding sub-read function according to the frame type
mqtt_ptr_t xbee_data_read(String xbee_data,int len);
//read ZigBee Receive Packet(0x90) data section, return the desired data structure pointer 
mqtt_ptr_t xbee_rx_data_read(String xbee_data,int len);
mqtt_ptr_t xbee_exrx_data_read(String xbee_data,int len);
int xbee_remoteAT_frame_assemble(String mac_addr,String ntwk_addr,char option,String cmd, char para);
int xbee_localAT_frame_assemble(String cmd,char value);
int xbee_mqtt_wframe_assenble(mqtt_ptr_t mqtt);
int xbee_frame_escape(String buf,int len);  //length of buffer(outbuf)
int xbee_frame_descape(String buf,int len); //length of buffer(inbuf)


/***********************************
            ttyUSB
************************************/

#define MAX_FRAME_SIZE   200
#define MODEM_DEVICE "/dev/ttyUSB0"

int tty_open(char* tty_name);
void tty_serial_config(int fds);
void tty_serial_init();
int tty_serial_read(int fds,String buf);
int tty_serial_write(int fds,String buf,int len);


/***********************************
            mystring
************************************/

char* str_get_between_index( char* str, int len, int bn, int en);

#endif
