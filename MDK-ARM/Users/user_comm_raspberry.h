#ifndef _USER_COMM_RASPBERRY_H
#define _USER_COMM_RASPBERRY_H


#include "main.h"


#define USART_SMP_TX_SIZE	120
#define USART_SMP_RX_SIZE	120

typedef struct
{
	float cmd_vx;
	float cmd_vy;
	float cmd_womiga;
	float cmd_m1;
	float	cmd_m2;
	float cmd_m3;
	float cmd_m4;
	float fix_px;
	float fix_py;
	float fix_ang;
	uint32_t rgb_buz;
	float	servo1;
	float	servo2;
	float servo3;
	float servo4;
	float rev_1;
	float rev_2;
	float rev_3;
	float rev_4;
	float rev_5;
	float rev_6;
	float rev_7;
	float rev_8;
	uint32_t laser_shoot_ms;
	uint32_t state;
}ROS_STM_TYPEDEF;

typedef struct
{
	float odom_px;
	float odom_py;
	float odom_ang;
	float odom_vx;
	float	odom_vy;
	float odom_womoga;
	float uwb_px;
	float uwb_py;
	float goal_px;
	float goal_py;
	float goal_ang;
	float	act_servo1;
	float	act_servo2;
	float act_servo3;
	float act_servo4;
	float spe_m1;
	float spe_m2;
	float spe_m3;
	float spe_m4;
	float pos_m1;
	float pos_m2;
	float pos_m3;
	float pos_m4;
	uint32_t laser_rev_ms;
	uint32_t state;
}STM_ROS_TYPEDEF;

typedef struct
{
	uint8_t usart_tx_buf[USART_SMP_TX_SIZE];
	uint8_t usart_rx_buf[USART_SMP_RX_SIZE];				
	uint16_t rx_data_len;
	uint8_t init_ok;
	uint8_t rev_cnt;
	
	ROS_STM_TYPEDEF ros_stm;
	STM_ROS_TYPEDEF stm_ros;
}USART_SMP_TYPEDEF;

extern USART_SMP_TYPEDEF SMP;


void User_CommSmpInit(void);
void User_CommSmpIRQHandler(void);
void User_CommRosToStmDataParas(void);

void User_CommStmToRosSend(void);
void User_CommSmpSendTask(void);


#endif










