/*********************************************
//��ݮ��ͨ����ع��ܣ��磺ROSͨ�š�����ͷ������ݴ��䡭����
��ݮ��ʹ�ô���4���볬����ģ�鴮�ڳ�ͻ
������δʹ����ݮ�ɣ��ʸ��ļ���Ч
**********************************************/
#include "user_comm_raspberry.h" 
#include "usart.h"
#include "user_movebase.h"
#include "user_comm_wifi.h"
#include "cmsis_os.h"
#include "string.h"
#include "stdio.h"
#include "stdlib.h"
#include "user_can_motor.h" 

USART_SMP_TYPEDEF SMP;


extern DMA_HandleTypeDef hdma_usart3_rx; 

#define USART_SMP_HUSART huart3
#define USART_SMP_HDMA_RX hdma_usart3_rx    
 
/*********************************************
//��ʼ��
**********************************************/
void User_CommSmpInit(void)
{               		
	__HAL_UART_CLEAR_IDLEFLAG(&USART_SMP_HUSART);
	__HAL_UART_ENABLE_IT(&USART_SMP_HUSART, UART_IT_IDLE);         
	HAL_UART_Receive_DMA(&USART_SMP_HUSART, (uint8_t*)SMP.usart_rx_buf, USART_SMP_RX_SIZE); 

	SMP.init_ok = 1;
	SMP.rev_cnt = 0;
}
/*********************************************
//���ڽ����ж�
**********************************************/
void User_CommSmpIRQHandler(void)
{	
	if(RESET != __HAL_UART_GET_FLAG(&USART_SMP_HUSART, UART_FLAG_IDLE))   
	{		
		__HAL_UART_CLEAR_IDLEFLAG(&USART_SMP_HUSART);    		
		HAL_UART_DMAStop(&USART_SMP_HUSART);                                                        
    SMP.rx_data_len  = USART_SMP_RX_SIZE - __HAL_DMA_GET_COUNTER(&USART_SMP_HDMA_RX);   
		User_CommRosToStmDataParas();///////
		HAL_UART_Receive_DMA(&USART_SMP_HUSART, (uint8_t*)SMP.usart_rx_buf, USART_SMP_RX_SIZE);
	} 		
} 
/*********************************************
//���ݽ���
����ROS�����ٶȡ����ٶȿ���ָ��
����ROS��״̬��Ϣ
**********************************************/
//void User_CommRosToStmDataParas(void)
//{
//	if(	 (SMP.usart_rx_buf[0] == 'R') 
//		&& (SMP.usart_rx_buf[1] == 'O')
//		&& (SMP.usart_rx_buf[2] == 'S')
//		&& (SMP.usart_rx_buf[3] == ':')
//		&& (SMP.usart_rx_buf[104] == '>')
//		&& (SMP.usart_rx_buf[105] == 'S')
//		&& (SMP.usart_rx_buf[106] == 'T')
//		&& (SMP.usart_rx_buf[107] == 'M')
//		&& (SMP.usart_rx_buf[108] == '\r')
//		&& (SMP.usart_rx_buf[109] == '\n'))////////////////////////////////////////���յ��ٶȿ���ָ��
//	{	
//		memcpy(&SMP.ros_stm, &SMP.usart_rx_buf[4],100);
//	}		
//}
void User_CommRosToStmDataParas(void)
{
	if(	 (SMP.usart_rx_buf[0] == 0x3f) 
		&& (SMP.usart_rx_buf[1] == 0x21)
		&& (SMP.usart_rx_buf[2] == 0x01)
		&& (SMP.usart_rx_buf[7] == 0x21))////////////////////////////////////////���յ��ٶȿ���ָ��
	{	
		SMP.ros_stm.cmd_vx = -((int)SMP.usart_rx_buf[3] - 128)/128.0f;
		SMP.ros_stm.cmd_vy = -((int)SMP.usart_rx_buf[4] - 128)/128.0f;
		SMP.ros_stm.cmd_womiga = -((int)SMP.usart_rx_buf[5] - 128)/128.0f;
		
		//��һ���ٶ�
	}		
} 
/*********************************************
//���ݷ���
������ROS�������Ϣ��λ�á����ٶȡ����ٶ�
**********************************************/
void User_CommStmToRosSend(void)//
{	
	SMP.stm_ros.odom_px = MOVE.odometer.pos_x;
	SMP.stm_ros.odom_py = MOVE.odometer.pos_y;
	SMP.stm_ros.odom_ang = MOVE.odometer.ang_rad;
	//SMP.stm_ros.odom_vx = MOVE.odometer.v_x;
	//SMP.stm_ros.odom_vy = MOVE.odometer.v_y;
	SMP.stm_ros.odom_womoga = MOVE.odometer.v_angular;
	
	
	SMP.usart_tx_buf[0] = 'S';
	SMP.usart_tx_buf[1] = 'T';
	SMP.usart_tx_buf[2] = 'M';
	SMP.usart_tx_buf[3] = ':';
	
	memcpy(&SMP.usart_tx_buf[4],&SMP.stm_ros,100);
		
	SMP.usart_tx_buf[104] = '>';
	SMP.usart_tx_buf[105] = 'R';
	SMP.usart_tx_buf[106] = 'O';
	SMP.usart_tx_buf[107] = 'S';
	SMP.usart_tx_buf[108] = '\r';
	SMP.usart_tx_buf[109] = '\n';
	
	HAL_UART_Transmit_DMA(&USART_SMP_HUSART,(uint8_t*)SMP.usart_tx_buf,110);
	
//	sprintf((char*)SMP.usart_tx_buf,"odom:%.3f;%.3f;%.3f;%.3f;%.3f;\r\n",MOVE.odometer.pos_x,MOVE.odometer.pos_y,MOVE.odometer.ang_rad,MOVE.odometer.v_linear,MOVE.odometer.v_angular);	
//	HAL_UART_Transmit_DMA(&USART_SMP_HUSART,(uint8_t*)SMP.usart_tx_buf,strlen((char*)SMP.usart_tx_buf));
}

void User_CommSmpSendTask(void)//2ms
{	
	static uint8_t cnt = 0;
	
	cnt++;
	
	if(cnt%10 == 0)//20ms-50Hz��
	{
		User_CommStmToRosSend();
		cnt = 0;
	}
}



