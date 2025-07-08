#ifndef _user_adc_h
#define _user_adc_h

#include "main.h"

#define ADC1_CH_NUM	4

typedef struct
{
	uint16_t adc1_buf[ADC1_CH_NUM];	
	
	float battery;
	float knob;
	float hand_x;
	float hand_y;
	
	uint8_t bat_remain;
	
}USER_ADC_TYPE;
extern USER_ADC_TYPE USER_ADC;


void User_AdcInit(void);
void User_AdcGetTask(void);

#endif










