
#ifndef __USER_TOFSENSE_F_H
#define __USER_TOFSENSE_F_H

#include "main.h"
#include "usart.h"

#define tofsense_num 5 //小于8（大于8需修改部分代码）
#define tofsense_huart huart2
#define tofsense_buf_len 20

typedef struct
{
	uint8_t rxbuf[tofsense_buf_len];
	uint8_t txbuf[tofsense_buf_len];
	
	uint8_t rxdata_len;
	uint8_t txdata_len;
	
	uint32_t dis[5];//单位:mm
	uint8_t dis_update_flag;//传感器数据是否更新(1位代表一个传感器0:未更新;1:已更新)
	
} TOFSENSE_TYPEDEF;

void User_TOFSenseInit(void);
void User_TOFSenseIRQHandler(void);
//void User_TOFSenseDataParas(void);
//void User_TOFSense_Get(uint8_t id);
//uint8_t User_TOFSense_GetAll(void);

#endif
