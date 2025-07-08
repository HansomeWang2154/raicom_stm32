/**
OLED驱动函数及显示功能
**/

#include "user_oled.h"
#include "user_iic.h"
#include "oled_font.h"
#include "oled_bmp.h"
#include "stdio.h"
#include "string.h"

#include "cmsis_os.h" 
#include "user_adc.h"
#include "user_imu.h"
#include "user_key.h"
#include "user_movebase.h"
#include "user_can_motor.h" 



//IIC速率可设置到1500000


//OLED的显存 128*32点阵
//存放格式如下.
//[0]0 1 2 3 ... 127//第0页：128列，每列8行	
//[1]0 1 2 3 ... 127//第1页：128列，每列8行		
//[2]0 1 2 3 ... 127//第2页：128列，每列8行		
//[3]0 1 2 3 ... 127//第3页：128列，每列8行		
/**********************************************
//OLED Write Command
**********************************************/
void User_OledWriteCmd(uint8_t cmd)
{
	User_IIC_Start();
  User_IIC_SendByte(_OLED_ADDR);  //Slave address,SA0=0,0x78
	User_IIC_WaitAck();	
  User_IIC_SendByte(0x00);			//write command
	User_IIC_WaitAck();	
	User_IIC_SendByte(cmd);
	User_IIC_WaitAck();
	User_IIC_Stop();
}
/**********************************************
//OLED Write Data
**********************************************/
void User_OledWriteData(uint8_t data)
{
	User_IIC_Start();
  User_IIC_SendByte(_OLED_ADDR);  //Slave address,SA0=0,0x78
	User_IIC_WaitAck();	
  User_IIC_SendByte(0x40);			//write data
	User_IIC_WaitAck();	
	User_IIC_SendByte(data);
	User_IIC_WaitAck();
	User_IIC_Stop();
}
/**********************************************
//OLED Write Byte
**********************************************/
void User_OledWriteByte(uint8_t dat, OLED_WR_MODE cmd)
{	
	switch(cmd)
	{
		case OLED_cmd: 
		{
			User_OledWriteCmd(dat);
		}break;
		case OLED_data:
		{
			User_OledWriteData(dat);
		}break;
		default:break;
	} 
}
/**********************************************
//OLED Init
**********************************************/
void User_OledInit(void)
{	
	User_IIC_Init();
	
	User_OledWriteByte(0xAE,OLED_cmd);//关闭显示 
	
	User_OledWriteByte(0x40,OLED_cmd);//---set low column address
	User_OledWriteByte(0xB0,OLED_cmd);//---set high column address

	User_OledWriteByte(0xC8,OLED_cmd);//-not offset

	User_OledWriteByte(0x81,OLED_cmd);//设置对比度
	User_OledWriteByte(0xff,OLED_cmd);

	User_OledWriteByte(0xa1,OLED_cmd);//段重定向设置

	User_OledWriteByte(0xa6,OLED_cmd);//
	
	User_OledWriteByte(0xa8,OLED_cmd);//设置驱动路数
	User_OledWriteByte(0x1f,OLED_cmd);
	
	User_OledWriteByte(0xd3,OLED_cmd);
	User_OledWriteByte(0x00,OLED_cmd);
	
	User_OledWriteByte(0xd5,OLED_cmd);
	User_OledWriteByte(0xf0,OLED_cmd);
	
	User_OledWriteByte(0xd9,OLED_cmd);
	User_OledWriteByte(0x22,OLED_cmd);
	
	User_OledWriteByte(0xda,OLED_cmd);
	User_OledWriteByte(0x02,OLED_cmd);
	
	User_OledWriteByte(0xdb,OLED_cmd);
	User_OledWriteByte(0x49,OLED_cmd);
	
	User_OledWriteByte(0x8d,OLED_cmd);//--set Charge Pump enable/disable
	User_OledWriteByte(0x14,OLED_cmd);//--set(0x10) disable
	
	User_OledWriteByte(0xaf,OLED_cmd);
	
	User_OledDisplayStatus(OLED_clear); 
	User_OledDisplayStatus(OLED_test); 
	User_OledDisplayStatus(OLED_clear); 
}
/**********************************************
//显示状态
**********************************************/
void User_OledDisplayStatus(OLED_DISP_MODE mode) 
{
	uint8_t i,n;		
	switch(mode)
	{
		case OLED_on://开启显示
			User_OledWriteByte(0X8D, OLED_cmd);  //SET DCDC命令
			User_OledWriteByte(0X14, OLED_cmd);  //DCDC ON
			User_OledWriteByte(0XAF, OLED_cmd);  //DISPLAY ON
			break;
		case OLED_off://关闭显示  
			User_OledWriteByte(0X8D, OLED_cmd);  //SET DCDC命令
			User_OledWriteByte(0X10, OLED_cmd);  //DCDC OFF
			User_OledWriteByte(0XAE, OLED_cmd);  //DISPLAY OFF
			break;
		case OLED_clear://清屏，整个屏幕是黑色的!和没点亮一样!!!		
			for(i = 0; i < 4; i ++)  
			{  
				User_OledWriteByte(0xB0 + i, OLED_cmd);    //设置页地址(0~3)
				User_OledWriteByte(0x00, OLED_cmd);      //设置显示位置—列低地址
				User_OledWriteByte(0x10, OLED_cmd);      //设置显示位置—列高地址 
				for(n = 0; n < _OLED_MAX_COLUMN; n++)
				{
					User_OledWriteByte(0, OLED_data); 
				}
			} 
			break;
		case OLED_test:
			for(i = 0; i < 4; i ++)  
			{  
				User_OledWriteByte(0xb0 + i, OLED_cmd);    //设置页地址(0~3)
				User_OledWriteByte(0x00, OLED_cmd);      //设置显示位置—列低地址
				User_OledWriteByte(0x10, OLED_cmd);      //设置显示位置—列高地址 
				for(n = 0; n < _OLED_MAX_COLUMN; n++)
				{
					User_OledWriteByte(0XFF, OLED_data);
					User_DelayMs(1);
				}
			} 
			break;
	}
}
/**********************************************
//坐标设置
**********************************************/
void User_OledSetPos(unsigned char x, unsigned char y) 
{ 	
	User_OledWriteByte(0xb0+y,OLED_cmd);
	User_OledWriteByte(((x&0xf0)>>4)|0x10,OLED_cmd);
	User_OledWriteByte((x&0x0f),OLED_cmd); 
}
/**********************************************
//显示汉字
**********************************************/
void User_OledShowChinese(uint8_t x,uint8_t y,uint8_t no)
{      			    
	uint8_t t,adder=0;
	User_OledSetPos(x,y);	
    for(t=0;t<16;t++)
		{
				User_OledWriteByte(Hzk[2*no][t],OLED_data);
				adder+=1;
     }	
		User_OledSetPos(x,y+1);	
    for(t=0;t<16;t++)
			{	
				User_OledWriteByte(Hzk[2*no+1][t],OLED_data);
				adder+=1;
      }					
}
/**********************************************
//在指定位置显示一个字符,包括部分字符
//x:0~127
//y:0~4
//mode:0,反白显示;1,正常显示				 
//size:选择字体 16/8 
**********************************************/
void User_OledShowChar(uint8_t x,uint8_t y,uint8_t chr,uint8_t Char_Size)
{      	
	unsigned char c=0,i=0;	
	c=chr-' ';//得到偏移后的值			
	if(x>_OLED_MAX_COLUMN-1){x=0;y=y+2;}
	if(Char_Size ==16)
	{
		User_OledSetPos(x,y);	
		for(i=0;i<8;i++)
		User_OledWriteByte(F8X16[c*16+i],OLED_data);
		User_OledSetPos(x,y+1);
		for(i=0;i<8;i++)
		User_OledWriteByte(F8X16[c*16+i+8],OLED_data);
	}
	else 
	{	
		User_OledSetPos(x,y);
		for(i=0;i<6;i++)
		User_OledWriteByte(F6x8[c][i],OLED_data);			
	}
} 
/**********************************************
//显示字符串
**********************************************/
void User_OledShowString(uint8_t x,uint8_t y,uint8_t *chr,uint8_t Char_Size)
{
	unsigned char j=0;
	while (chr[j]!='\0')
	{		User_OledShowChar(x,y,chr[j],Char_Size);
			x+=8;
		if(x>120){x=0;y+=2;}
			j++;
	}
}
/***********功能描述：显示显示BMP图片128×64起始点坐标(x,y),x的范围0～127，y为页的范围0～7*****************/
void User_OledDrawBMP(unsigned char x0, unsigned char y0,unsigned char x1, unsigned char y1,unsigned char BMP[])
{ 	
	unsigned int j=0;
	unsigned char x,y;

	if(y1%8==0) y=y1/8;      
	else y=y1/8+1;
	for(y=y0;y<y1;y++)
	{
		User_OledSetPos(x0,y);
		for(x=x0;x<x1;x++)
		{      
			User_OledWriteByte(BMP[j++],OLED_data);	
		}
	}
}

/**********************************************************************************************************************************************************
                                                                                     自定义显示内容
************************************************************************************************************************************************************/
/*********************************************
//显示标题
**********************************************/
void User_OledScreenTitle(void)
{
	User_OledShowString(25,0,(uint8_t*)"*Group:11*",8);//批次-协同组号-车号
}
/*********************************************
//显示主页面
**********************************************/
void User_OledScreenMain(void)
{
	static uint8_t cnt = 0;
	char temp[32];
	
	cnt++;	
	switch(cnt)
	{
		case 1:
		{					
			sprintf(temp, "Bat:%.1fV ",USER_ADC.battery);
//			sprintf(temp, "spd1:%d ",	CHASS_MOTOR[1].flash.item.spe_now);
			User_OledShowString(0,1,(uint8_t*)temp,8);	
		
		}break;
		case 2:
		{
//			sprintf(temp, "Ya:%.1f' Pi:%.1f ",IMU1.angz,IMU1.angy);
			sprintf(temp, "odo:%fmm ",	MOVE.odometer.pos_y);			
			User_OledShowString(0,2,(uint8_t*)temp,8);			
		}break;
		case 3:
		{
//			sprintf(temp, "Ad:%d",(int8_t)USER_ADC.knob);
//			User_OledShowString(0,3,(uint8_t*)temp,8);		
		}break;
		case 4:
		{
//			sprintf(temp, "PosY:%.1f ",MOVE.odometer.pos_y); 
//			User_OledShowString(0,3,(uint8_t*)temp,8);
			cnt = 0;	
		}break;				
		default:cnt = 0;break;
	}	
}
/*********************************************
//显示按键状态页面
**********************************************/
void User_OledScreenKey(void)
{
	static uint8_t cnt = 0;
	char temp[32];
	
	cnt++;	
	switch(cnt)
	{
		case 1:
		{					
			sprintf(temp, "K0:%.1fs ",KEY[0].press_time/1000.0f);
			User_OledShowString(0,0,(uint8_t*)temp,8);
			if(KEY[0].press_time < 2000)
			{
				User_OledShowString(60,0,(uint8_t*)"--Short",8);
			}
			else if(KEY[0].press_time < 5000)
			{
				User_OledShowString(60,0,(uint8_t*)"--Midd ",8);
			}
			else if(KEY[0].press_time >= 5000)
			{
				User_OledShowString(60,0,(uint8_t*)"--Long ",8);
			}
				
		}break;
		case 2:
		{
			sprintf(temp, "K1:%.1fs ",KEY[1].press_time/1000.0f);
			User_OledShowString(0,1,(uint8_t*)temp,8);
			if(KEY[1].press_time < 2000)
			{
				User_OledShowString(60,1,(uint8_t*)"--Short",8);
			}
			else if(KEY[1].press_time < 5000)
			{
				User_OledShowString(60,1,(uint8_t*)"--Midd ",8);
			}
			else if(KEY[1].press_time >= 5000)
			{
				User_OledShowString(60,1,(uint8_t*)"--Long ",8);
			}
		}break;
		case 3:
		{
			sprintf(temp, "K2:%.1fs ",KEY[2].press_time/1000.0f);
			User_OledShowString(0,2,(uint8_t*)temp,8);
			if(KEY[2].press_time < 2000)
			{
				User_OledShowString(60,2,(uint8_t*)"--Short",8);
			}
			else if(KEY[2].press_time < 5000)
			{
				User_OledShowString(60,2,(uint8_t*)"--Midd ",8);
			}
			else if(KEY[2].press_time >= 5000)
			{
				User_OledShowString(60,2,(uint8_t*)"--Long ",8);
			}
			
			cnt = 0;	
		}break;				
		default:cnt = 0;break;
	}		
}
/*********************************************
//显示电机测试页面
**********************************************/
void User_OledScreenMotorTest(void)
{
	static uint8_t cnt = 0;
	char temp[32];
	
	cnt++;	
	switch(cnt)
	{
		case 1:
		{					
			User_OledShowString(5,0,(uint8_t*)"*Motor Testing*",8);			
		}break;
		case 2:
		{
			switch(MOVE.motor_en)
			{
				case 3:
				{
					User_OledShowString(0,1,(uint8_t*)"Work Mode:PWM!",8);					
				}break;
				case 4:
				{
					User_OledShowString(0,1,(uint8_t*)"Work Mode:Cur!",8);	
				}break;
				case 5:
				{
					User_OledShowString(0,1,(uint8_t*)"Work Mode:Spe!",8);	
				}break;
				case 6:
				{
					User_OledShowString(0,1,(uint8_t*)"Work Mode:Pos!",8);	
				}break;
				default:break;
			}			
		}break;
		case 3:
		{
			sprintf(temp, "Value:%.1f%%  ",USER_ADC.hand_y);
			User_OledShowString(0,2,(uint8_t*)temp,8);
			cnt = 0;	
		}break;		
		default:cnt = 0;break;
	}	
}
/*********************************************
//显示主任务
**********************************************/
void User_OledShow(void)
{

	static uint8_t scrren_state = 0;
	
	switch(scrren_state)
	{
		case 0:
		{
			User_OledDisplayStatus(OLED_clear);
			User_OledScreenTitle();
			scrren_state = 1;
		}break;
		case 1:
		{
			User_OledScreenMain();
			if((KEY[0].press_time > 500)||(KEY[1].press_time > 500)||(KEY[2].press_time > 500))
			{
				scrren_state = 2;
				User_OledDisplayStatus(OLED_clear);
			}
		}break;
		case 2:
		{
			User_OledScreenKey();
			if(KEY[0].level*KEY[1].level*KEY[2].level != 0)
			{
				scrren_state = 0;
			}
		}break;
		case 3:
		{
			User_OledScreenMotorTest();
			if(MOVE.motor_en < 3)
			{
				scrren_state = 0;
			}
		}break;
		default:scrren_state = 0;break;
	}
}

