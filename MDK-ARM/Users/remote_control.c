//#include "remote_control.h"
//#include "user_movebase.h"
//#include "user_tasks.h"
//#include "user_scissor.h"
//#include "user_m3508.h"
//#include "user_can_motor.h"
//#define USART_REMOTE huart3
//uint8_t rebuffer[1];
//uint16_t sbusbuffer[25];
//unsigned int remote_i=0;
//uint8_t head=0X0f;                   // 
//int16_t ch[16];                // 
//uint8_t flag=0x2f;                   // 
//uint8_t end=0x00;                    // 
//uint8_t rxdat;
//uint16_t rx_len;
//int ch4_last=1;
//int ch4_now=1;
//uint8_t rx_buffer[25];
//int main_control=0;//模式切换
//int sci_control=0;//剪刀切换
//int rocker_control=0;//摇杆切换
//extern DMA_HandleTypeDef hdma_usart3_rx;
//#define RightRocker_Horizontal ch[0]    //left:173   center:993    right:1809 水平
//#define RightRocker_Vertical ch[1]		 //173	   center:999  	1809            垂直
//#define LeftRocker_Vertical ch[2]	     //up:1812 	   center:1012  	down:173
//#define LeftRocker_Horizontal ch[3]     //left:173 	 center:993  	right:1809
//void User_RemoteInit(void)
//{               	
//	
//  if (HAL_UART_Init(&USART_REMOTE) != HAL_OK)
//  {
//    Error_Handler();
//  }
//	
//	__HAL_UART_ENABLE_IT(&USART_REMOTE, UART_IT_IDLE);
//	HAL_UART_Receive_DMA(&USART_REMOTE, rx_buffer,25);
////	HAL_UART_Receive_IT(&huart3,&rxdat,1);
//}
//void sbus_process ()
//{
//	if(rx_buffer[0]==head&&rx_buffer[24]==end)
//	{
//		ch[0] =((rx_buffer[2]<<8)  + (rx_buffer[1])) & 0x07ff;          
//		ch[1] =((rx_buffer[3]<<5)  + (rx_buffer[2]>>3)) & 0x07ff;
//		ch[2] =((rx_buffer[5]<<10) + (rx_buffer[4]<<2) + (rx_buffer[3]>>6)) & 0x07ff;
//		ch[3] =((rx_buffer[6]<<7)  + (rx_buffer[5]>>1)) & 0x07ff;
//		ch[4] =((rx_buffer[7]<<4)  + (rx_buffer[6]>>4)) & 0x07ff;
//		ch[5] =((rx_buffer[9]<<9)  + (rx_buffer[8]<<1) + (rx_buffer[7]>>7)) & 0x07ff; //左边第二个拨码 
//		ch[6] =((rx_buffer[10]<<6) + (rx_buffer[9]>>2)) & 0x07ff;//右边第二个拨码
//		ch[7] =((rx_buffer[11]<<3) + (rx_buffer[10]>>5)) & 0x07ff;//右边第一个拨码
//		ch[8] =((rx_buffer[13]<<8)  + rx_buffer[12]) & 0x07ff;
//		ch[9] =((rx_buffer[14]<<5)  + (rx_buffer[13]>>3)) & 0x07ff;
//		ch[10]=((rx_buffer[16]<<10) + (rx_buffer[15]<<2) + (rx_buffer[14]>>6)) & 0x07ff;
//		ch[11]=((rx_buffer[17]<<7)  + (rx_buffer[16]>>1)) & 0x07ff;
//		ch[12]=((rx_buffer[18]<<4)  + (rx_buffer[17]>>4)) & 0x07ff;
//		ch[13]=((rx_buffer[20]<<9)  + (rx_buffer[19]<<1) + (rx_buffer[18]>>7)) & 0x07ff;
//		ch[14]=((rx_buffer[21]<<6) + (rx_buffer[20]>>2)) & 0x07ff;
//		ch[15]=((rx_buffer[22]<<3) + (rx_buffer[21]>>5)) & 0x07ff;
//		if(ch[4]<1000) sci_control=0;
//		else if(ch[4]>=1000) sci_control=1;
//		if(ch[5]<200) main_control=0;
//		else if(ch[5]>=200) main_control=1;
////		else if(ch[5]>1300) main_control=2;
//		if(ch[6]<200) rocker_control=0;
//		else if(ch[6]>=200&&ch[6]<=1100) rocker_control=1;
//		else  rocker_control=2;
//		if(ch[4]<1000) ch4_now=1;
//		else if(ch[4]>=1000) ch4_now=2;
//		if(ch4_now!=ch4_last)
//		{
//			CAN_SendCurGroup3508(0x200,0,0,0,0);
//			ch4_last =ch4_now;
//		}
////		if(ch4_now!=ch4_last)
////		{
////		if(ch4_last<ch4_now)
////		{
////			Scissor.control=1;
////			CAN_SendCurGroup3508(0x200,0,0,0,0);
////			MOVE.motor_en=2;
////		}
////		else if(ch4_last>ch4_now)
////		{
////			Scissor.control=0;
////			CAN_SendCurGroup3508(0x200,0,0,0,0);
////			MOVE.motor_en=2;
////		}
////		ch4_last =ch4_now;
////	}
//}
//}
//int flag_con=0;
//void User_Remote_Control_IRQHandler(void)
//{
//	if(RESET != __HAL_UART_GET_FLAG(&USART_REMOTE, UART_FLAG_IDLE))   
//	{
//		__HAL_UART_CLEAR_IDLEFLAG(&USART_REMOTE); 
//		HAL_UART_DMAStop(&USART_REMOTE);    
//		
//		rx_len  = 25 - __HAL_DMA_GET_COUNTER(&hdma_usart3_rx);//获取接收到的字符串长度  
//		sbus_process();//解析数据
//		
//		HAL_UART_Receive_DMA(&USART_REMOTE,rx_buffer,25);
//	}
//}