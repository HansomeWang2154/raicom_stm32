#include "user_scissor.h"
#include "user_can_motor.h"
#include "user_math.h"

SCISSOR_TYPE Scissor;

void User_scissorInit(void)
{
	Scissor.pos_close[0] = 0;
	Scissor.pos_close[1] = 0;
	Scissor.pos_open[0] = 0;
	Scissor.pos_open[1] = 0;
	Scissor.pos_dif = 280000;
	
	Scissor.close_flag[0] = 0;
	Scissor.close_flag[1] = 0;
	
	Scissor.control = 0;
	
	Scissor.spe = 3000;
	Scissor.cur_max = 1000;
}

uint8_t User_scissorClose(void)
{
	static uint8_t cur_count1 = 0;
	static uint8_t cur_count2 = 0;
	
	if(User_AbsInt(CHASS_MOTOR[0].flash.item.cur_now) < Scissor.cur_max)
		cur_count1 = 0;
	else
	{
		if(cur_count1++ > 50)
		{
			Scissor.pos_close[0] = CHASS_MOTOR[0].flash.item.pos_now - 1000;
			Scissor.pos_open[0] = CHASS_MOTOR[0].flash.item.pos_now - Scissor.pos_dif;
			Scissor.close_flag[0] = 1;
		}
	}
	
	if(User_AbsInt(CHASS_MOTOR[1].flash.item.cur_now) < Scissor.cur_max)
		cur_count2 = 0;
	else
	{
		if(cur_count2++ > 50)
		{
			Scissor.pos_close[1] = CHASS_MOTOR[1].flash.item.pos_now - 1000;
			Scissor.pos_open[1] = CHASS_MOTOR[1].flash.item.pos_now - Scissor.pos_dif;
			Scissor.close_flag[1] = 1;
		}
	}
	
	if((Scissor.close_flag[0] == 1) && (Scissor.close_flag[1] == 1))
	{
		CAN_SendSpeGroup(0,0,0,0,0);
		return 1;
	}
	else if((Scissor.close_flag[0] == 0) && (Scissor.close_flag[1] == 0))
		CAN_SendSpeGroup(0,Scissor.spe,Scissor.spe,0,0);
	else if(Scissor.close_flag[0] == 0)
		CAN_SendSpeGroup(0,Scissor.spe,0,0,0);
	else if(Scissor.close_flag[1] == 0)
		CAN_SendSpeGroup(0,0,Scissor.spe,0,0);
	
	return 0;
}

uint8_t User_scissorOpen(void)
{
	if((User_AbsInt(CHASS_MOTOR[0].flash.item.pos_now - Scissor.pos_open[0]) < 100) && 
		(User_AbsInt(CHASS_MOTOR[1].flash.item.pos_now - Scissor.pos_open[1]) < 100))
		return 1;
	else
	{
		Scissor.close_flag[0] = 0;
		Scissor.close_flag[1] = 0;
		CAN_SendPosSingle(1,Scissor.pos_open[0]);
		CAN_SendPosSingle(2,Scissor.pos_open[1]);
		return 0;
	}
}

uint8_t User_scissorControl(void)
{
	static uint8_t Init_flag = 1;//0代表剪刀机构已经历过初始化过程，可正常使用。
	
	switch(Init_flag)
	{
		case 0://执行剪刀命令并返回值
		{
			if(Scissor.control == 0)
			{
				if(User_scissorOpen())
					return 1;
			}
			else if(Scissor.control == 1)
			{
				if(User_scissorClose())
					return 1;
			}
		}break;
		case 1://初始化过程1
		{
			if(User_scissorClose())
				Init_flag = 2;
		}break;
		case 2://初始化过程2
		{
			if(User_scissorOpen())
			{
				Init_flag = 0;
			}
		}break;
		default:
		{
			Init_flag = 1;
		}break;
	}
	return 0;
}
