#ifndef _user_movebase_h
#define _user_movebase_h

#include "main.h"

typedef enum
{
	CTR_NONE,
	CTR_HANDLE,
	CTR_ROTATE,
	CTR_SLIDE,
	CTR_TRACK,
	CTR_SLAM,
	CTR_REMOTE
}USER_CTR_TYPE;

typedef enum
{
	DIR_X,
	DIR_Y
}USER_DIR_TYPE;


typedef struct
{		
	float kp;
	float ki;
	float kd;
	
	float p_max;
	float i_max;
	float d_max;
	float pid_max;
	float errMax;

	float p_out;
	float i_sum;
	float i_out;
	float d_out;    
	float pid_out;    

	float target;
	float err_now;
	float err_last;  
}PID_TYPEDEF;

typedef struct
{		
//  float pos_x;
//	float pos_y;
//	float ang_rad;
//	float v_linear;
//	float v_angular;

	float pos_x;       // X位置
	float pos_y;       // Y位置
	float ang_rad;     // 角度
	float v_linear_x;  // 线速度X 
	float v_linear_y;  // 新增Y方向线速度
	float v_angular;   // 角速度
	
	float fix_x;
	float fix_y;
	float fix_ang;
	uint8_t fix_en;
}ODOMETER_TYPEDEF;

typedef struct
{
	uint8_t motor_en;
	int16_t spe_x;
	int16_t spe_y;
	int16_t spe_ang;
	
	int16_t pos[8];
	int16_t speed[8];
	int16_t cur[8];
	
//	PID_TYPEDEF speed_pid;
	PID_TYPEDEF angz_pid;
	PID_TYPEDEF womiga_pid;
	
	USER_CTR_TYPE ctr_type;
	ODOMETER_TYPEDEF odometer;
	
}MOVE_TYPEDEF;
extern MOVE_TYPEDEF MOVE;


void User_MoveBaseInit(void);
void User_MoveSpeedCal(void); 
float User_MoveAngPID(void);


void User_MoveOdomCal(void);
void User_MovePosCorrect(void);
void User_MoveOdomSet(float x,float y ,float ang);
void User_MoveOdomReset(void);

uint8_t User_MoveStop(void);
uint8_t User_TurnToAngle(float ang);
uint8_t User_MoveToDist(uint8_t dir,float dist);
float User_MoveVerticalPID(float target);//竖直方向调整
float User_MoveHorizontalPID(float target);//水平方向调整
float User_MoveDouble_anglePID(void);//双路角度pid
uint8_t User_ToDist(uint8_t dir,float dist);//传感器主导前进
uint8_t User_AutoToDist_L(float dist);//自动左路控制
uint8_t User_AutoToDist_R(float dist);//自动右路控制
uint8_t User_AutoToDist_F(float dist);//自动前进控制
extern int16_t ver_diff;//竖直方向调整速度
extern int16_t hor_diff;//水平方向调整速度
extern int16_t angle_diff;//角度调整速度
uint8_t User_AutoToDist_C(float dist);//自动柱子测量
uint8_t User_AngleCheck(void);//角度检查
#endif




