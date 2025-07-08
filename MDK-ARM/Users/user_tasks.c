
/**
用户定义程序执行内容：
1、操作系统任务服务函数：user_tasks.c : StartTaskHigh();StartTaskMid();StartTaskLow();
2、定时器中断服务函数：见user_timer.c
3、外设中断服务函数：见stm32f1xx_it.c
**/


/**
包含使用函数声明所在的头文件
**/
#include "user_tasks.h"
#include "cmsis_os.h" 
#include "user_led.h"
#include "user_oled.h"
#include "user_adc.h"
#include "user_imu.h"
#include "user_timer.h"
#include "tim.h"
#include "user_key.h"
#include "user_can_motor.h" 
#include "user_movebase.h"
#include "user_comm_wifi.h"   
#include "user_state.h" 
#include "user_comm_raspberry.h"
#include "user_math.h"
#include "user_m3508.h"
#include "user_m3510.h"
#include "user_tofsense_f.h"
#include "remote_control.h"
#include "user_tofsenser2.h"
#include "user_scissor.h"
#include "user_catch.h"

//#include "user_uart1.h"  // 新增

int temp=600;
uint32_t jian1=285000;//合 285000 闭 0
uint32_t jian2=280000;//合 280000 闭 0
int cur1=2000;
int16_t ch4_temp;
int jian_count=0;
int jian_flag=0;
/**
外设、模块初始化配置
**/
void User_Init(void)
{		
	__disable_irq();
	
	User_LedInit();
	User_TimerInit();
	User_MoveBaseInit();
	User_M3508_Init();
	User_M3510_Init();
	User_StateMachineInit();
	
	User_AdcInit();
	User_ImuInit();	
	User_OledInit();//部分OLED上电过程较慢，若初始化太快可能OLED无显示，需上电后复位stm32才能正常显示		
	User_CanMotorInit();			
	User_KeyInit();
	User_TOFSenseInit();
	User_Tofsenser2Init();
	User_scissorInit();
//	User_RemoteInit();
//	HAL_TIM_PWM_Start(&htim1,TIM_CHANNEL_1);
//	__HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_1,1000);
  User_CommWifiInit();//延时一段时间后再对模块对应串口的初始化,否则串口无法接收数据
	__enable_irq();
//	HAL_UART_Receive_IT(&huart3,rebuffer,1);
	
	osDelay(100);
	MOVE.angz_pid.target=IMU1.angz;
	User_CommSmpInit();
	
	
//	UART1_InterruptRxInit();
}

uint8_t STATE_MACHINE_EN = 0;
/**
操作系统任务服务函数，最高优先级
**/
void StartTaskHigh(void *argument)
{
	User_Init();
	
	for(;;)
  {						 
		User_MoveOdomCal();//小车行驶里程计计算
		
		if(SMP.init_ok == 1)
		{
			User_CommSmpSendTask();
			User_CommWifiSendTask();
		}	
		
		osDelay(2);//执行周期2ms
  } 
}
uint8_t txdataTest[8] = {8,8,8,8,8,8,8,8};
/**
操作系统任务服务函数，次高优先级
**/
void StartTaskMid(void *argument)
{	
	int16_t temp = 0;
	MOVE.motor_en = 0;
	int test_state=0;
	int test_round=10;
	int test_cur = 0;
	for(;;)
  {																		
		//读取按键状态，根据不同状态控制电机工作模式
		User_Key1_Deal(); //MOVE.motor_en <-0,1,2,3
		User_Key2_Deal();	//MOVE.motor_en <-4,5,6,7
		
		switch(MOVE.motor_en) 
		{
			case 0://测试		
			{
				MOVE.ctr_type = CTR_NONE;
				User_MoveSpeedCal();//根据控制模式通过输入计算move的三个速度
				User_M3508_SpetoCur_PID();
//				HAL_UART_Transmit(&huart4, (uint8_t *)txdataTest, 8,1000);
				
							
				//CAN_SendCurGroup3508(0x200,test_cur,test_cur,test_cur,test_cur);
	//			CAN_SendCurGroup3508(0x1FF,0,0,0,0);
//				HAL_TIM_PWM_Start(&htim1,TIM_CHANNEL_1);
				
			}break;	
			case 1://底盘解算电机速度
			{
				//运动控制
				MOVE.ctr_type = CTR_REMOTE;
				User_MoveSpeedCal();//根据控制模式通过输入计算move的三个速度
				User_M3508_SpetoCur_PID();
				//Caughters_Control();
			}break;
			case 2://SLAM
			{
				MOVE.ctr_type = CTR_SLAM;
				User_MoveSpeedCal();//根据控制模式通过输入计算move的三个速度
				User_M3508_SpetoCur_PID();
			}break;
			case 3://保留
			{
//				CAN_SendPosSingle(3,jian2);
				CAN_SendCurGroup3508(0x200,test_cur,test_cur,test_cur,test_cur);
				//CAN_SendCurGroup3508(0x1FF,5000,2000,2000,2000);
			}break;
			case 4://设置某组电机工作在PWM模式（PWM值设置为0），此时可读取电机速度、位置等反馈信息。
			{
				CAN_SendPwmGroup(0,0,0,0,0);	
				STATE_MACHINE_EN = 0;
			}break;
			case 5:
			{
				MOVE.ctr_type = CTR_NONE;
				STATE_MACHINE_EN = 1;
				MOVE.motor_en = 1;
			}break;
			case 6://保留
			{
				if(jian_flag==1)
				{
				CAN_SendPosSingle(1,jian1);
				CAN_SendPosSingle(2,jian2);
				}
				else if(jian_flag==0)
				{
					CAN_SendPosSingle(1,0);
					CAN_SendPosSingle(2,0);
				}
				MOVE.motor_en=1;
			}break;
			
			default:MOVE.motor_en = 0;break;
		}	
		
		User_AdcGetTask();//读取ADC采样信息	
		
		osDelay(5);//执行周期5ms
  }
}
/**
操作系统任务服务函数，最底优先级
**/
float TEST_MAX = 0;
float TEST_MIN = 0;
uint16_t cnt_test = 0;
uint8_t i = 0;
void StartTaskLow(void *argument)
{
	User_LedShowSet(2,5,2,1);	//蓝灯、闪烁频率5Hz、闪烁2次、使能
	BUZ.remain_ms = 100;
	uint16_t cnt = 0;
	for(;;)
  {		 	
		if(cnt++ == 100)//2s
		{
			User_CommWifiInit();//延时一段时间后再对模块对应串口的初始化,否则串口无法接收数据
//			IMU1.angz_dif = 180 - IMU1.angz;
		}
		else if(cnt == 200)//10s
		{
			User_CommSmpInit();//延时一段时间后再对模块对应串口的初始化,否则树莓派将进入BIOS
			
		}
		else if(cnt > 200)
		{
			cnt = 201;
		}
		
		User_LedShow();					//触发LED显示任务1次
		User_OledShow();				//触发OLED显示任务1次
		User_BuzSound(); 				//触发蜂鸣器指示任务1次

//		User_CommWifiSendTask();
		
		osDelay(_LED_TASK_TIME);//执行周期20ms
  }
}



