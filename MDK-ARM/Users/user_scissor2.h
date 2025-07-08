#ifndef _USER_SCISSOR2_H
#define _USER_SCISSOR2_H

#include "main.h"
#include "user_can_motor.h"
#include "user_math.h"

typedef struct 
{
	int pos_dif;
	int spe_min;
	int close_flag;
	int open_flag;
	int pos_close;
	int pos_open;
}Scissor2_Type;//定义电机必要速度

extern int sci_cur;//控制电机加速度
extern int sci_control;//控制电机开合
uint8_t User_scissor2Close();//电机关闭函数
uint8_t User_scissor2Open();//电机打开函数
#endif