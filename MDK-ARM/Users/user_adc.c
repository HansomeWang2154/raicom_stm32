/*********************************************
//ADC相关功能
**********************************************/

#include "user_adc.h"
#include "adc.h"
#include "user_led.h" 

 
USER_ADC_TYPE USER_ADC; 

//锂电池电压-电量（%）关系
//100 	90  	80  	70  	60  	50  	40  	30  	20  	10  	5   	0
//4.2 	4.06	3.98	3.92	3.87	3.82	3.79	3.77	3.74	3.68	3.45	3
//12.6	12.18	11.94	11.76	11.61	11.46	11.37	11.31	11.22	11.04	10.35	9
uint8_t BAT_LIST[11] = {90,110,112,113,114,115,116,117,119,122,126};

/*********************************************
//初始化
**********************************************/
void User_AdcInit(void)
{
	HAL_ADCEx_Calibration_Start(&hadc1);//F1需要校准，F4无需自校准！！！
	HAL_ADC_Start_DMA(&hadc1, (uint32_t*)USER_ADC.adc1_buf, ADC1_CH_NUM);
	USER_ADC.battery = 12.6f;
	USER_ADC.adc1_buf[3] = 4096;
}
/*********************************************
//ADC采集任务
**********************************************/
void User_AdcGetTask(void)
{
	static uint16_t cnt = 0;
	
	USER_ADC.battery = USER_ADC.adc1_buf[3]*3.3f/4096*11.07f;//电池电压0~36V
	USER_ADC.knob = -(USER_ADC.adc1_buf[1]-2048)/20.48f;//电位器数值：-100~100
	USER_ADC.hand_x = -(USER_ADC.adc1_buf[0] - 2048)/20.48f -2.1;//外接PS2摇杆X方向数值：-100~100
	USER_ADC.hand_y = -(USER_ADC.adc1_buf[2] - 2048)/20.48f -2.1;//外接PS2摇杆Y方向数值：-100~100
	
	
	if(USER_ADC.battery < 11.0)//电池保护电压
	{
		cnt++;
		if(cnt > 1000)//持续5s以上低电压则报警
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





