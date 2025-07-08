/**
对周期实时性要求较高或执行周期<2ms的任务在定时器中断服务函数中执行
如：巡线传感器检测、里程计坐标校正、按键检测等
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
使能定时器工作在中断模式
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
按键检测周期：1ms
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
	User_MoveOdomCal();//小车行驶里程计计算
}


/**
us级延时，该延时函数将阻塞程序执行，非必要不使用
用于IO口模拟IIC通信使用（stm32f1硬件IIC尚存在问题）
**/
void TimerDelayUs(uint16_t us)
{
	_USER_TIM_1US->CNT = 0;
	HAL_TIM_Base_Start(&_USER_HTIM_1US); 	
	while(_USER_TIM_1US->CNT < us);
	HAL_TIM_Base_Stop(&_USER_HTIM_1US); 
}
/**
ms级延时，该延时函数将阻塞程序执行，非必要不使用
**/
void User_DelayMs(uint16_t ms)
{
	while(ms--)
		TimerDelayUs(1000);
}


