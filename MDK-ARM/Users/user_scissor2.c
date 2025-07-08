#include "user_scissor2.h"

Scissor2_Type Scissor2[2];
int sci_cur=0;//控制电机加速度
int sci_control=0;//控制电机开合
void User_scissor2_Init()
{
	Scissor2[0].spe_min=0;
	Scissor2[0].pos_dif=0;
	Scissor2[0].pos_close=0;
	Scissor2[0].pos_open=0;
	Scissor2[0].close_flag=0;
	Scissor2[0].open_flag=0;
	Scissor2[1].spe_min=0;
	Scissor2[1].pos_dif=0;
	Scissor2[1].pos_close=0;
	Scissor2[1].pos_open=0;
	Scissor2[1].close_flag=0;
	Scissor2[1].open_flag=0;
}

uint8_t User_scissor2Close()
{
	static uint8_t spe_count1=0;
	static uint8_t spe_count2=0;
	
	if(User_AbsInt(MOTOR_3508[4].spe_now)>Scissor2[0].spe_min)
	spe_count1=0;
	else 
	{
		if(spe_count1++>50)
		{
			Scissor2[0].pos_close=MOTOR_3508[4].pos_now;
			Scissor2[0].pos_open=MOTOR_3508[4].pos_now-Scissor2[0].pos_dif;
			Scissor2[0].close_flag=1;
		}
	}
	if(User_AbsInt(MOTOR_3508[5].spe_now)>Scissor2[1].spe_min)
	spe_count2=0;
	else 
	{
		if(spe_count2++>50)
		{
			Scissor2[1].pos_close=MOTOR_3508[5].pos_now;
			Scissor2[1].pos_open=MOTOR_3508[5].pos_now-Scissor2[1].pos_dif;
			Scissor2[1].close_flag=1;
		}
	}
	if((Scissor2[0].close_flag==1)&&(Scissor2[1].close_flag==1))
	{
		CAN_SendCurGroup3508(0x1FF,0,0,0,0);
		Scissor2[0].open_flag=0;
		Scissor2[1].open_flag=0;
		return 1;
	}
	else if((Scissor2[0].close_flag==0)&&(Scissor2[1].close_flag==0))
		CAN_SendCurGroup3508(0x1FF,sci_cur,sci_cur,0,0);
	else if((Scissor2[0].close_flag==1)&&(Scissor2[1].close_flag==0))
		CAN_SendCurGroup3508(0x1FF,0,sci_cur,0,0);
	else if((Scissor2[0].close_flag==0)&&(Scissor2[1].close_flag==1))
		CAN_SendCurGroup3508(0x1FF,sci_cur,0,0,0);
		
	return 0;
}

uint8_t User_scissor2Open()
{
	if(User_AbsInt(MOTOR_3508[5].pos_now-Scissor2[0].pos_open)<100)
		Scissor2[0].open_flag=1;
	if(User_AbsInt(MOTOR_3508[6].pos_now-Scissor2[1].pos_open)<100)
		Scissor2[1].open_flag=1;
	if((Scissor2[0].open_flag==1)&&(Scissor2[1].open_flag==1))
	{
		CAN_SendCurGroup3508(0x1FF,0,0,0,0);
		Scissor2[0].close_flag=0;
		Scissor2[1].close_flag=0;
		return 1;
	}
	else if((Scissor2[0].open_flag==0)&&(Scissor2[1].open_flag==0))
		CAN_SendCurGroup3508(0x1FF,-sci_cur,-sci_cur,0,0);
	else if((Scissor2[0].open_flag==1)&&(Scissor2[1].open_flag==0))
		CAN_SendCurGroup3508(0x1FF,0,-sci_cur,0,0);
	else if((Scissor2[0].open_flag==0)&&(Scissor2[1].open_flag==1))
		CAN_SendCurGroup3508(0x1FF,-sci_cur,0,0,0);
	return 0;
}

uint8_t User_scissorControl2(void)
{
	static uint8_t Init_flag = 1;//0代表剪刀机构已经历过初始化过程，可正常使用。
	
	switch(Init_flag)
	{
		case 0://执行剪刀命令并返回值
		{
			if(sci_control== 0)
			{
				if(User_scissor2Open())
					return 1;
			}
			else if(sci_control == 1)
			{
				if(User_scissor2Close())
					return 1;
			}
		}break;
		case 1://初始化过程1
		{
			if(User_scissor2Close())
				Init_flag = 2;
		}break;
		case 2://初始化过程2
		{
			if(User_scissor2Open())
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

		
			