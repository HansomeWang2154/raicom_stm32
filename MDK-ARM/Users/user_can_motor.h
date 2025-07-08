#ifndef _USER_CAN_MOTOR_H
#define _USER_CAN_MOTOR_H

#include "main.h"    

#define _CHASS_MOTOR_NUM 4
#define _FLASH_PARAM_NUM 50


#define _CAN_TX_CMD_ID		0
#define _CAN_TX_CMD_ACT		1
#define _CAN_TX_CMD_PARAM	2
#define _CAN_TX_CMD_IDLE	3
#define _CAN_GROUP_ALL 0x7f


#define	_DEVICE_TYPE	0x10
#define _INFO_SOURCE_HOST 0x00
#define _DEVICE_GROUP_ALL	0x7f

#define _CMD_TYPE_ID_FUNC				0x00
#define _CMD_TYPE_ACT_PWM				0x01
#define _CMD_TYPE_ACT_CURRENT		0x02
#define _CMD_TYPE_ACT_SPEED			0x03
#define _CMD_TYPE_ACT_POS				0x04
#define _CMD_TYPE_PARAM					0x0a
#define _CMD_TYPE_REPORT				0x0f


typedef struct 
{			
	CAN_RxHeaderTypeDef  rx_header;
	CAN_TxHeaderTypeDef  tx_header;

	uint8_t rev_id_number;
	
	uint8_t rx_buf[8];
	uint8_t tx_buf[8];

	uint8_t flag_tx_ok;
	uint8_t flag_tx_req;

}USR_CAN_COM;
extern USR_CAN_COM CAN1_COM;
extern USR_CAN_COM CAN2_COM;

/** 定义3508电机参数**/
typedef struct
{
	int16_t		cur_now;	
	int16_t		spe_now;
	int16_t   spe_last;
	int16_t		pos_now;
	int16_t		pos_last;
	int16_t		pos_add;
	float			pos_total;
	uint8_t		temp_now;
}MOTOR_3508_TYPE;
extern MOTOR_3508_TYPE MOTOR_3508[8];

typedef struct 
{
	uint32_t	soft_version;
	uint32_t	device_type;
	
	int32_t		enc_ppr;
	int32_t		enc_agc;
	
	float			raduction;
	
	uint32_t	m_id;
	uint32_t	m_protec;//bit0-volt_protec; bit1-temp_protec; bit2-current_protec; bit3-stall_protec;
	
	int32_t		pwm_max;
	int32_t		pwm_pre;
	int32_t		pwm_now;
		
	int32_t		cur_max;
	int32_t		cur_pre;
	int32_t		cur_tar;
	uint32_t	cur_kp;
	uint32_t	cur_ki;
	uint32_t	cur_kd;
	int32_t		cur_now;	

	int32_t		spe_max;
	int32_t		spe_pre;
	int32_t		spe_tar;
	uint32_t	spe_kp;
	uint32_t	spe_ki;
	uint32_t	spe_kd;
	int32_t		spe_now;
	
	int32_t		acc_max;
	int32_t		acc_now;	
	
	int32_t		pos_max;
	int32_t		pos_min;
	int32_t 	pos_pre;
	int32_t 	pos_tar;
	uint32_t	pos_kp;
	uint32_t	pos_ki;
	uint32_t	pos_kd;	
	int32_t		pos_now;
	
	uint32_t	volt_max;
	uint32_t	volt_min;
	uint32_t	volt_now;
	
	int32_t		temp_max;
	int32_t		temp_r_now;
	int32_t		temp_c_now;	
	
	uint32_t	down_load;
}USR_MOTOR_PARAM;

typedef union
{
	USR_MOTOR_PARAM item;
	uint32_t 				list[_FLASH_PARAM_NUM];
}UNION_MOTOR_PARAM;
//m_err：电机错误信息：
//#define _ERR_NONE 				0
//#define _ERR_FLASH_BIT 		1
//#define _ERR_CUR_BIT 			2
//#define _ERR_ENC_BIT 			3 
//#define _ERR_TEMP_BIT 		4
//#define _ERR_VOLT_BIT 		5
//#define _ERR_CAN_BIT 			6
//#define _ERR_STALL_BIT 		7
typedef struct
{
	UNION_MOTOR_PARAM flash;
	uint8_t		m_state;
	uint8_t		m_err;
	uint8_t		m_war;
	uint8_t 	m_id;
}CHASS_MOTOR_TYPE;

extern CHASS_MOTOR_TYPE CHASS_MOTOR[_CHASS_MOTOR_NUM];



void User_CanMotorInit(void);
void User_Can1FilterInit(void);
void User_Can2FilterInit(void);
void User_Can1RxIRQHandler(void);
void User_Can2RxIRQHandler(void);

void User_Can1RxMessageHandler(void);
void User_Can2RxMessageHandler(void);


uint8_t CAN_SendFreeGroup(uint8_t group,uint8_t m0_disable,uint8_t m1_disable,uint8_t m2_disable,uint8_t m3_disable);
uint8_t CAN_SendFreeAll(uint64_t select_id);

uint8_t CAN_SendPwmGroup(uint8_t group,int16_t pwm0,int16_t pwm1,int16_t pwm2,int16_t pwm3);
uint8_t CAN_SendPwmAll(uint64_t select_id, int16_t pwm_data);

uint8_t CAN_SendCurGroup(uint8_t group,int16_t cur0,int16_t cur1,int16_t cur2,int16_t cur3);
uint8_t CAN_SendCurAll(uint64_t select_id, int16_t cur_data);

uint8_t CAN_SendSpeGroup(uint8_t group,int16_t spe0,int16_t spe1,int16_t spe2,int16_t spe3);
uint8_t CAN_SendSpeAll(uint64_t select_id, int16_t spe_data);

uint8_t CAN_SendPosSingle(uint8_t id,int32_t pos);
uint8_t CAN_SendPosAll(uint64_t select_id,int32_t pos);

uint8_t CAN_SendResetIdAll(void); 
uint8_t CAN_SendCurGroup3508(uint16_t group_id,int16_t cur0,int16_t cur1,int16_t cur2,int16_t cur3);
#endif

















