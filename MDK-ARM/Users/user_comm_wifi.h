#ifndef _USER_COMM_WIFI_H
#define _USER_COMM_WIFI_H


#include "main.h"


#define USART_WIFI_TX_SIZE	50
#define USART_WIFI_RX_SIZE	100


typedef struct
{
	int8_t left_x;
	int8_t left_y;
	int8_t right_x;
	int8_t right_y;
	uint8_t button;
	uint8_t text[20];
}HANDLE_MES_TYPEDEF;

typedef struct
{
	char usart_tx_buf[USART_WIFI_TX_SIZE];
	char usart_rx_buf[USART_WIFI_RX_SIZE];				
	uint16_t rx_data_len;
	uint8_t rx_data_seq;
	HANDLE_MES_TYPEDEF handle;
	uint8_t init_ok;
}USART_WIFI_TYPEDEF;

extern USART_WIFI_TYPEDEF WIFI;

extern uint8_t wifi_flag;


void User_CommWifiInit(void);
void User_CommWifiIRQHandler(void);
void User_CommWifiDataParas(void);
void User_CommWifiYaoGan1Get(void);
void User_CommWifiYaoGan2Get(void);
void User_CommWifiTextGet(void);
void User_CommWifiButtonGet(void);
void User_CommWifiSendOut(char* str);
uint8_t User_WifiSendCmd(char *cmd,uint16_t waittime);
void User_CommWifiMasterCreat(void);
void User_CommWifi_Int16Data(int16_t data1,int16_t data2,int16_t data3,int16_t data4,int16_t data5);
void User_CommWifiSendTask(void);
void User_send(void);

#endif










