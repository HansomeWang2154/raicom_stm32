/**
IMU��ȡ�Ƕ�����
�ֱ��ȡxyz����Ƕ����ڻ�������̬���ƣ�IMU1.angx��IMU1.angy��IMU1.angz
**/

#include "user_imu.h"
#include "usart.h" 
#include "user_movebase.h"

extern DMA_HandleTypeDef hdma_usart1_rx;
#define USART_IMU1_HUSART huart1
#define USART_IMU1_HDMA_RX hdma_usart1_rx

USART_IMU_TYPEDEF IMU1;

/**
����Ӳ��ѡ�������ͺţ�JY60��JY61��JY62
��ӦƵ�ʣ�JY60(20Hz) < JY62(100Hz) < JY61(200Hz)
�ɱ��۸�JY60(20Hz) < JY62(100Hz) < JY61(200Hz)

����ƽ�⳵����и��ߵĶ�̬��Ӧ��ѡ��JY62
���೵��ֻ�����ƫ���Ǽ�⣬ѡ��JY60����
**/
#define _IMU_JY60 9600//����Ƶ��20Hz
#define _IMU_JY61 230400//����Ƶ��200Hz
#define _IMU_JY62 115200//����Ƶ��100Hz


/**
�������ô��ڲ�����
�����������������������ݻ�������ʵ�ʰ�װ�ͺ�ѡ��_IMU_JY60/_IMU_JY61/_IMU_JY62��������������������������
����ƽ�⳵һ��ʹ��JY62�����೵��ʹ��JY60
**/
void User_ImuInit(void)
{               
	USART_IMU1_HUSART.Init.BaudRate = _IMU_JY61;/**********_IMU_JY60/_IMU_JY61/_IMU_JY62************/

  if (HAL_UART_Init(&USART_IMU1_HUSART) != HAL_OK)
  {
    Error_Handler();
  }
	
	__HAL_UART_ENABLE_IT(&USART_IMU1_HUSART, UART_IT_IDLE);         
	HAL_UART_Receive_DMA(&USART_IMU1_HUSART, (uint8_t*)IMU1.usart_rx_buf, USART_IMU_RX_SIZE);   	
}
/**
���ڿ����жϷ����������յ��ַ����󴥷����жϣ�
**/
void User_Imu1IRQHandler(void)
{
	if(RESET != __HAL_UART_GET_FLAG(&USART_IMU1_HUSART, UART_FLAG_IDLE))   
	{
		__HAL_UART_CLEAR_IDLEFLAG(&USART_IMU1_HUSART);    		
		HAL_UART_DMAStop(&USART_IMU1_HUSART);    
		
    IMU1.rx_data_len  = USART_IMU_RX_SIZE - __HAL_DMA_GET_COUNTER(&USART_IMU1_HDMA_RX);//��ȡ���յ����ַ�������  
		User_Imu1DataParas();//�������ڽ��������е�����
		
		HAL_UART_Receive_DMA(&USART_IMU1_HUSART, (uint8_t*)IMU1.usart_rx_buf, USART_IMU_RX_SIZE);
	}
}
uint8_t User_Imu1AngzOffsetPowerUp(float gyro_z,float angz_derta)
{
	static uint16_t cnt = 0;
	static float sum_angz_derta = 0;
	static float sum_gyro_z = 0;
	
	cnt++;		
	sum_angz_derta += angz_derta;
	sum_gyro_z += gyro_z;
	if(cnt  == 200)//����1sƫ����
	{
		IMU1.angz_derta_bias = sum_angz_derta/cnt;
		IMU1.gyroz_bias = sum_gyro_z/cnt;
		cnt = 0;
		sum_angz_derta = 0;
		sum_gyro_z = 0;
		return 1;
	}
	else
		return 0;
}
void User_Imu1AngzOffsetGet(float gyro_z,float angz_derta)
{
	static uint16_t cnt = 0;
	static float sum_angz_derta = 0;
	static float sum_gyro_z = 0;
	
	if((gyro_z > -0.8) && (gyro_z < 0.8))//���ٶ�����3sС�ڸ�ֵ����Ϊ���Ӿ�ֹ����
	{
		cnt++;		
		sum_angz_derta += angz_derta;
		sum_gyro_z += gyro_z;
		if(cnt  == 400)//����2s���������ƫ����
		{
			IMU1.angz_derta_offset = sum_angz_derta/cnt;
			IMU1.gyroz_offset = sum_gyro_z/cnt;
			cnt = 0;
			sum_angz_derta = 0;
			sum_gyro_z = 0;
		}
	}
	else
	{
		cnt = 0;
		sum_angz_derta = 0;
		sum_gyro_z = 0;
	}
}
/**
����Լ���Ĵ���ͨ��Э�����ݽ��������ղ���
**/
	float ang_z_temp = 0;	
void User_Imu1DataParas(void)
{
	uint8_t i = 0,j = 0;
	uint8_t sum = 0;
	float temp_ang = 0;

	static float gyro_z_temp = 0;
	static float ang_z_derta = 0;
	static float ang_z_last = 0;
	static uint16_t cnt = 0; 
	static uint16_t filter_state = 0; 
	
	for(i=0;i<IMU1.rx_data_len;i++)
	{
		if((IMU1.usart_rx_buf[i] == 0x55) && (IMU1.usart_rx_buf[i+1] == 0x51))//acc data
		{
			sum = 0;
			for(j=i;j<i+10;j++)
			{
				sum += IMU1.usart_rx_buf[j];
			}
			if(sum == IMU1.usart_rx_buf[j])
			{
				/*attention:"<<" priority is lower than "|" !!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
				IMU1.accx = ((int16_t)(IMU1.usart_rx_buf[i+3]<<8) | IMU1.usart_rx_buf[i+2])/32768.0f*16;//g
				IMU1.accy = ((int16_t)(IMU1.usart_rx_buf[i+5]<<8) | IMU1.usart_rx_buf[i+4])/32768.0f*16;//g
				IMU1.accz = ((int16_t)(IMU1.usart_rx_buf[i+7]<<8) | IMU1.usart_rx_buf[i+6])/32768.0*16;//g
				IMU1.temper = ((int16_t)(IMU1.usart_rx_buf[i+9]<<8) | IMU1.usart_rx_buf[i+8])/340.0+36.53;//��C				
			}
			else;	
			i = j;	
		}
		else if((IMU1.usart_rx_buf[i] == 0x55) && (IMU1.usart_rx_buf[i+1] == 0x52))//gyro data
		{
			sum = 0;
			for(j=i;j<i+10;j++)
			{
				sum += IMU1.usart_rx_buf[j];
			}
			if(sum == IMU1.usart_rx_buf[j])
			{
				/*attention:"<<" priority is lower than "|" !!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
				IMU1.gyrox = ((int16_t)(IMU1.usart_rx_buf[i+3]<<8) | IMU1.usart_rx_buf[i+2])/32768.0f*2000;//��/s
				IMU1.gyroy = ((int16_t)(IMU1.usart_rx_buf[i+5]<<8) | IMU1.usart_rx_buf[i+4])/32768.0f*2000;//��/s
				gyro_z_temp = ((int16_t)(IMU1.usart_rx_buf[i+7]<<8) | IMU1.usart_rx_buf[i+6])/32768.0f*2000;//��/s	
			}
			else;
			i = j;
		}
		else if((IMU1.usart_rx_buf[i] == 0x55) && (IMU1.usart_rx_buf[i+1] == 0x53))//ang data
		{
			sum = 0;
			for(j=i;j<i+10;j++)
			{
				sum += IMU1.usart_rx_buf[j];
			}
			if(sum == IMU1.usart_rx_buf[j])
			{
				/*attention:"<<" priority is lower than "|" !!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
				temp_ang = ((int16_t)(IMU1.usart_rx_buf[i+3]<<8) | IMU1.usart_rx_buf[i+2])/32768.0f*180;//0~360��
				IMU1.angx = (temp_ang>180)?(temp_ang-360):temp_ang;//-180~180��
				temp_ang = ((int16_t)(IMU1.usart_rx_buf[i+5]<<8) | IMU1.usart_rx_buf[i+4])/32768.0f*180;//0~360��
				IMU1.angy = (temp_ang>180)?(temp_ang-360):temp_ang;//-180~180��
				temp_ang = ((int16_t)(IMU1.usart_rx_buf[i+7]<<8) | IMU1.usart_rx_buf[i+6])/32768.0f*180;//0~360��
				ang_z_temp = (temp_ang>180)?(temp_ang-360):temp_ang;//-180~180��
				
				//Ϊʹƫ���Ƿ�Χ�����������´���
				ang_z_derta = ang_z_temp - ang_z_last;//�Ƕ�����
				if(ang_z_derta > 180)
					ang_z_derta -= 360;
				else if(ang_z_derta < -180)
					ang_z_derta += 360;
				ang_z_last = ang_z_temp;
				
				switch(filter_state)
				{
					case 0:
					{
						cnt++;
						if(cnt == 100)
							filter_state++;
					}break;
					case 1:
					{
						if(User_Imu1AngzOffsetPowerUp(gyro_z_temp,ang_z_derta) == 1)
							filter_state++;
					}break;
					case 2:
					{			
						IMU1.gyroz = gyro_z_temp - IMU1.gyroz_bias -  IMU1.gyroz_offset;
						IMU1.angz += ang_z_derta - IMU1.angz_derta_bias - IMU1.angz_derta_offset;//�Ƕ������Ļ��ּ��Ƕȡ�
						//������Ʈ�������С����ֹʱ�ɼ���Ʈ���ݣ����ڴ˼�����Ʈƫ�ƣ���������
						User_Imu1AngzOffsetGet(IMU1.gyroz,ang_z_derta - IMU1.angz_derta_bias);
						
					}break;
					default:filter_state = 0;break;
				}
			}
			else;
			i = j;
		}
	}
	
	
}



