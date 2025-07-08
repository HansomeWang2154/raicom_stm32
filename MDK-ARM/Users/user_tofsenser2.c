#include "user_tofsenser2.h"
uint8_t tof2_rxbuffer[50];
uint8_t tof2_rxlen;
uint32_t dis1=0;
uint32_t dis2=0;
uint32_t dis3=0;
uint32_t dis4=0;
uint32_t dis5=0;
uint32_t dis3_remain=0;

extern DMA_HandleTypeDef hdma_uart4_rx;
void Tofsenser_Paras()//数据处理
{
	if(tof2_rxlen>0)
	{
		for(int i=0;i<50;i++)
		{
			if(tof2_rxbuffer[i]=='@'&&tof2_rxbuffer[i+1]==' '&&tof2_rxbuffer[i+7]==0x0D&&tof2_rxbuffer[i+8]==0x0A)
			{
				dis1=(tof2_rxbuffer[i+2]<<16)+(tof2_rxbuffer[i+4]<<8)+(tof2_rxbuffer[i+6]);
			}
			if(tof2_rxbuffer[i]=='#'&&tof2_rxbuffer[i+1]==' '&&tof2_rxbuffer[i+7]==0x0D)
			{
				dis2=(tof2_rxbuffer[i+2]<<16)+(tof2_rxbuffer[i+4]<<8)+(tof2_rxbuffer[i+6]);
			}
			if(tof2_rxbuffer[i]=='!'&&tof2_rxbuffer[i+1]==' '&&tof2_rxbuffer[i+7]==0x0D)
			{
				dis3=(tof2_rxbuffer[i+2]<<16)+(tof2_rxbuffer[i+4]<<8)+(tof2_rxbuffer[i+6]);
				if(dis3!=0) dis3_remain=dis3;
				else if(dis3==0) dis3=dis3_remain;
			}
			if(tof2_rxbuffer[i]=='^'&&tof2_rxbuffer[i+1]==' '&&tof2_rxbuffer[i+7]==0x0D&&tof2_rxbuffer[i+8]==0x0A)
			{
				dis4=(tof2_rxbuffer[i+2]<<16)+(tof2_rxbuffer[i+4]<<8)+(tof2_rxbuffer[i+6]);
			}
			if(tof2_rxbuffer[i]=='&'&&tof2_rxbuffer[i+1]==' '&&tof2_rxbuffer[i+7]==0x0D&&tof2_rxbuffer[i+8]==0x0A)
			{
				dis5=(tof2_rxbuffer[i+2]<<16)+(tof2_rxbuffer[i+4]<<8)+(tof2_rxbuffer[i+6]);
			}
		}
	}
}	
void User_Tofsenser2Init(void)
{               	
	
  if (HAL_UART_Init(&huart4) != HAL_OK)
  {
    Error_Handler();
  }
	
	__HAL_UART_ENABLE_IT(&huart4, UART_IT_IDLE);
	HAL_UART_Receive_DMA(&huart4, tof2_rxbuffer,50);
}
void User_Tofsenser2_IRQHandler(void)
{
	if(RESET != __HAL_UART_GET_FLAG(&huart4, UART_FLAG_IDLE))   
	{
		__HAL_UART_CLEAR_IDLEFLAG(&huart4); 
		HAL_UART_DMAStop(&huart4);    
		
		tof2_rxlen  = __HAL_DMA_GET_COUNTER(&hdma_uart4_rx);//获取接收到的字符串长度  
		Tofsenser_Paras();
		HAL_UART_Receive_DMA(&huart4,tof2_rxbuffer,50);
	}
}