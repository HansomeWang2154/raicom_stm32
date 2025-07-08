/*********************************************
//按键处理
**********************************************/

#include "user_key.h"
#include "user_led.h" 
#include "user_movebase.h"

KEY_TYPEDEF KEY[3];

/*********************************************
//初始化
**********************************************/
void User_KeyInit(void)
{
	KEY[0].level = 1;
	KEY[0].press_time = 0;
	KEY[0].trig = NONE;
	KEY[1].level = 1;
	KEY[1].press_time = 0;
	KEY[1].trig = NONE;
	KEY[2].level = 1;
	KEY[2].press_time = 0;
	KEY[2].trig = NONE;
}
/*********************************************
//按键检测
**********************************************/
void User_KeyCheck(void)
{
	uint8_t i=0;
	
	KEY[0].level = _KEY_1_LEV;
	KEY[1].level = _KEY_2_LEV; 
	KEY[2].level = _KEY_H_LEV;
	
	for(i=0;i<3;i++)
	{
		if(KEY[i].level == 0)//按下状态为0
		{
			KEY[i].press_time++;	
			BUZ.remain_ms = 30;
		}
		else
		{
			if(KEY[i].press_time <= 5)
			{			
//				KEY[i].trig = NONE;	//不进行重置，因为在有效按压后可能存在抖动重新计数一个很小的时间参数,致使有效按压状态被更新为NONE!!			
			}
			else if((KEY[i].press_time > 5) && (KEY[i].press_time <= 2000))
			{
				KEY[i].trig = SHORT;	
			}
			else if((KEY[i].press_time > 2000) && (KEY[i].press_time <= 5000))
			{
				KEY[i].trig = MID;		
			}
			else if((KEY[i].press_time > 5000))
			{
				KEY[i].trig = LONG;	
			}
			KEY[i].press_time = 0;
		}
	}
}
/*********************************************
//按键1处理
**********************************************/
void User_Key1_Deal(void)
{	
	switch(KEY[0].trig)
	{
		case NONE:;break;
		case SHORT:	
		{
			if(MOVE.motor_en != 0)
				MOVE.motor_en = 0;
			else
				MOVE.motor_en = 1;
		}break;
		case MID:
		{
			if(MOVE.motor_en == 2)
				MOVE.motor_en = 0;
			else
				MOVE.motor_en = 2;
		}break;
		case LONG:
		{
			MOVE.motor_en = 3;
		}break;
		default:
		{
		}break;
	}
	KEY[0].trig = NONE;
}
/*********************************************
//按键2处理
**********************************************/
void User_Key2_Deal(void)
{
	switch(KEY[1].trig)
	{
		case NONE:;break;
		case SHORT:	
		{
			if(MOVE.motor_en != 4)
				MOVE.motor_en = 4;
			else 
				MOVE.motor_en = 5;
		}break;//
		case MID:
		{
				MOVE.motor_en = 6;
		}break;
		case LONG:
		{
			MOVE.motor_en = 7;
		}break;
		default:
		{
		}break;
	}
	KEY[1].trig = NONE;		
}
/*********************************************
//外接PS2摇杆按键处理
**********************************************/
void User_KeyH_Deal(void)
{
	static uint8_t state = 0;
	
	switch (state)
	{
		case 0:
		{
			if(KEY[2].trig == MID)
			{
				MOVE.motor_en = 8;//All motor work at PWM mode
				state++;
			}
			else;
			
		}break;
		case 1:
		{
			MOVE.motor_en = 9;//All motor work at current mode 
			if(KEY[2].trig == MID)
			{
				state++;
			}
		}break;
		case 2:
		{
			MOVE.motor_en = 10;//All motor work at speed mode  
			if(KEY[2].trig == MID)
			{
				state++;
			}
		}break;
		case 3:
		{
			MOVE.motor_en = 11;//All motor work at pos mode
			if(KEY[2].trig == MID)
			{
				state = 1;
			}
		}break;
		default:state = 0;MOVE.motor_en = 0;break;
	}
	
	if(KEY[2].trig == SHORT)
	{
		state = 0;
		MOVE.motor_en = 0;//
	}
			
	KEY[2].trig = NONE;	
}



