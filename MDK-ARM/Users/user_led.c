/**
LED������ع���
**/
#include "user_led.h"

LED_TYPEDEF LED[3];
BUZ_TYPEDEF BUZ;

/**
BUZ����IO����USART1_TX�źŸ��ã�USART1ΪIMUģ�飬һ��ֻ�����IMU���ݣ���TX�ӿڲ����ã�������BUZ����
����Ҫ�������ݵ�IMUʱӦע���ȹر�BUZ������ع��ܣ��ٸ��ĸ�IOΪTXģʽ���д��䡣
**/
void User_BuzOff(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	
	GPIO_InitStruct.Pin = U1_TX_BUZ_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(U1_TX_BUZ_GPIO_Port, &GPIO_InitStruct);	
	BUZ_OFF; 
}
void User_BuzOn(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	
	GPIO_InitStruct.Pin = U1_TX_BUZ_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(U1_TX_BUZ_GPIO_Port, &GPIO_InitStruct);	
	BUZ_ON; 	
}
/**
��ʼ��
**/
void User_LedInit(void)
{
	LED_R_OFF;
	LED_G_OFF;
	LED_B_OFF;
	User_BuzOff();
}

/**
LED��˸���ã�
rgb��0-���ú�ƣ�1-�����̵ƣ�2-��������
freq��������˸Ƶ�ʣ�Hz��
time��������˸����
en��������˸ʹ��
**/
void User_LedShowSet(uint8_t rgb,uint8_t freq,uint8_t time,uint8_t en)
{	
	LED[rgb].flash_freq = freq;
	LED[rgb].flash_time = time;
	LED[rgb].flash_en = en;  
}

void User_LedEn(uint8_t rgpb,uint8_t en)
{
	switch(rgpb)
	{
		case 0: en?LED_R_ON:LED_R_OFF;break;
		case 1: en?LED_G_ON:LED_G_OFF;break;
		case 2: en?LED_B_ON:LED_B_OFF;break;
		default:break;
	}
}
/**
LED��˸����
**/
void User_LedShow(void)//
{
	uint8_t i = 0;
// static uint16_t cnt = 0;
	
	for(i=0;i<=2;i++)
	{
		if(LED[i].flash_en == 0)
		{
			User_LedEn(i,0);
		}
		else if(LED[i].flash_freq == 0)
		{
			User_LedEn(i,1);
		}
		else
		{
			LED[i].on_cnt = 1000/_LED_TASK_TIME/LED[i].flash_freq/2;

			LED[i].task_cnt++;
			if(LED[i].remain_time) 
			{
				if(LED[i].task_cnt <= LED[i].on_cnt)
				{
					User_LedEn(i,1);
				}
				else if( LED[i].task_cnt <= 2*LED[i].on_cnt)
				{
					User_LedEn(i,0);			
				}
				if(LED[i].task_cnt == 2*LED[i].on_cnt)
				{
					LED[i].remain_time--;
					LED[i].task_cnt = 0;
				}		
			}
			else
			{
				User_LedEn(i,0);	

				LED[i].task_cnt++;					
				if(LED[i].task_cnt == 1000/_LED_TASK_TIME)
				{
					LED[i].task_cnt = 0;
					LED[i].remain_time = LED[i].flash_time;
				}			
			}
		}	
	}
}

/**
BUZ����
**/
void User_BuzSound(void)
{
	BUZ.remain_ms -= _LED_TASK_TIME;
	if(BUZ.remain_ms > 0)
	{
		User_BuzOn();
	}		
	else 
	{
		BUZ.remain_ms = 0;
		User_BuzOff();
	}	
}



