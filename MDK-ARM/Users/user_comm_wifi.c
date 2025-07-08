/*********************************************
//WIFI无线通信相关功能
**********************************************/
#include "user_comm_wifi.h" 
#include "usart.h"

#include "string.h"
#include "stdio.h"
#include "stdlib.h"

#include "user_math.h"
#include "user_imu.h"
#include "user_movebase.h"
#include "user_tasks.h"
#include "user_state.h"

#define USART_WIFI_HUSART huart5 //f103串口5无DMA功能，只能单字节发送/接收

USART_WIFI_TYPEDEF WIFI;

/*
问题现象：系统重新上电后stm32串口无法接收数据，单独复位stm32后可正常接收（调试状态下可正常接收）？？？？

问题分析：该模块上电后会自动发送模块自检信息，stm32若在接收这些串口信息时进行初始化则可能导致无法正常工作！！！

解决办法：延时一段时间后再对模块对应串口的初始化！！！！！
*/
/*********************************************
//初始化
**********************************************/
void User_CommWifiInit(void)
{               	
	__HAL_UART_ENABLE_IT(&USART_WIFI_HUSART, UART_IT_RXNE);   
	__HAL_UART_ENABLE_IT(&USART_WIFI_HUSART, UART_IT_IDLE);	
}
/*********************************************
//存储接收数据到缓存数组
//f103串口5无DMA功能，只能单字节发送/接收
**********************************************/
void User_CommWifiDataPush(void)
{
	WIFI.usart_rx_buf[WIFI.rx_data_len] = UART5->DR;
	WIFI.rx_data_len++;
}

/*********************************************
//串口接收中断
**********************************************/
void User_CommWifiIRQHandler(void)
{	
	if(RESET != __HAL_UART_GET_FLAG(&USART_WIFI_HUSART, UART_FLAG_RXNE))   
	{		
		__HAL_UART_CLEAR_NEFLAG(&USART_WIFI_HUSART);  
		
		User_CommWifiDataPush();//单字节存储到数组	
	} 	
	if(RESET != __HAL_UART_GET_FLAG(&USART_WIFI_HUSART, UART_FLAG_IDLE))   
	{
		__HAL_UART_CLEAR_IDLEFLAG(&USART_WIFI_HUSART);    				                                                         
		User_CommWifiDataParas();//接收连续字符串结束后（串口空闲）解析数据
		WIFI.rx_data_len = 0;
	}	
}
/*********************************************
//接收数据解析
**********************************************/
void User_CommWifiDataParas(void)
{
	uint8_t i = 0,j = 0;
	char str1[2] = {0};
	char str2[2] = {0};
	
	for(i=0;i<WIFI.rx_data_len;i++)
	{
		if((WIFI.usart_rx_buf[i] == '#') && (WIFI.usart_rx_buf[i+1] != '#'))//收到#开头，且下一个字符不是#，表示一帧数据的开始
		{	
			i++;
			for(j=0;WIFI.usart_rx_buf[j+i] != ';';j++)//提取以;隔断的字符串
			{
				str1[j] = WIFI.usart_rx_buf[j+i];
			}
			i=i+j+1;
			for(j=0;WIFI.usart_rx_buf[j+i] != '#';j++)//提取以#隔断的字符串
			{
				str2[j] = WIFI.usart_rx_buf[j+i];
			}
			WIFI.handle.left_x = (int8_t)atof(str1);//字符串转换为数值
			WIFI.handle.left_y = -(int8_t)atof(str2);//字符串转换为数值
			return;
		}
		else if((WIFI.usart_rx_buf[i] == '*') && (WIFI.usart_rx_buf[i+1] != '*'))//收到*开头，且下一个字符不是*，表示一帧数据的开始
		{	
			i++;
			for(j=0;WIFI.usart_rx_buf[j+i] != ';';j++)//提取以;隔断的字符串
			{
				str1[j] = WIFI.usart_rx_buf[j+i];
			}
			i=i+j+1;
			for(j=0;WIFI.usart_rx_buf[j+i] != '*';j++)//提取以#隔断的字符串
			{
				str2[j] = WIFI.usart_rx_buf[j+i];
			}
			WIFI.handle.right_x = (int8_t)atof(str1);//字符串转换为数值
			WIFI.handle.right_y = -(int8_t)atof(str2);//字符串转换为数值	
			return;
		}
		else if((WIFI.usart_rx_buf[i] == '%')&&(WIFI.usart_rx_buf[i+1] != '%'))
		{
			i++;
			for(j=0;WIFI.usart_rx_buf[j+i] != ';';j++)//提取以;隔断的字符串
			{
				str1[j] = WIFI.usart_rx_buf[j+i];		
			}
			i=i+j+1;
			for(j=0;WIFI.usart_rx_buf[j+i] != '%';j++)
			{
				str2[j] = WIFI.usart_rx_buf[j+i];
			}
			return;
		}
	}
}

void User_send(void)
{
//	if(equation(STATE.location_now, STATE.location_target) && (STATE.in_now == STATE.in_target))
//			HAL_UART_Transmit(&USART_WIFI_HUSART, (uint8_t*)ok, 4, 10);
//		else
//			HAL_UART_Transmit(&USART_WIFI_HUSART, (uint8_t*)no, 4, 10);
}

/*********************************************
//发送5个int16数据到网络
**********************************************/
void User_CommWifi_Int16Data(int16_t data1,int16_t data2,int16_t data3,int16_t data4,int16_t data5)//
{
	uint8_t i = 0;
	uint8_t sum_check = 0;
	uint8_t add_check = 0;
	UNION_INT16 send_int16_1,send_int16_2,send_int16_3,send_int16_4,send_int16_5;
	
	send_int16_1.data = data1;
	send_int16_2.data = data2;
	send_int16_3.data = data3; 
	send_int16_4.data = data4;
	send_int16_5.data = data5;
	
	WIFI.usart_tx_buf[0] = 0xAA;//帧头
	WIFI.usart_tx_buf[1] = 0xFF;//帧头
	WIFI.usart_tx_buf[2] = 0xF1;//帧头
	WIFI.usart_tx_buf[3] = 10;//数据字节数
	
	WIFI.usart_tx_buf[4] = send_int16_1.buf[0];
	WIFI.usart_tx_buf[5] = send_int16_1.buf[1];
	WIFI.usart_tx_buf[6] = send_int16_2.buf[0];
	WIFI.usart_tx_buf[7] = send_int16_2.buf[1];
	WIFI.usart_tx_buf[8] = send_int16_3.buf[0];
	WIFI.usart_tx_buf[9] = send_int16_3.buf[1];
	WIFI.usart_tx_buf[10] = send_int16_4.buf[0];
	WIFI.usart_tx_buf[11] = send_int16_4.buf[1];
	WIFI.usart_tx_buf[12] = send_int16_5.buf[0];
	WIFI.usart_tx_buf[13] = send_int16_5.buf[1];

	for(i=0;i<(WIFI.usart_tx_buf[3]+4);i++)
	{
		sum_check += WIFI.usart_tx_buf[i];//求和校验
		add_check += sum_check;//附加校验
	}
	WIFI.usart_tx_buf[14] = sum_check;
	WIFI.usart_tx_buf[15] = add_check;
	
//	HAL_UART_Transmit(&USART_WIFI_HUSART, (uint8_t*)WIFI.usart_tx_buf, 16, 10);
}
/*********************************************
//发送任务
**********************************************/
void User_CommWifiSendTask(void)
{
	User_CommWifi_Int16Data(MOVE.angz_pid.target,IMU1.angx,IMU1.angy,IMU1.angz,0);
}


