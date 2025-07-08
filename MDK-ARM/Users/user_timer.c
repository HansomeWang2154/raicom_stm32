/**
������ʵʱ��Ҫ��ϸ߻�ִ������<2ms�������ڶ�ʱ���жϷ�������ִ��
�磺Ѳ�ߴ�������⡢��̼�����У������������
**/

#include "user_timer.h"
#include "tim.h"
#include "user_movebase.h"
#include "user_key.h" 
#include "user_can_motor.h"
#include "user_tofsense_f.h"


#define _USER_HTIM_100US	htim1
#define _USER_TIM_100US		TIM1
#define _USER_HTIM_1MS		htim2
#define _USER_TIM_1MS			TIM2
#define _USER_HTIM_1US		htim8
#define _USER_TIM_1US			TIM8

/**
ʹ�ܶ�ʱ���������ж�ģʽ
**/
void User_TimerInit(void)
{
	HAL_TIM_Base_Start_IT(&_USER_HTIM_100US);// 
	HAL_TIM_Base_Start_IT(&_USER_HTIM_1MS);//  		
}


void User_Timer100usIRQ(void)
{
//	static uint8_t cnt = 0;
}
/**
����������ڣ�1ms
**/
void User_Timer1msIRQ(void)
{
//	static uint16_t cnt = 0;
//	static uint8_t id = 0;
//	if(++cnt == 10){
//		cnt = 0;
//		User_TOFSense_Get(id);
//		if(id == tofsense_num)
//			id = 0;
//		else
//			id++;
//	}
	User_KeyCheck(); 
	User_MoveOdomCal();//С����ʻ��̼Ƽ���
}


/**
us����ʱ������ʱ��������������ִ�У��Ǳ�Ҫ��ʹ��
����IO��ģ��IICͨ��ʹ�ã�stm32f1Ӳ��IIC�д������⣩
**/
void TimerDelayUs(uint16_t us)
{
	_USER_TIM_1US->CNT = 0;
	HAL_TIM_Base_Start(&_USER_HTIM_1US); 	
	while(_USER_TIM_1US->CNT < us);
	HAL_TIM_Base_Stop(&_USER_HTIM_1US); 
}
/**
ms����ʱ������ʱ��������������ִ�У��Ǳ�Ҫ��ʹ��
**/
void User_DelayMs(uint16_t ms)
{
	while(ms--)
		TimerDelayUs(1000);
}


