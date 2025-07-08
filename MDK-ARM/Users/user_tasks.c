
/**
�û��������ִ�����ݣ�
1������ϵͳ�����������user_tasks.c : StartTaskHigh();StartTaskMid();StartTaskLow();
2����ʱ���жϷ���������user_timer.c
3�������жϷ���������stm32f1xx_it.c
**/


/**
����ʹ�ú����������ڵ�ͷ�ļ�
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

//#include "user_uart1.h"  // ����

int temp=600;
uint32_t jian1=285000;//�� 285000 �� 0
uint32_t jian2=280000;//�� 280000 �� 0
int cur1=2000;
int16_t ch4_temp;
int jian_count=0;
int jian_flag=0;
/**
���衢ģ���ʼ������
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
	User_OledInit();//����OLED�ϵ���̽���������ʼ��̫�����OLED����ʾ�����ϵ��λstm32����������ʾ		
	User_CanMotorInit();			
	User_KeyInit();
	User_TOFSenseInit();
	User_Tofsenser2Init();
	User_scissorInit();
//	User_RemoteInit();
//	HAL_TIM_PWM_Start(&htim1,TIM_CHANNEL_1);
//	__HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_1,1000);
  User_CommWifiInit();//��ʱһ��ʱ����ٶ�ģ���Ӧ���ڵĳ�ʼ��,���򴮿��޷���������
	__enable_irq();
//	HAL_UART_Receive_IT(&huart3,rebuffer,1);
	
	osDelay(100);
	MOVE.angz_pid.target=IMU1.angz;
	User_CommSmpInit();
	
	
//	UART1_InterruptRxInit();
}

uint8_t STATE_MACHINE_EN = 0;
/**
����ϵͳ�����������������ȼ�
**/
void StartTaskHigh(void *argument)
{
	User_Init();
	
	for(;;)
  {						 
		User_MoveOdomCal();//С����ʻ��̼Ƽ���
		
		if(SMP.init_ok == 1)
		{
			User_CommSmpSendTask();
			User_CommWifiSendTask();
		}	
		
		osDelay(2);//ִ������2ms
  } 
}
uint8_t txdataTest[8] = {8,8,8,8,8,8,8,8};
/**
����ϵͳ������������θ����ȼ�
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
		//��ȡ����״̬�����ݲ�ͬ״̬���Ƶ������ģʽ
		User_Key1_Deal(); //MOVE.motor_en <-0,1,2,3
		User_Key2_Deal();	//MOVE.motor_en <-4,5,6,7
		
		switch(MOVE.motor_en) 
		{
			case 0://����		
			{
				MOVE.ctr_type = CTR_NONE;
				User_MoveSpeedCal();//���ݿ���ģʽͨ���������move�������ٶ�
				User_M3508_SpetoCur_PID();
//				HAL_UART_Transmit(&huart4, (uint8_t *)txdataTest, 8,1000);
				
							
				//CAN_SendCurGroup3508(0x200,test_cur,test_cur,test_cur,test_cur);
	//			CAN_SendCurGroup3508(0x1FF,0,0,0,0);
//				HAL_TIM_PWM_Start(&htim1,TIM_CHANNEL_1);
				
			}break;	
			case 1://���̽������ٶ�
			{
				//�˶�����
				MOVE.ctr_type = CTR_REMOTE;
				User_MoveSpeedCal();//���ݿ���ģʽͨ���������move�������ٶ�
				User_M3508_SpetoCur_PID();
				//Caughters_Control();
			}break;
			case 2://SLAM
			{
				MOVE.ctr_type = CTR_SLAM;
				User_MoveSpeedCal();//���ݿ���ģʽͨ���������move�������ٶ�
				User_M3508_SpetoCur_PID();
			}break;
			case 3://����
			{
//				CAN_SendPosSingle(3,jian2);
				CAN_SendCurGroup3508(0x200,test_cur,test_cur,test_cur,test_cur);
				//CAN_SendCurGroup3508(0x1FF,5000,2000,2000,2000);
			}break;
			case 4://����ĳ����������PWMģʽ��PWMֵ����Ϊ0������ʱ�ɶ�ȡ����ٶȡ�λ�õȷ�����Ϣ��
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
			case 6://����
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
		
		User_AdcGetTask();//��ȡADC������Ϣ	
		
		osDelay(5);//ִ������5ms
  }
}
/**
����ϵͳ�����������������ȼ�
**/
float TEST_MAX = 0;
float TEST_MIN = 0;
uint16_t cnt_test = 0;
uint8_t i = 0;
void StartTaskLow(void *argument)
{
	User_LedShowSet(2,5,2,1);	//���ơ���˸Ƶ��5Hz����˸2�Ρ�ʹ��
	BUZ.remain_ms = 100;
	uint16_t cnt = 0;
	for(;;)
  {		 	
		if(cnt++ == 100)//2s
		{
			User_CommWifiInit();//��ʱһ��ʱ����ٶ�ģ���Ӧ���ڵĳ�ʼ��,���򴮿��޷���������
//			IMU1.angz_dif = 180 - IMU1.angz;
		}
		else if(cnt == 200)//10s
		{
			User_CommSmpInit();//��ʱһ��ʱ����ٶ�ģ���Ӧ���ڵĳ�ʼ��,������ݮ�ɽ�����BIOS
			
		}
		else if(cnt > 200)
		{
			cnt = 201;
		}
		
		User_LedShow();					//����LED��ʾ����1��
		User_OledShow();				//����OLED��ʾ����1��
		User_BuzSound(); 				//����������ָʾ����1��

//		User_CommWifiSendTask();
		
		osDelay(_LED_TASK_TIME);//ִ������20ms
  }
}



