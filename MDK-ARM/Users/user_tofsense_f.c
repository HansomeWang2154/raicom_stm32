#include "user_tofsense_f.h"
#include "cmsis_os.h" 

TOFSENSE_TYPEDEF tofsense;
extern DMA_HandleTypeDef hdma_usart2_rx;

void User_TOFSenseInit(void)
{               
	
  if (HAL_UART_Init(&tofsense_huart) != HAL_OK)
  {
    Error_Handler();
  }
	
	__HAL_UART_ENABLE_IT(&tofsense_huart, UART_IT_IDLE);
	HAL_UART_Receive_DMA(&tofsense_huart, tofsense.rxbuf, 16);
	
}

//USART2中断
void User_TOFSenseIRQHandler(void)
{
	if(RESET != __HAL_UART_GET_FLAG(&tofsense_huart, UART_FLAG_IDLE))   
	{
		__HAL_UART_CLEAR_IDLEFLAG(&tofsense_huart); 
		HAL_UART_DMAStop(&tofsense_huart);    
		
		tofsense.rxdata_len  = tofsense_buf_len - __HAL_DMA_GET_COUNTER(&hdma_usart2_rx);//获取接收到的字符串长度  
//		User_TOFSenseDataParas();//解析数据
		
		HAL_UART_Receive_DMA(&tofsense_huart, tofsense.rxbuf,16);
	}
}

////数据解析
//void User_TOFSenseDataParas(void)
//{
//	if((tofsense.rxdata_len >= 16) && (tofsense.rxbuf[0] == 0x57) && (tofsense.rxbuf[11] == 0x00))
//	{
//		uint8_t sum_check = 0;
//		for(uint8_t i=0; i<15 ;i++)
//		{
//			sum_check += tofsense.rxbuf[i];
//		}
//		if(sum_check == tofsense.rxbuf[15])
//		{
//			uint8_t id = tofsense.rxbuf[3];
//			tofsense.dis[id] = (tofsense.rxbuf[10] << 16) | (tofsense.rxbuf[9] << 8) | tofsense.rxbuf[8];
//			tofsense.dis_update_flag |= (1<<id);
//		}
//	}
//}

//void User_TOFSense_Get(uint8_t id)
//{
//	tofsense.dis_update_flag &= (0xfeff >>(8-id));
//	tofsense.rxbuf[4] = id;
//	tofsense.rxbuf[7] = 0x63 + id;
//	HAL_UART_Transmit_DMA(&tofsense_huart, tofsense.rxbuf, tofsense.txdata_len);
//}

//uint8_t User_TOFSense_GetAll(void)//较长延时时间，非必要不使用
//{
//	tofsense.dis_update_flag = 0;
//	for(uint8_t id=0;id<tofsense_num;id++)
//	{
//		User_TOFSense_Get(id);
//		osDelay(10);
//		if(!(tofsense.dis_update_flag & (1<<id)))
//			return 0;
//	}
//	return 1;
//}
