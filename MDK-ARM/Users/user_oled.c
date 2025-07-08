/**
OLED������������ʾ����
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



//IIC���ʿ����õ�1500000


//OLED���Դ� 128*32����
//��Ÿ�ʽ����.
//[0]0 1 2 3 ... 127//��0ҳ��128�У�ÿ��8��	
//[1]0 1 2 3 ... 127//��1ҳ��128�У�ÿ��8��		
//[2]0 1 2 3 ... 127//��2ҳ��128�У�ÿ��8��		
//[3]0 1 2 3 ... 127//��3ҳ��128�У�ÿ��8��		
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
	
	User_OledWriteByte(0xAE,OLED_cmd);//�ر���ʾ 
	
	User_OledWriteByte(0x40,OLED_cmd);//---set low column address
	User_OledWriteByte(0xB0,OLED_cmd);//---set high column address

	User_OledWriteByte(0xC8,OLED_cmd);//-not offset

	User_OledWriteByte(0x81,OLED_cmd);//���öԱȶ�
	User_OledWriteByte(0xff,OLED_cmd);

	User_OledWriteByte(0xa1,OLED_cmd);//���ض�������

	User_OledWriteByte(0xa6,OLED_cmd);//
	
	User_OledWriteByte(0xa8,OLED_cmd);//��������·��
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
//��ʾ״̬
**********************************************/
void User_OledDisplayStatus(OLED_DISP_MODE mode) 
{
	uint8_t i,n;		
	switch(mode)
	{
		case OLED_on://������ʾ
			User_OledWriteByte(0X8D, OLED_cmd);  //SET DCDC����
			User_OledWriteByte(0X14, OLED_cmd);  //DCDC ON
			User_OledWriteByte(0XAF, OLED_cmd);  //DISPLAY ON
			break;
		case OLED_off://�ر���ʾ  
			User_OledWriteByte(0X8D, OLED_cmd);  //SET DCDC����
			User_OledWriteByte(0X10, OLED_cmd);  //DCDC OFF
			User_OledWriteByte(0XAE, OLED_cmd);  //DISPLAY OFF
			break;
		case OLED_clear://������������Ļ�Ǻ�ɫ��!��û����һ��!!!		
			for(i = 0; i < 4; i ++)  
			{  
				User_OledWriteByte(0xB0 + i, OLED_cmd);    //����ҳ��ַ(0~3)
				User_OledWriteByte(0x00, OLED_cmd);      //������ʾλ�á��е͵�ַ
				User_OledWriteByte(0x10, OLED_cmd);      //������ʾλ�á��иߵ�ַ 
				for(n = 0; n < _OLED_MAX_COLUMN; n++)
				{
					User_OledWriteByte(0, OLED_data); 
				}
			} 
			break;
		case OLED_test:
			for(i = 0; i < 4; i ++)  
			{  
				User_OledWriteByte(0xb0 + i, OLED_cmd);    //����ҳ��ַ(0~3)
				User_OledWriteByte(0x00, OLED_cmd);      //������ʾλ�á��е͵�ַ
				User_OledWriteByte(0x10, OLED_cmd);      //������ʾλ�á��иߵ�ַ 
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
//��������
**********************************************/
void User_OledSetPos(unsigned char x, unsigned char y) 
{ 	
	User_OledWriteByte(0xb0+y,OLED_cmd);
	User_OledWriteByte(((x&0xf0)>>4)|0x10,OLED_cmd);
	User_OledWriteByte((x&0x0f),OLED_cmd); 
}
/**********************************************
//��ʾ����
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
//��ָ��λ����ʾһ���ַ�,���������ַ�
//x:0~127
//y:0~4
//mode:0,������ʾ;1,������ʾ				 
//size:ѡ������ 16/8 
**********************************************/
void User_OledShowChar(uint8_t x,uint8_t y,uint8_t chr,uint8_t Char_Size)
{      	
	unsigned char c=0,i=0;	
	c=chr-' ';//�õ�ƫ�ƺ��ֵ			
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
//��ʾ�ַ���
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
/***********������������ʾ��ʾBMPͼƬ128��64��ʼ������(x,y),x�ķ�Χ0��127��yΪҳ�ķ�Χ0��7*****************/
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
                                                                                     �Զ�����ʾ����
************************************************************************************************************************************************************/
/*********************************************
//��ʾ����
**********************************************/
void User_OledScreenTitle(void)
{
	User_OledShowString(25,0,(uint8_t*)"*Group:11*",8);//����-Эͬ���-����
}
/*********************************************
//��ʾ��ҳ��
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
//��ʾ����״̬ҳ��
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
//��ʾ�������ҳ��
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
//��ʾ������
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

