/*********************************************
//ADC��ع���
**********************************************/

#include "user_adc.h"
#include "adc.h"
#include "user_led.h" 

 
USER_ADC_TYPE USER_ADC; 

//﮵�ص�ѹ-������%����ϵ
//100 	90  	80  	70  	60  	50  	40  	30  	20  	10  	5   	0
//4.2 	4.06	3.98	3.92	3.87	3.82	3.79	3.77	3.74	3.68	3.45	3
//12.6	12.18	11.94	11.76	11.61	11.46	11.37	11.31	11.22	11.04	10.35	9
uint8_t BAT_LIST[11] = {90,110,112,113,114,115,116,117,119,122,126};

/*********************************************
//��ʼ��
**********************************************/
void User_AdcInit(void)
{
	HAL_ADCEx_Calibration_Start(&hadc1);//F1��ҪУ׼��F4������У׼������
	HAL_ADC_Start_DMA(&hadc1, (uint32_t*)USER_ADC.adc1_buf, ADC1_CH_NUM);
	USER_ADC.battery = 12.6f;
	USER_ADC.adc1_buf[3] = 4096;
}
/*********************************************
//ADC�ɼ�����
**********************************************/
void User_AdcGetTask(void)
{
	static uint16_t cnt = 0;
	
	USER_ADC.battery = USER_ADC.adc1_buf[3]*3.3f/4096*11.07f;//��ص�ѹ0~36V
	USER_ADC.knob = -(USER_ADC.adc1_buf[1]-2048)/20.48f;//��λ����ֵ��-100~100
	USER_ADC.hand_x = -(USER_ADC.adc1_buf[0] - 2048)/20.48f -2.1;//���PS2ҡ��X������ֵ��-100~100
	USER_ADC.hand_y = -(USER_ADC.adc1_buf[2] - 2048)/20.48f -2.1;//���PS2ҡ��Y������ֵ��-100~100
	
	
	if(USER_ADC.battery < 11.0)//��ر�����ѹ
	{
		cnt++;
		if(cnt > 1000)//����5s���ϵ͵�ѹ�򱨾�
		{
			cnt = 1000;
			BUZ.remain_ms = 100;
		}	
	}
	else
	{
		cnt = 0;
	}
}





