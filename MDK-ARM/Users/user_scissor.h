#ifndef _USER_SCISSOR_H
#define _USER_SCISSOR_H

#include "main.h"

typedef struct
{
	int32_t pos_open[2];
	int32_t pos_close[2];
	int32_t pos_dif;
	
	uint8_t close_flag[2];
	
	uint8_t control;//0:open; 1:close
	
	int32_t spe;
	int32_t cur_max;
	
	
} SCISSOR_TYPE;
extern SCISSOR_TYPE Scissor;;


void User_scissorInit(void);
uint8_t User_scissorClose(void);
uint8_t User_scissorOpen(void);
uint8_t User_scissorControl(void);
	
#endif
