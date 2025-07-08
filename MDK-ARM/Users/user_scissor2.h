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
}Scissor2_Type;//��������Ҫ�ٶ�

extern int sci_cur;//���Ƶ�����ٶ�
extern int sci_control;//���Ƶ������
uint8_t User_scissor2Close();//����رպ���
uint8_t User_scissor2Open();//����򿪺���
#endif