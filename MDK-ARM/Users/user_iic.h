#ifndef _user_iic_h
#define _user_iic_h
#include "main.h" 
#include "user_timer.h"
//////////////////////////////////////////////////////////////////////////////////	 
							  
////////////////////////////////////////////////////////////////////////////////// 	

#define _IIC_SCL_H	HAL_GPIO_WritePin(IIC_SCL_GPIO_Port, IIC_SCL_Pin, GPIO_PIN_SET)
#define _IIC_SCL_L	HAL_GPIO_WritePin(IIC_SCL_GPIO_Port, IIC_SCL_Pin, GPIO_PIN_RESET)
#define _IIC_SDA_H	HAL_GPIO_WritePin(IIC_SDA_GPIO_Port, IIC_SDA_Pin, GPIO_PIN_SET)
#define _IIC_SDA_L	HAL_GPIO_WritePin(IIC_SDA_GPIO_Port, IIC_SDA_Pin, GPIO_PIN_RESET)
#define _IIC_SDA_R	HAL_GPIO_ReadPin(IIC_SDA_GPIO_Port, IIC_SDA_Pin)

#define User_DelayUs(us) TimerDelayUs(us)

//IIC���в�������
void User_IIC_Init(void);                //��ʼ��IIC��IO��				 
void User_IIC_Start(void);				//����IIC��ʼ�ź�
void User_IIC_Stop(void);	  			//����IICֹͣ�ź�
void User_IIC_SendByte(uint8_t txd);			//IIC����һ���ֽ�
uint8_t User_IIC_ReadByte(unsigned char ack);//IIC��ȡһ���ֽ�
uint8_t User_IIC_WaitAck(void); 				//IIC�ȴ�ACK�ź�
void User_IIC_Ack(void);					//IIC����ACK�ź�
void User_IIC_NAck(void);				//IIC������ACK�ź�


#endif
















