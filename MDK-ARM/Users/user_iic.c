/**
IIC��������IOģ��IIC��
����OLED����
**/

#include "user_iic.h"

/**
����IICͨ������ʱʱ��
����ʱ����2us
����ʱ����1us
��IICͨ�Ų��ȶ����ɳ������Ӹ���ʱ
**/
#define CLK_T_LONG 	2
#define CLK_T_SHORT 1

//��ʼ��IIC
void User_IIC_Init(void)
{			
	_IIC_SCL_H;
	_IIC_SDA_H;
}

void User_SetSdaIn(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Pin = IIC_SDA_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(IIC_SDA_GPIO_Port, &GPIO_InitStruct);	
}
void User_SetSdaOut(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Pin = IIC_SDA_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(IIC_SDA_GPIO_Port, &GPIO_InitStruct);
}

//����IIC��ʼ�ź�
void User_IIC_Start(void) 
{
	User_SetSdaOut();     //sda�����
	_IIC_SDA_H;	  	  
	_IIC_SCL_H;
	User_DelayUs(CLK_T_LONG);
 	_IIC_SDA_L;//START:when CLK is high,DATA change form high to low 
	User_DelayUs(CLK_T_LONG);
	_IIC_SCL_L;//ǯסI2C���ߣ�׼�����ͻ�������� 
}	  
//����IICֹͣ�ź�
void User_IIC_Stop(void)
{
	User_SetSdaOut();//sda�����
	_IIC_SCL_L;
	_IIC_SDA_L;//STOP:when CLK is high DATA change form low to high
 	User_DelayUs(CLK_T_LONG);
	_IIC_SCL_H; 
	_IIC_SDA_H;//����I2C���߽����ź�
	User_DelayUs(CLK_T_LONG);							   	
}
//�ȴ�Ӧ���źŵ���
//����ֵ��1������Ӧ��ʧ��
//        0������Ӧ��ɹ�
uint8_t User_IIC_WaitAck(void)
{
	uint8_t cnt_time=0;
	User_SetSdaIn();      //SDA����Ϊ����  
	_IIC_SDA_H;
	User_DelayUs(CLK_T_SHORT);	   
	_IIC_SCL_H;
	User_DelayUs(CLK_T_SHORT);	 
	while(_IIC_SDA_R)
	{
		cnt_time++;
		if(cnt_time>250)
		{
			User_IIC_Stop();
			return 1;
		}
	}
	_IIC_SCL_L;//ʱ�����0 	   
	return 0;  
} 
//����ACKӦ��
void User_IIC_Ack(void)
{
	_IIC_SCL_L;
	User_SetSdaOut();
	_IIC_SDA_L;
	User_DelayUs(CLK_T_SHORT);
	_IIC_SCL_H;
	User_DelayUs(CLK_T_SHORT);
	_IIC_SCL_L;
}
//������ACKӦ��		    
void User_IIC_NAck(void)
{
	_IIC_SCL_L;
	User_SetSdaOut();
	_IIC_SDA_H;
	User_DelayUs(CLK_T_SHORT);
	_IIC_SCL_H;
	User_DelayUs(CLK_T_SHORT);
	_IIC_SCL_L;
}					 				     
//IIC����һ���ֽ�
//���شӻ�����Ӧ��
//1����Ӧ��
//0����Ӧ��			  
void User_IIC_SendByte(uint8_t txd)
{                        
  uint8_t t;   
	User_SetSdaOut(); 	    
	_IIC_SCL_L;//����ʱ�ӿ�ʼ���ݴ���
	for(t=0;t<8;t++)
	{              
		if((txd & 0x80) >> 7)
			_IIC_SDA_H;
		else
			_IIC_SDA_L;
		txd <<= 1; 	  
		User_DelayUs(CLK_T_SHORT);   //
		_IIC_SCL_H;
		User_DelayUs(CLK_T_SHORT); 
		_IIC_SCL_L;	
		User_DelayUs(CLK_T_SHORT);
	}	 
} 	    
//��1���ֽڣ�ack=1ʱ������ACK��ack=0������nACK   
uint8_t User_IIC_ReadByte(unsigned char ack)
{
	unsigned char i,receive=0;
	User_SetSdaIn();//SDA����Ϊ����
  for(i=0;i<8;i++ )
	{
    _IIC_SCL_L; 
    User_DelayUs(CLK_T_SHORT);
		_IIC_SCL_H;
    receive <<= 1;
    if(_IIC_SDA_R)
			receive++;   
		User_DelayUs(CLK_T_SHORT); 
  }					 
  if (!ack)
    User_IIC_NAck();//����nACK
  else
    User_IIC_Ack(); //����ACK   
  return receive;
}



























