#ifndef _USER_IMU_H
#define _USER_IMU_H

#include "main.h"

#define USART_IMU_TX_SIZE	20
#define USART_IMU_RX_SIZE	100



typedef struct
{
	uint8_t usart_tx_buf[USART_IMU_TX_SIZE];
	uint8_t usart_rx_buf[USART_IMU_RX_SIZE];				
	uint8_t rx_data_len;
	
	float accx;
	float accy;
	float accz;
	
	float gyrox;
	float gyroy;
	float gyroz;
	
	float angx;
	float angy;
	float angz;
	float angz_last;
	
	float gyroz_bias;
	float angz_derta_bias;
	float gyroz_offset;
	float angz_derta_offset;
	
	float temper;
}USART_IMU_TYPEDEF;
extern USART_IMU_TYPEDEF IMU1;

void User_ImuInit(void);
void User_Imu1IRQHandler(void);
void User_Imu1DataParas(void);


#endif










