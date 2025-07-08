/**
IIC驱动程序（IO模拟IIC）
用于OLED操作
**/

#include "user_iic.h"

/**
定义IIC通信中延时时间
长延时——2us
短延时——1us
若IIC通信不稳定，可尝试增加该延时
**/
#define CLK_T_LONG 	2
#define CLK_T_SHORT 1

//初始化IIC
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

//产生IIC起始信号
void User_IIC_Start(void) 
{
	User_SetSdaOut();     //sda线输出
	_IIC_SDA_H;	  	  
	_IIC_SCL_H;
	User_DelayUs(CLK_T_LONG);
 	_IIC_SDA_L;//START:when CLK is high,DATA change form high to low 
	User_DelayUs(CLK_T_LONG);
	_IIC_SCL_L;//钳住I2C总线，准备发送或接收数据 
}	  
//产生IIC停止信号
void User_IIC_Stop(void)
{
	User_SetSdaOut();//sda线输出
	_IIC_SCL_L;
	_IIC_SDA_L;//STOP:when CLK is high DATA change form low to high
 	User_DelayUs(CLK_T_LONG);
	_IIC_SCL_H; 
	_IIC_SDA_H;//发送I2C总线结束信号
	User_DelayUs(CLK_T_LONG);							   	
}
//等待应答信号到来
//返回值：1，接收应答失败
//        0，接收应答成功
uint8_t User_IIC_WaitAck(void)
{
	uint8_t cnt_time=0;
	User_SetSdaIn();      //SDA设置为输入  
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
	_IIC_SCL_L;//时钟输出0 	   
	return 0;  
} 
//产生ACK应答
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
//不产生ACK应答		    
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
//IIC发送一个字节
//返回从机有无应答
//1，有应答
//0，无应答			  
void User_IIC_SendByte(uint8_t txd)
{                        
  uint8_t t;   
	User_SetSdaOut(); 	    
	_IIC_SCL_L;//拉低时钟开始数据传输
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
//读1个字节，ack=1时，发送ACK，ack=0，发送nACK   
uint8_t User_IIC_ReadByte(unsigned char ack)
{
	unsigned char i,receive=0;
	User_SetSdaIn();//SDA设置为输入
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
    User_IIC_NAck();//发送nACK
  else
    User_IIC_Ack(); //发送ACK   
  return receive;
}



























