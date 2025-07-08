#ifndef _user_key_h
#define _user_key_h

#include "main.h"

#define _KEY_1_LEV 		HAL_GPIO_ReadPin(KEY_1_GPIO_Port, KEY_1_Pin)
#define _KEY_2_LEV 		HAL_GPIO_ReadPin(KEY_2_GPIO_Port, KEY_2_Pin)
#define _KEY_H_LEV 		HAL_GPIO_ReadPin(KEY_H_GPIO_Port, KEY_H_Pin)

typedef enum
{
	NONE,
	SHORT,
	MID,
	LONG
}KEY_TRIG_TYPE;


typedef struct
{
	KEY_TRIG_TYPE trig;
	uint8_t level;
	uint32_t press_time;
}KEY_TYPEDEF;
extern KEY_TYPEDEF KEY[3];



void User_KeyInit(void);
void User_KeyCheck(void);

void User_Key1_Deal(void);
void User_Key2_Deal(void);
void User_KeyH_Deal(void);

#endif










