/*********************************************
//���CAN��������

ע�⣡����������
Ϊ�ṩ׼ȷCAN����ʱ�ӣ�����оƬ��Ҫ����������ù�����������ܳ���CAN�����޷�����ͨ�ŵ������
**********************************************/

/*********************************************
CAN�ӿڣ�CAN2.0B  
CAN���ʣ�1Mbps  
ID��ʽ����չ��ʽ-EXID��29bit�� 
֡���ͣ�����֡
λʱ�䣺SYNC-1  BS1-8  BS2-7
����������������ID
Э���ʽ��
	ID--29bit��
		Device-Type:bit28:23
		Info-Source:bit22
		Command:		bit21:18
		Command-Ex:	bit17:15
		Motor-Group:bit14:8
		Motor-ID:		bit7:0
	Data--8Bytes��
**********************************************/

#include "user_can_motor.h" 
#include "can.h"


#define _CHASS_HCAN hcan

USR_CAN_COM CAN_CHASS_COM;
CHASS_MOTOR_TYPE CHASS_MOTOR[_CHASS_MOTOR_NUM];

MOTOR_3508_TYPE MOTOR_3508[8];
/*********************************************
//��ʼ��
**********************************************/
void User_CanMotorInit(void)
{	
	CHASS_MOTOR[0].m_id = 1;
	CHASS_MOTOR[1].m_id = 2;
	CHASS_MOTOR[2].m_id = 3;
	CHASS_MOTOR[3].m_id = 4;
	
	User_Can1FilterInit();
	
//	__HAL_CAN_CLEAR_FLAG(&hcan, CAN_FLAG_RQCP0);
//	HAL_CAN_ActivateNotification(&hcan, CAN_IT_TX_MAILBOX_EMPTY);//������������ж�	
}

/*********************************************
CAN1�˲�������
**********************************************/
void User_Can1FilterInit(void)
{
	CAN_FilterTypeDef  sFilterConfig;
	
	/* Configure the CAN Filter */
  sFilterConfig.FilterBank = 0;
  sFilterConfig.FilterMode = CAN_FILTERMODE_IDMASK;
  sFilterConfig.FilterScale = CAN_FILTERSCALE_32BIT;
  sFilterConfig.FilterIdHigh = 0x0000;
  sFilterConfig.FilterIdLow = 0x0000;
  sFilterConfig.FilterMaskIdHigh = 0x0000;
  sFilterConfig.FilterMaskIdLow = 0x0000;
  sFilterConfig.FilterFIFOAssignment = CAN_RX_FIFO0;//ӳ�䵽FIFO0/1
  sFilterConfig.FilterActivation = ENABLE;
  sFilterConfig.SlaveStartFilterBank = 14;//ֻ��һ��can��оƬ�ò��������� 
  if (HAL_CAN_ConfigFilter(&_CHASS_HCAN, &sFilterConfig) != HAL_OK)
  {
    /* Filter configuration Error */
    Error_Handler();
  }
	
  /* Start the CAN peripheral */
  if (HAL_CAN_Start(&_CHASS_HCAN) != HAL_OK)
  {
    /* Start Error */
    Error_Handler();
  }
  /* Activate CAN RX notification */
	if (HAL_CAN_ActivateNotification(&_CHASS_HCAN, CAN_IT_RX_FIFO0_MSG_PENDING) != HAL_OK)
  {
    /* Notification Error */
    Error_Handler();
  }
}
/*********************************************
��������ж�
**********************************************/
void HAL_CAN_TxMailbox0CompleteCallback(CAN_HandleTypeDef *hcan)
{
	//HAL_CAN_DeactivateNotification(hcan, CAN_IT_TX_MAILBOX_EMPTY);//�رշ�������ж�
	CAN_CHASS_COM.flag_tx_ok = 1;
}
/*********************************************
CAN1�����ж�
**********************************************/
void User_Can1RxIRQHandler(void)
{	
	if(HAL_CAN_GetRxMessage(&_CHASS_HCAN, CAN_RX_FIFO0, &CAN_CHASS_COM.rx_header, CAN_CHASS_COM.rx_buf) == HAL_OK)
  {    																		
		User_Can1RxMessageHandler();		
  }	
  else
	{				
		Error_Handler();/* Reception Error */
	}
}
/*********************************************
CAN1�����жϣ��������Ե���ķ������ϱ�����Ϣ
	ID--29bit��
		Device-Type:bit28:23
		Info-Source:bit22
		Command:		bit21:18
		Command-Ex:	bit17:15
		Motor-Group:bit14:8
		Motor-ID:		bit7:0
**********************************************/
void User_Can1RxMessageHandler(void)
{
	uint8_t i = 0;
	if(CAN_CHASS_COM.rx_header.IDE == CAN_ID_STD)//�յ���׼֡������M3508�������
	{
		uint8_t std_id_3508 = (uint8_t)(CAN_CHASS_COM.rx_header.StdId - 0x0201);
		if(std_id_3508 < 8)
		{
			MOTOR_3508[std_id_3508].pos_now = ((int16_t)CAN_CHASS_COM.rx_buf[0]<<8) | (int16_t)CAN_CHASS_COM.rx_buf[1];
			MOTOR_3508[std_id_3508].spe_now = ((int16_t)CAN_CHASS_COM.rx_buf[2]<<8) | (int16_t)CAN_CHASS_COM.rx_buf[3];
			MOTOR_3508[std_id_3508].cur_now = ((int16_t)CAN_CHASS_COM.rx_buf[4]<<8) | (int16_t)CAN_CHASS_COM.rx_buf[5];
			MOTOR_3508[std_id_3508].temp_now = CAN_CHASS_COM.rx_buf[6];
			MOTOR_3508[std_id_3508].pos_add = MOTOR_3508[std_id_3508].pos_now - MOTOR_3508[std_id_3508].pos_last;
			MOTOR_3508[std_id_3508].pos_last = MOTOR_3508[std_id_3508].pos_now;
			if(MOTOR_3508[std_id_3508].pos_add > 4096)
			{
				MOTOR_3508[std_id_3508].pos_add  -= 8192;
			}
			else if(MOTOR_3508[std_id_3508].pos_add < -4096)
			{
				MOTOR_3508[std_id_3508].pos_add += 8192;
			}
			MOTOR_3508[std_id_3508].pos_total += MOTOR_3508[std_id_3508].pos_add*360/8192.0f;
		}
		return;
	}
		
	else//�յ���չ֡������С�������
	{
		//	uint8_t device_type 	= (CAN_CHASS_COM.rx_header.ExtId>>23) & 0x3f;
		//	uint8_t info_sourse 	= (CAN_CHASS_COM.rx_header.ExtId>>22) & 0x01;
		uint8_t cmd_type 			= (CAN_CHASS_COM.rx_header.ExtId>>18) & 0x0f;
		//	uint8_t cmd_extend 		= (CAN_CHASS_COM.rx_header.ExtId>>15) & 0x07;
		//	uint8_t motor_group 	= (CAN_CHASS_COM.rx_header.ExtId>>8)  & 0x7f;	
		uint8_t motor_id 		= (CAN_CHASS_COM.rx_header.ExtId>>0) 	& 0xff;
		
		CAN_CHASS_COM.rx_header.ExtId = 0;
		CAN_CHASS_COM.rx_header.StdId = 0;
		for(i=0;i<_CHASS_MOTOR_NUM;i++)
		{
			if(CHASS_MOTOR[i].m_id == motor_id)//�����յ��ĵ��ID���ڵ�i�������ID���ȡ�䷴����Ϣ
			{
				switch(cmd_type)//�жϷ�����Ϣ����
				{
					case _CAN_TX_CMD_ID:;break;
					case _CAN_TX_CMD_ACT://����ִ����Ϣ 
					{
						//���������-3000~3000��mA��
						CHASS_MOTOR[i].flash.item.cur_now = (int16_t)((int16_t)CAN_CHASS_COM.rx_buf[0]<<8) | CAN_CHASS_COM.rx_buf[1];
						//����ٶȣ�-10000~10000��rpm��
						CHASS_MOTOR[i].flash.item.spe_now = (int16_t)((int16_t)CAN_CHASS_COM.rx_buf[2]<<8) | CAN_CHASS_COM.rx_buf[3];
						//���λ�ã�-x~x(3600ppr)
						CHASS_MOTOR[i].flash.item.pos_now = (int32_t)((int32_t)CAN_CHASS_COM.rx_buf[4]<<24)|((int32_t)CAN_CHASS_COM.rx_buf[5]<<16)|((int32_t)CAN_CHASS_COM.rx_buf[6]<<8)|(int16_t)CAN_CHASS_COM.rx_buf[7];
					}break;
					case _CAN_TX_CMD_PARAM://����������Ϣ(δʹ�ã�
					{
						CHASS_MOTOR[i].flash.list[CAN_CHASS_COM.rx_buf[0]] = ((int32_t)CAN_CHASS_COM.rx_buf[1]<<24)|((int32_t)CAN_CHASS_COM.rx_buf[2]<<16)|((int32_t)CAN_CHASS_COM.rx_buf[3]<<8)|(int16_t)CAN_CHASS_COM.rx_buf[4];break;
					}
					case _CAN_TX_CMD_IDLE://����״̬��Ϣ 
					{
						CHASS_MOTOR[i].m_id		 								= CAN_CHASS_COM.rx_buf[0];//���ID
						CHASS_MOTOR[i].m_state						 		= CAN_CHASS_COM.rx_buf[1];//���״̬
																								//= CAN_CHASS_COM.rx_buf[2];//δ����
																								//= CAN_CHASS_COM.rx_buf[3];//δ����
						CHASS_MOTOR[i].m_err						 	 		= CAN_CHASS_COM.rx_buf[4];//���������Ϣ
						CHASS_MOTOR[i].flash.item.volt_now	 	= CAN_CHASS_COM.rx_buf[5];//�����ѹ
						CHASS_MOTOR[i].flash.item.temp_c_now	= CAN_CHASS_COM.rx_buf[6];//���оƬ�¶�
						CHASS_MOTOR[i].flash.item.temp_r_now 	= CAN_CHASS_COM.rx_buf[7];//����ȵ����¶�
					}break;
					default:break;
				}
			}
		}
		return;
	}
}
/*********************************************
//CAN����һ������
**********************************************/
uint8_t CAN_ChassSendMsg(void) 
{	
 	uint32_t tx_mailbox;

	if(HAL_CAN_AddTxMessage(&_CHASS_HCAN, &CAN_CHASS_COM.tx_header, CAN_CHASS_COM.tx_buf, &tx_mailbox) == HAL_OK)
		return 1;//USR_PARAM.flash.item.m_err &= ~(1<<3);
	else	
		return 0;//USR_PARAM.flash.item.m_err = _ERR_CAN; 
}
/*****************************************************************************************************************************************
																								CAN���Ƶ�����ܺ��������ͷŵ��
//��1�飺group=0��ID=1,2,3,4
//��2�飺group=1��ID=5,6,7,8
//��3�飺group=2��ID=9,10,11,12
//����
******************************************************************************************************************************************/
/*********************************************
//�ͷ�ĳ����ѡ�еĵ��
//m0_disable = 1���ͷ�
//m0_disable = 0�����ͷ�
**********************************************/
uint8_t CAN_SendFreeGroup(uint8_t group,uint8_t m0_disable,uint8_t m1_disable,uint8_t m2_disable,uint8_t m3_disable)
{
	uint32_t exid_send = 0; 
	
	exid_send = ((uint32_t)_DEVICE_TYPE<<23) | ((uint32_t)_INFO_SOURCE_HOST<<22)| 
											 ((uint32_t)0x0b<<18)| ((uint32_t)0x00<<15)|
											 ((uint32_t)group<<8) | 0x00;
	
	CAN_CHASS_COM.tx_header.StdId=exid_send>>18;	 // ��׼��ʶ��
	CAN_CHASS_COM.tx_header.ExtId=exid_send;	 // ������չ��ʾ����29λ��
	CAN_CHASS_COM.tx_header.IDE=CAN_ID_EXT;		  // ʹ����չ��ʶ��
	CAN_CHASS_COM.tx_header.RTR=CAN_RTR_DATA;		  // ��Ϣ����Ϊ����֡��һ֡8λ
	CAN_CHASS_COM.tx_header.DLC=8;							 
	
	CAN_CHASS_COM.tx_buf[0] = 0;
	CAN_CHASS_COM.tx_buf[1] = m0_disable;
	CAN_CHASS_COM.tx_buf[2] = 0;
	CAN_CHASS_COM.tx_buf[3] = m1_disable;
	CAN_CHASS_COM.tx_buf[4] = 0;
	CAN_CHASS_COM.tx_buf[5] = m2_disable;
	CAN_CHASS_COM.tx_buf[6] = 0;
	CAN_CHASS_COM.tx_buf[7] = m3_disable;
	
	return CAN_ChassSendMsg();	 		
}
/*********************************************
//�ͷ�����ѡ�еĵ��:�����ID=1~64��
//select_id�ĵ�iλ����0λ��ʼ��Ϊ1��ȱ�ʾID=i+1�ĵ����ѡ���ͷ�
//���ͬʱѡ��64�������select_id = 0xffffffffffffffff��
**********************************************/
uint8_t CAN_SendFreeAll(uint64_t select_id)
{
	uint32_t exid_send = 0;
	
	exid_send = ((uint32_t)_DEVICE_TYPE<<23) | ((uint32_t)_INFO_SOURCE_HOST<<22)| 
											 ((uint32_t)0x0b<<18)| ((uint32_t)0x00<<15)|
											 ((uint32_t)_CAN_GROUP_ALL<<8) | 0x00;
	
	CAN_CHASS_COM.tx_header.StdId=exid_send>>18;	 // ��׼��ʶ��
	CAN_CHASS_COM.tx_header.ExtId=exid_send;	 // ������չ��ʾ����29λ��
	CAN_CHASS_COM.tx_header.IDE=CAN_ID_EXT;		  // ʹ����չ��ʶ��
	CAN_CHASS_COM.tx_header.RTR=CAN_RTR_DATA;		  // ��Ϣ����Ϊ����֡��һ֡8λ
	CAN_CHASS_COM.tx_header.DLC=8;							 

	CAN_CHASS_COM.tx_buf[0] = select_id >> 56;
	CAN_CHASS_COM.tx_buf[1] = select_id >> 48;
	CAN_CHASS_COM.tx_buf[2] = select_id >> 40;
	CAN_CHASS_COM.tx_buf[3] = select_id >> 32;
	CAN_CHASS_COM.tx_buf[4] = select_id >> 24;
	CAN_CHASS_COM.tx_buf[5] = select_id >> 16;
	CAN_CHASS_COM.tx_buf[6] = select_id >> 8;
	CAN_CHASS_COM.tx_buf[7] = select_id >> 0;		
	
	return CAN_ChassSendMsg();
}
/*****************************************************************************************************************************************
																								CAN���Ƶ�����ܺ������������� PWM ģʽ
//��1�飺group=0��ID=1,2,3,4
//��2�飺group=1��ID=5,6,7,8
//��3�飺group=2��ID=9,10,11,12
//����
******************************************************************************************************************************************/
/*********************************************
//����ĳ��������PWM������-400~400��
**********************************************/
uint8_t CAN_SendPwmGroup(uint8_t group,int16_t pwm0,int16_t pwm1,int16_t pwm2,int16_t pwm3)
{	
	uint32_t exid_send = 0;
	
	exid_send = ((uint32_t)_DEVICE_TYPE<<23) | ((uint32_t)_INFO_SOURCE_HOST<<22)| 
											 ((uint32_t)0x01<<18)| ((uint32_t)0x01<<15)|
											 ((uint32_t)group<<8) | 0x00;
	
	CAN_CHASS_COM.tx_header.StdId=exid_send>>18;	 // ��׼��ʶ��
	CAN_CHASS_COM.tx_header.ExtId=exid_send;	 // ������չ��ʾ����29λ��
	CAN_CHASS_COM.tx_header.IDE=CAN_ID_EXT;		  // ʹ����չ��ʶ��
	CAN_CHASS_COM.tx_header.RTR=CAN_RTR_DATA;		  // ��Ϣ����Ϊ����֡��һ֡8λ
	CAN_CHASS_COM.tx_header.DLC=8;							 
	
	CAN_CHASS_COM.tx_buf[0] = pwm0>>8;
	CAN_CHASS_COM.tx_buf[1] = pwm0 & 0xff;
	CAN_CHASS_COM.tx_buf[2] = pwm1>>8;
	CAN_CHASS_COM.tx_buf[3] = pwm1 & 0xff;
	CAN_CHASS_COM.tx_buf[4] = pwm2>>8;
	CAN_CHASS_COM.tx_buf[5] = pwm2 & 0xff;
	CAN_CHASS_COM.tx_buf[6] = pwm3>>8;
	CAN_CHASS_COM.tx_buf[7] = pwm3 & 0xff;

	return CAN_ChassSendMsg();
}
/*********************************************
//����ĳЩ���PWM������-400~400��
//select_id�ĵ�iλ����0λ��ʼ��Ϊ1��ȱ�ʾID=i+1�ĵ����ѡ��
//���ͬʱѡ��64�������select_id = 0xffffffffffffffff��
**********************************************/
uint8_t CAN_SendPwmAll(uint64_t select_id, int16_t pwm_data)
{
	uint32_t exid_send = 0;
	
	exid_send = ((uint32_t)_DEVICE_TYPE<<23) | ((uint32_t)_INFO_SOURCE_HOST<<22)| 
											 ((uint32_t)0x01<<18)| ((uint32_t)0x00<<15)|
											 ((uint32_t)_CAN_GROUP_ALL<<8) | 0x00;
	
	CAN_CHASS_COM.tx_header.StdId=exid_send>>18;	 // ��׼��ʶ��
	CAN_CHASS_COM.tx_header.ExtId=exid_send;	 // ������չ��ʾ����29λ��
	CAN_CHASS_COM.tx_header.IDE=CAN_ID_EXT;		  // ʹ����չ��ʶ��
	CAN_CHASS_COM.tx_header.RTR=CAN_RTR_DATA;		  // ��Ϣ����Ϊ����֡��һ֡8λ
	CAN_CHASS_COM.tx_header.DLC=8;							 
	
	CAN_CHASS_COM.tx_buf[0] = select_id >> 56;
	CAN_CHASS_COM.tx_buf[1] = select_id >> 48;
	CAN_CHASS_COM.tx_buf[2] = select_id >> 40;
	CAN_CHASS_COM.tx_buf[3] = select_id >> 32;
	CAN_CHASS_COM.tx_buf[4] = select_id >> 24;
	CAN_CHASS_COM.tx_buf[5] = select_id >> 16;
	CAN_CHASS_COM.tx_buf[6] = select_id >> 8;
	CAN_CHASS_COM.tx_buf[7] = select_id >> 0;	
	
	CAN_ChassSendMsg();
		
	exid_send = ((uint32_t)_DEVICE_TYPE<<23) | ((uint32_t)_INFO_SOURCE_HOST<<22)| 
											 ((uint32_t)0x01<<18)| ((uint32_t)0x01<<15)|
											 ((uint32_t)_CAN_GROUP_ALL<<8) | 0x00;
	CAN_CHASS_COM.tx_header.StdId=exid_send>>18;	 // ��׼��ʶ��
	CAN_CHASS_COM.tx_header.ExtId=exid_send;	 // ������չ��ʾ����29λ��
	CAN_CHASS_COM.tx_header.IDE=CAN_ID_EXT;		  // ʹ����չ��ʶ��
	CAN_CHASS_COM.tx_header.RTR=CAN_RTR_DATA;		  // ��Ϣ����Ϊ����֡��һ֡8λ
	CAN_CHASS_COM.tx_header.DLC=8;				
	
	CAN_CHASS_COM.tx_buf[0] = pwm_data>>8;
	CAN_CHASS_COM.tx_buf[1] = pwm_data & 0xff;			
		
	return CAN_ChassSendMsg();
}
/*****************************************************************************************************************************************
																								CAN���Ƶ�����ܺ������������� ���� ģʽ
//��1�飺group=0��ID=1,2,3,4
//��2�飺group=1��ID=5,6,7,8
//��3�飺group=2��ID=9,10,11,12
//����
******************************************************************************************************************************************/
/*********************************************
//����ĳ����������������
**********************************************/
uint8_t CAN_SendCurGroup(uint8_t group,int16_t cur0,int16_t cur1,int16_t cur2,int16_t cur3)
{
	uint32_t exid_send = 0;
	
	exid_send = ((uint32_t)_DEVICE_TYPE<<23) | ((uint32_t)_INFO_SOURCE_HOST<<22)| 
											 ((uint32_t)0x02<<18)| ((uint32_t)0x01<<15)|
											 ((uint32_t)group<<8) | 0x00;
	
	CAN_CHASS_COM.tx_header.StdId=exid_send>>18;	 // ��׼��ʶ��
	CAN_CHASS_COM.tx_header.ExtId=exid_send;	 // ������չ��ʾ����29λ��
	CAN_CHASS_COM.tx_header.IDE=CAN_ID_EXT;		  // ʹ����չ��ʶ��
	CAN_CHASS_COM.tx_header.RTR=CAN_RTR_DATA;		  // ��Ϣ����Ϊ����֡��һ֡8λ
	CAN_CHASS_COM.tx_header.DLC=8;							 
	
	CAN_CHASS_COM.tx_buf[0] = cur0>>8;
	CAN_CHASS_COM.tx_buf[1] = cur0 & 0xff;
	CAN_CHASS_COM.tx_buf[2] = cur1>>8;
	CAN_CHASS_COM.tx_buf[3] = cur1 & 0xff;
	CAN_CHASS_COM.tx_buf[4] = cur2>>8;
	CAN_CHASS_COM.tx_buf[5] = cur2 & 0xff;
	CAN_CHASS_COM.tx_buf[6] = cur3>>8;
	CAN_CHASS_COM.tx_buf[7] = cur3 & 0xff;
	
	return CAN_ChassSendMsg();	
}
/**
��C620���--3508���
group_id:0x200--���Ƶ��1-4
group_id:0x1FF--���Ƶ��5-8
cur0-4��-16384~16384
**/
uint8_t CAN_SendCurGroup3508(uint16_t group_id,int16_t cur0,int16_t cur1,int16_t cur2,int16_t cur3)
{
	
	CAN_CHASS_COM.tx_header.StdId=group_id;	 // ��׼��ʶ��
	CAN_CHASS_COM.tx_header.ExtId=group_id;	 // ������չ��ʾ����29λ��
	CAN_CHASS_COM.tx_header.IDE=CAN_ID_STD;		  // ʹ�ñ�׼��ʶ��
	CAN_CHASS_COM.tx_header.RTR=CAN_RTR_DATA;		  // ��Ϣ����Ϊ����֡��һ֡8λ
	CAN_CHASS_COM.tx_header.DLC=8;							 
	
	CAN_CHASS_COM.tx_buf[0] = cur0>>8;
	CAN_CHASS_COM.tx_buf[1] = cur0 & 0xff;
	CAN_CHASS_COM.tx_buf[2] = cur1>>8;
	CAN_CHASS_COM.tx_buf[3] = cur1 & 0xff;
	CAN_CHASS_COM.tx_buf[4] = cur2>>8;
	CAN_CHASS_COM.tx_buf[5] = cur2 & 0xff;
	CAN_CHASS_COM.tx_buf[6] = cur3>>8;
	CAN_CHASS_COM.tx_buf[7] = cur3 & 0xff;
	
	return CAN_ChassSendMsg();	
}
/*********************************************
//����ĳЩ�������������-3000~3000��mA��
//select_id�ĵ�iλ����0λ��ʼ��Ϊ1��ȱ�ʾID=i+1�ĵ����ѡ��
//���ͬʱѡ��64�������select_id = 0xffffffffffffffff��
**********************************************/
uint8_t CAN_SendCurAll(uint64_t select_id, int16_t cur_data)
{
	uint32_t exid_send = 0;
	
	exid_send = ((uint32_t)_DEVICE_TYPE<<23) | ((uint32_t)_INFO_SOURCE_HOST<<22)| 
											 ((uint32_t)0x02<<18)| ((uint32_t)0x00<<15)|
											 ((uint32_t)_CAN_GROUP_ALL<<8) | 0x00;
	
	CAN_CHASS_COM.tx_header.StdId=exid_send>>18;	 // ��׼��ʶ��
	CAN_CHASS_COM.tx_header.ExtId=exid_send;	 // ������չ��ʾ����29λ��
	CAN_CHASS_COM.tx_header.IDE=CAN_ID_EXT;		  // ʹ����չ��ʶ��
	CAN_CHASS_COM.tx_header.RTR=CAN_RTR_DATA;		  // ��Ϣ����Ϊ����֡��һ֡8λ
	CAN_CHASS_COM.tx_header.DLC=8;							 
	
	CAN_CHASS_COM.tx_buf[0] = select_id >> 56;
	CAN_CHASS_COM.tx_buf[1] = select_id >> 48;
	CAN_CHASS_COM.tx_buf[2] = select_id >> 40;
	CAN_CHASS_COM.tx_buf[3] = select_id >> 32;
	CAN_CHASS_COM.tx_buf[4] = select_id >> 24;
	CAN_CHASS_COM.tx_buf[5] = select_id >> 16;
	CAN_CHASS_COM.tx_buf[6] = select_id >> 8;
	CAN_CHASS_COM.tx_buf[7] = select_id >> 0;	
	
	CAN_ChassSendMsg();
			
	exid_send = ((uint32_t)_DEVICE_TYPE<<23) | ((uint32_t)_INFO_SOURCE_HOST<<22)| 
											 ((uint32_t)0x02<<18)| ((uint32_t)0x01<<15)|
											 ((uint32_t)_CAN_GROUP_ALL<<8) | 0x00;
	CAN_CHASS_COM.tx_header.StdId=exid_send>>18;	 // ��׼��ʶ��
	CAN_CHASS_COM.tx_header.ExtId=exid_send;	 // ������չ��ʾ����29λ��
	CAN_CHASS_COM.tx_header.IDE=CAN_ID_EXT;		  // ʹ����չ��ʶ��
	CAN_CHASS_COM.tx_header.RTR=CAN_RTR_DATA;		  // ��Ϣ����Ϊ����֡��һ֡8λ
	CAN_CHASS_COM.tx_header.DLC=8;				
	
	CAN_CHASS_COM.tx_buf[0] = cur_data >> 8;
	CAN_CHASS_COM.tx_buf[1] = cur_data & 0xff;	
	
	return CAN_ChassSendMsg();
}
/*****************************************************************************************************************************************
																								CAN���Ƶ�����ܺ������������� �ٶ� ģʽ
//��1�飺group=0��ID=1,2,3,4
//��2�飺group=1��ID=5,6,7,8
//��3�飺group=2��ID=9,10,11,12
//����
******************************************************************************************************************************************/
/*********************************************
//����ĳ���������ٶȲ�����-10000~10000��rpm��
**********************************************/
uint8_t CAN_SendSpeGroup(uint8_t group,int16_t spe0,int16_t spe1,int16_t spe2,int16_t spe3)
{
	uint32_t exid_send = 0;
	
	exid_send = ((uint32_t)_DEVICE_TYPE<<23) | ((uint32_t)_INFO_SOURCE_HOST<<22)| 
											 ((uint32_t)0x03<<18)| ((uint32_t)0x01<<15)|
											 ((uint32_t)group<<8) | 0x00;
	
	CAN_CHASS_COM.tx_header.StdId=exid_send>>18;	 // ��׼��ʶ��
	CAN_CHASS_COM.tx_header.ExtId=exid_send;	 // ������չ��ʾ����29λ��
	CAN_CHASS_COM.tx_header.IDE=CAN_ID_EXT;		  // ʹ����չ��ʶ��
	CAN_CHASS_COM.tx_header.RTR=CAN_RTR_DATA;		  // ��Ϣ����Ϊ����֡��һ֡8λ
	CAN_CHASS_COM.tx_header.DLC=8;							 
	
	CAN_CHASS_COM.tx_buf[0] = spe0>>8;
	CAN_CHASS_COM.tx_buf[1] = spe0 & 0xff;
	CAN_CHASS_COM.tx_buf[2] = spe1>>8;
	CAN_CHASS_COM.tx_buf[3] = spe1 & 0xff;
	CAN_CHASS_COM.tx_buf[4] = spe2>>8;
	CAN_CHASS_COM.tx_buf[5] = spe2 & 0xff;
	CAN_CHASS_COM.tx_buf[6] = spe3>>8;
	CAN_CHASS_COM.tx_buf[7] = spe3 & 0xff;
	
	return CAN_ChassSendMsg();	
}
/*********************************************
//����ĳЩ�������������-10000~10000��rpm��
//select_id�ĵ�iλ����0λ��ʼ��Ϊ1��ȱ�ʾID=i+1�ĵ����ѡ��
//���ͬʱѡ��64�������select_id = 0xffffffffffffffff��
**********************************************/
uint8_t CAN_SendSpeAll(uint64_t select_id, int16_t spe_data)
{
	uint32_t exid_send = 0;
	
	exid_send = ((uint32_t)_DEVICE_TYPE<<23) | ((uint32_t)_INFO_SOURCE_HOST<<22)| 
											 ((uint32_t)0x03<<18)| ((uint32_t)0x00<<15)|
											 ((uint32_t)_CAN_GROUP_ALL<<8) | 0x00;
	
	CAN_CHASS_COM.tx_header.StdId=exid_send>>18;	 // ��׼��ʶ��
	CAN_CHASS_COM.tx_header.ExtId=exid_send;	 // ������չ��ʾ����29λ��
	CAN_CHASS_COM.tx_header.IDE=CAN_ID_EXT;		  // ʹ����չ��ʶ��
	CAN_CHASS_COM.tx_header.RTR=CAN_RTR_DATA;		  // ��Ϣ����Ϊ����֡��һ֡8λ
	CAN_CHASS_COM.tx_header.DLC=8;							 
	
	CAN_CHASS_COM.tx_buf[0] = select_id >> 56;
	CAN_CHASS_COM.tx_buf[1] = select_id >> 48;
	CAN_CHASS_COM.tx_buf[2] = select_id >> 40;
	CAN_CHASS_COM.tx_buf[3] = select_id >> 32;
	CAN_CHASS_COM.tx_buf[4] = select_id >> 24;
	CAN_CHASS_COM.tx_buf[5] = select_id >> 16;
	CAN_CHASS_COM.tx_buf[6] = select_id >> 8;
	CAN_CHASS_COM.tx_buf[7] = select_id >> 0;	
	
	CAN_ChassSendMsg();
			
	exid_send = ((uint32_t)_DEVICE_TYPE<<23) | ((uint32_t)_INFO_SOURCE_HOST<<22)| 
											 ((uint32_t)0x03<<18)| ((uint32_t)0x01<<15)|
											 ((uint32_t)_CAN_GROUP_ALL<<8) | 0x00;
	CAN_CHASS_COM.tx_header.StdId=exid_send>>18;	 // ��׼��ʶ��
	CAN_CHASS_COM.tx_header.ExtId=exid_send;	 // ������չ��ʾ����29λ��
	CAN_CHASS_COM.tx_header.IDE=CAN_ID_EXT;		  // ʹ����չ��ʶ��
	CAN_CHASS_COM.tx_header.RTR=CAN_RTR_DATA;		  // ��Ϣ����Ϊ����֡��һ֡8λ
	CAN_CHASS_COM.tx_header.DLC=8;				
	
	CAN_CHASS_COM.tx_buf[0] = spe_data >> 8;
	CAN_CHASS_COM.tx_buf[1] = spe_data & 0xff;										 
	return CAN_ChassSendMsg();
}
/*****************************************************************************************************************************************
																								CAN���Ƶ�����ܺ������������� λ�� ģʽ

******************************************************************************************************************************************/
/*********************************************
//����ĳ�����λ�ò�����-x~x��int32��ֵ��Χ����3600ppr��
**********************************************/
uint8_t CAN_SendPosSingle(uint8_t id,int32_t pos)
{
	uint32_t exid_send = 0;
	uint8_t group = (id-1)/4;
	exid_send = ((uint32_t)_DEVICE_TYPE<<23) | ((uint32_t)_INFO_SOURCE_HOST<<22)| 
											 ((uint32_t)0x04<<18)| ((uint32_t)0x01<<15)|
											 ((uint32_t)group<<8) | id;
	
	CAN_CHASS_COM.tx_header.StdId=exid_send>>18;	 // ��׼��ʶ��
	CAN_CHASS_COM.tx_header.ExtId=exid_send;	 // ������չ��ʾ����29λ��
	CAN_CHASS_COM.tx_header.IDE=CAN_ID_EXT;		  // ʹ����չ��ʶ��
	CAN_CHASS_COM.tx_header.RTR=CAN_RTR_DATA;		  // ��Ϣ����Ϊ����֡��һ֡8λ
	CAN_CHASS_COM.tx_header.DLC=8;							 
	
	CAN_CHASS_COM.tx_buf[0] = pos>>24;
	CAN_CHASS_COM.tx_buf[1] = pos>>16;
	CAN_CHASS_COM.tx_buf[2] = pos>>8;
	CAN_CHASS_COM.tx_buf[3] = pos&0xff;
	CAN_CHASS_COM.tx_buf[4] = 0;
	CAN_CHASS_COM.tx_buf[5] = 0;
	CAN_CHASS_COM.tx_buf[6] = 0;
	CAN_CHASS_COM.tx_buf[7] = 0;
	
	return CAN_ChassSendMsg();
}
/*********************************************
//����ĳЩ���λ�ò�����-x~x��int32��ֵ��Χ����3600ppr��
//select_id�ĵ�iλ����0λ��ʼ��Ϊ1��ȱ�ʾID=i+1�ĵ����ѡ��
//���ͬʱѡ��64�������select_id = 0xffffffffffffffff��
**********************************************/
uint8_t CAN_SendPosAll(uint64_t select_id,int32_t pos)
{
	uint32_t exid_send = 0;
	

	exid_send = ((uint32_t)_DEVICE_TYPE<<23) | ((uint32_t)_INFO_SOURCE_HOST<<22)| 
											 ((uint32_t)0x04<<18)| ((uint32_t)0x00<<15)|
											 ((uint32_t)_CAN_GROUP_ALL<<8) | 0x00;
	
	CAN_CHASS_COM.tx_header.StdId=exid_send>>18;	 // ��׼��ʶ��
	CAN_CHASS_COM.tx_header.ExtId=exid_send;	 // ������չ��ʾ����29λ��
	CAN_CHASS_COM.tx_header.IDE=CAN_ID_EXT;		  // ʹ����չ��ʶ��
	CAN_CHASS_COM.tx_header.RTR=CAN_RTR_DATA;		  // ��Ϣ����Ϊ����֡��һ֡8λ
	CAN_CHASS_COM.tx_header.DLC=8;							 
	
	CAN_CHASS_COM.tx_buf[0] = select_id >> 56;
	CAN_CHASS_COM.tx_buf[1] = select_id >> 48;
	CAN_CHASS_COM.tx_buf[2] = select_id >> 40;
	CAN_CHASS_COM.tx_buf[3] = select_id >> 32;
	CAN_CHASS_COM.tx_buf[4] = select_id >> 24;
	CAN_CHASS_COM.tx_buf[5] = select_id >> 16;
	CAN_CHASS_COM.tx_buf[6] = select_id >> 8;
	CAN_CHASS_COM.tx_buf[7] = select_id >> 0;	
	
	CAN_ChassSendMsg();
		
	exid_send = ((uint32_t)_DEVICE_TYPE<<23) | ((uint32_t)_INFO_SOURCE_HOST<<22)| 
											 ((uint32_t)0x04<<18)| ((uint32_t)0x01<<15)|
											 ((uint32_t)_CAN_GROUP_ALL<<8) | 0x00;
	CAN_CHASS_COM.tx_header.StdId=exid_send>>18;	 // ��׼��ʶ��
	CAN_CHASS_COM.tx_header.ExtId=exid_send;	 // ������չ��ʾ����29λ��
	CAN_CHASS_COM.tx_header.IDE=CAN_ID_EXT;		  // ʹ����չ��ʶ��
	CAN_CHASS_COM.tx_header.RTR=CAN_RTR_DATA;		  // ��Ϣ����Ϊ����֡��һ֡8λ
	CAN_CHASS_COM.tx_header.DLC=8;				
	
	CAN_CHASS_COM.tx_buf[0] = pos>>24;
	CAN_CHASS_COM.tx_buf[1] = pos>>16;
	CAN_CHASS_COM.tx_buf[2] = pos>>8;
	CAN_CHASS_COM.tx_buf[3] = pos&0xff;
	CAN_CHASS_COM.tx_buf[4] = 0;
	CAN_CHASS_COM.tx_buf[5] = 0;
	CAN_CHASS_COM.tx_buf[6] = 0;
	CAN_CHASS_COM.tx_buf[7] = 0;										 

	return CAN_ChassSendMsg();
}
/*****************************************************************************************************************************************
																								CAN���Ƶ�����ܺ��������������е��ID
ע�⣺�ú���Ӧֻ����һ�Σ���ֹ�������øú���
���øú��������е������ID����״̬������Ϻ�����̵ƾ�������-->��ʱ����ת������������Զ�����ID��ţ���ID=1����ת��˳�������
******************************************************************************************************************************************/
uint8_t CAN_SendResetIdAll(void)
{
	uint32_t exid_send = 0;
	
	exid_send = ((uint32_t)_DEVICE_TYPE<<23) | ((uint32_t)_INFO_SOURCE_HOST<<22)| 
											 ((uint32_t)0<<18)| ((uint32_t)0<<15)|
											 ((uint32_t)_CAN_GROUP_ALL<<8) | 0x00;
	
	CAN_CHASS_COM.tx_header.StdId=exid_send>>18;	 // ��׼��ʶ��
	CAN_CHASS_COM.tx_header.ExtId=exid_send;	 // ������չ��ʾ����29λ��
	CAN_CHASS_COM.tx_header.IDE=CAN_ID_EXT;		  // ʹ����չ��ʶ��
	CAN_CHASS_COM.tx_header.RTR=CAN_RTR_DATA;		  // ��Ϣ����Ϊ����֡��һ֡8λ
	CAN_CHASS_COM.tx_header.DLC=8;							 
	
	CAN_CHASS_COM.tx_buf[0] = 0;
	CAN_CHASS_COM.tx_buf[1] = 0;
	CAN_CHASS_COM.tx_buf[2] = 0;
	CAN_CHASS_COM.tx_buf[3] = 0;
	CAN_CHASS_COM.tx_buf[4] = 0;
	CAN_CHASS_COM.tx_buf[5] = 0;
	CAN_CHASS_COM.tx_buf[6] = 0;
	CAN_CHASS_COM.tx_buf[7] = 0;

	return CAN_ChassSendMsg();		
}

