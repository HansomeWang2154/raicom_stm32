
#ifndef __USER_TOFSENSE_F_H
#define __USER_TOFSENSE_F_H

#include "main.h"
#include "usart.h"

#define tofsense_num 5 //С��8������8���޸Ĳ��ִ��룩
#define tofsense_huart huart2
#define tofsense_buf_len 20

typedef struct
{
	uint8_t rxbuf[tofsense_buf_len];
	uint8_t txbuf[tofsense_buf_len];
	
	uint8_t rxdata_len;
	uint8_t txdata_len;
	
	uint32_t dis[5];//��λ:mm
	uint8_t dis_update_flag;//�����������Ƿ����(1λ����һ��������0:δ����;1:�Ѹ���)
	
} TOFSENSE_TYPEDEF;

void User_TOFSenseInit(void);
void User_TOFSenseIRQHandler(void);
//void User_TOFSenseDataParas(void);
//void User_TOFSense_Get(uint8_t id);
//uint8_t User_TOFSense_GetAll(void);

#endif
