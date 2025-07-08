/*********************************************
//运动控制相关函数
**********************************************/
#include "user_movebase.h"
#include "user_tasks.h"
#include "cmsis_os.h"
#include "user_imu.h"
#include "user_comm_wifi.h" 
#include "user_can_motor.h"  
#include "user_comm_raspberry.h" 
#include "user_state.h" 
#include "user_math.h"
#include "user_timer.h"
#include "user_tofsense_f.h"
#include "remote_control.h"
#include "user_tofsenser2.h"

//遥控灵敏度
#define _HANDLE_SPEED_SCALE 20
MOVE_TYPEDEF MOVE;
POINT_TYPEDEF AUTO_POINT_L[8]={
	30,6950,			//0
  600,6950,	//1
	1500,6950, //2
	2100.5,6950,//3
	2700,6950,//4
	3200,6950,//5
	3800,6950,//6
	4400,6950,//7
};
POINT_TYPEDEF AUTO_POINT_R[4]={
	120,6950,			//0
  682.5,6950,	//1
	1245,6950, //2
	1807.5,6950,//3
};
PID_TYPEDEF VERTICAL,HORIZONTAL,DOUBLE_ANGLE;
int16_t ver_diff;//竖直方向调整速度
int16_t hor_diff;//水平方向调整速度
int16_t angle_diff;//角度调整速度
float dist_remain = 0;
float dist_remain2 = 0;
float dist_remain3=0;
float dist_remain4=0;
float dist_remain5=0;
float dist_remain6=0;
float yaokong_k=4;
int auto_flag=0;
int auto_target=0;
int auto_step=0;
float ang_dif=0;
/*********************************************
//初始化
**********************************************/
void User_MoveBaseInit(void)
{
	User_MoveOdomReset();
	
	MOVE.angz_pid.kp = 90;
	MOVE.angz_pid.ki = 0;
	MOVE.angz_pid.kd = 400;
	MOVE.womiga_pid.kp = 5;
	VERTICAL.kp=3;
	VERTICAL.kd=1;
	VERTICAL.ki=0.000001;
	HORIZONTAL.kp=10;
	HORIZONTAL.kd=5;
	HORIZONTAL.ki=0.001;
	DOUBLE_ANGLE.kp=1;
	DOUBLE_ANGLE.kd=0;
	DOUBLE_ANGLE.ki=0;
	for(int i=0;i<8;i++) MOVE.cur[i]=0;
}

/***************************************************************************************************************************************
																											行驶过程闭环控制
***************************************************************************************************************************************/
//角速度——闭环控制
float User_MoveWomigaPID(void) 
{	
	MOVE.womiga_pid.err_now = MOVE.womiga_pid.target - IMU1.gyroz;	

	MOVE.womiga_pid.p_out = MOVE.womiga_pid.kp * MOVE.womiga_pid.err_now;

	MOVE.womiga_pid.i_out += MOVE.womiga_pid.ki * MOVE.womiga_pid.err_now;
	MOVE.womiga_pid.i_out = LimitMax(MOVE.womiga_pid.i_out,5000); 

	MOVE.womiga_pid.d_out = MOVE.womiga_pid.kd * (MOVE.womiga_pid.err_now - MOVE.womiga_pid.err_last);

	MOVE.womiga_pid.err_last = MOVE.womiga_pid.err_now;
	
	MOVE.womiga_pid.pid_out = MOVE.womiga_pid.p_out + MOVE.womiga_pid.i_out + MOVE.womiga_pid.d_out;
	
	return(-LimitMax(MOVE.womiga_pid.pid_out,7000)); 
}
//偏航角——角度——闭环控制
float User_MoveAngPID(void) 
{
	MOVE.angz_pid.err_now = MOVE.angz_pid.target - IMU1.angz;	
	if(MOVE.angz_pid.err_now>180)
		MOVE.angz_pid.err_now -= 360;
	else if(MOVE.angz_pid.err_now<-180)
		MOVE.angz_pid.err_now += 360;

	MOVE.angz_pid.p_out = MOVE.angz_pid.kp * MOVE.angz_pid.err_now;

	MOVE.angz_pid.i_out += MOVE.angz_pid.ki * MOVE.angz_pid.err_now;
	MOVE.angz_pid.i_out = LimitMax(MOVE.angz_pid.i_out,2000); 
	if(MOVE.angz_pid.err_last * MOVE.angz_pid.err_now < 0)
		MOVE.angz_pid.i_out = 0;

	MOVE.angz_pid.d_out = MOVE.angz_pid.kd * (MOVE.angz_pid.err_now - MOVE.angz_pid.err_last);

	MOVE.angz_pid.err_last = MOVE.angz_pid.err_now;
	
	MOVE.angz_pid.pid_out = MOVE.angz_pid.p_out + MOVE.angz_pid.i_out + MOVE.angz_pid.d_out;
	
	return(-LimitMax(MOVE.angz_pid.pid_out,7000)); 
}
//竖直pid控制
float User_MoveVerticalPID(float target)
{
	VERTICAL.err_now=(dis1+dis4)/2-target;
	VERTICAL.p_out = VERTICAL.kp * VERTICAL.err_now;
		
	VERTICAL.i_out += VERTICAL.ki * VERTICAL.err_now;
	VERTICAL.i_out = LimitMax(VERTICAL.i_out, 5000); 
		
	VERTICAL.d_out = VERTICAL.kd * (VERTICAL.err_now - VERTICAL.err_last);
		
	VERTICAL.pid_out = VERTICAL.p_out + VERTICAL.i_out + VERTICAL.d_out;
	VERTICAL.pid_out = LimitMax(VERTICAL.pid_out,1000);
		
	VERTICAL.err_last = VERTICAL.err_now;
	return (int16_t)VERTICAL.pid_out;
}
//水平pid控制
float User_MoveHorizontalPID(float target)
{
	HORIZONTAL.err_now=dis2-target;
	HORIZONTAL.p_out = HORIZONTAL.kp * HORIZONTAL.err_now;
		
	HORIZONTAL.i_out += HORIZONTAL.ki * HORIZONTAL.err_now;
	HORIZONTAL.i_out = LimitMax(HORIZONTAL.i_out, 5000); 
		
	HORIZONTAL.d_out = HORIZONTAL.kd * (HORIZONTAL.err_now - HORIZONTAL.err_last);
		
	HORIZONTAL.pid_out = HORIZONTAL.p_out + HORIZONTAL.i_out + HORIZONTAL.d_out;
	HORIZONTAL.pid_out = LimitMax(HORIZONTAL.pid_out,1000);
		
	HORIZONTAL.err_last = HORIZONTAL.err_now;
	return (int16_t)HORIZONTAL.pid_out;
}
//双路角度pid控制
float User_MoveDouble_anglePID(void)
{
	DOUBLE_ANGLE.err_now=(float)dis4-(float)dis1;
	DOUBLE_ANGLE.p_out = DOUBLE_ANGLE.kp * DOUBLE_ANGLE.err_now;
		
	DOUBLE_ANGLE.i_out += DOUBLE_ANGLE.ki * DOUBLE_ANGLE.err_now;
	DOUBLE_ANGLE.i_out = LimitMax(DOUBLE_ANGLE.i_out, 5000); 
		
	DOUBLE_ANGLE.d_out = DOUBLE_ANGLE.kd * (DOUBLE_ANGLE.err_now - DOUBLE_ANGLE.err_last);
		
	DOUBLE_ANGLE.pid_out = DOUBLE_ANGLE.p_out + DOUBLE_ANGLE.i_out + DOUBLE_ANGLE.d_out;
	DOUBLE_ANGLE.pid_out = LimitMax(DOUBLE_ANGLE.pid_out,1000);
		
	DOUBLE_ANGLE.err_last = DOUBLE_ANGLE.err_now;
	if(DOUBLE_ANGLE.err_now>1||DOUBLE_ANGLE.err_now<-1) return (int16_t)DOUBLE_ANGLE.pid_out;
	else return 0;
}

/***************************************************************************************************************************************
																											运动学解算
***************************************************************************************************************************************/
//麦轮底盘——运动学解算
void User_Move_4W_Mecanum(void)
{
	switch(MOVE.ctr_type)
	{
		
		case CTR_SLAM:
		{
			MOVE.spe_x = SMP.ros_stm.cmd_vx*100;
			MOVE.spe_y = SMP.ros_stm.cmd_vy*100;
			MOVE.spe_ang =SMP.ros_stm.cmd_womiga*100; 
		}break;

		case CTR_REMOTE:
		{
			if((SMP.ros_stm.cmd_vx < 0.01) && (SMP.ros_stm.cmd_vx > -0.01))
				SMP.ros_stm.cmd_vx  = 0;
			if((SMP.ros_stm.cmd_vy < 0.01) && (SMP.ros_stm.cmd_vy > -0.01))
				SMP.ros_stm.cmd_vy  = 0;
			if((SMP.ros_stm.cmd_womiga < 0.01) && (SMP.ros_stm.cmd_womiga > -0.01))
				SMP.ros_stm.cmd_womiga  = 0;
			
			MOVE.spe_x = SMP.ros_stm.cmd_vy*1000;
			MOVE.spe_y = SMP.ros_stm.cmd_vx*1000;
			MOVE.spe_ang = -SMP.ros_stm.cmd_womiga*1000;
			
			//MOVE.angz_pid.target -= SMP.ros_stm.cmd_womiga/100.0f;
			//MOVE.spe_ang = User_MoveAngPID();
		}break;
		default:
		{
			MOVE.spe_x = 0;
			MOVE.spe_y = 0;
			MOVE.spe_ang = 0;
		}break;
	}
	
	MOVE.speed[0] = MOVE.spe_x + MOVE.spe_y + MOVE.spe_ang;
	MOVE.speed[1] = MOVE.spe_x - MOVE.spe_y + MOVE.spe_ang;
	MOVE.speed[2] = -MOVE.spe_x + MOVE.spe_y + MOVE.spe_ang;
	MOVE.speed[3] = -MOVE.spe_x - MOVE.spe_y + MOVE.spe_ang;
}
/***************************************************************************************************************************************
																											电机速度解算
***************************************************************************************************************************************/
void User_MoveSpeedCal(void)
{
//	if((WIFI.handle.right_y != 0)||(WIFI.handle.right_x != 0)||(WIFI.handle.left_x != 0)||(WIFI.handle.left_y != 0))
//	{
//		MOVE.ctr_type = CTR_HANDLE;
//	}
//	if(main_control==2) MOVE.ctr_type=CTR_NONE;
//	else
//	{
//		if(ch[0]==0) MOVE.ctr_type=CTR_NONE;
//		else if(ch[0]>1000||ch[0]<950||ch[2]>200||ch[3]>1000||ch[3]<950)
//		{
//			if(ch[0]-993>15) 
//				{
//					MOVE.angz_pid.target -=User_AbsInt(ch[0]-993)*0.1*yaokong_k/450*0.8 ;
//					if(MOVE.angz_pid.target<-180) MOVE.angz_pid.target+=360;
//				}
//				else if(ch[0]-993<-15) 
//				{
//					MOVE.angz_pid.target +=User_AbsInt(ch[0]-993)*0.1*yaokong_k/450*0.8 ;
//					if(MOVE.angz_pid.target>180) MOVE.angz_pid.target-=360;
//				}
//			if(User_AbsFloat(ch[0]-993)<10&&User_AbsFloat(ch[2]-1020)<20&&User_AbsFloat(ch[3]-993)<20)
//				{
//					MOVE.ctr_type=CTR_NONE;
//					MOVE.spe_x = 0;
//					MOVE.spe_y = 0;
//					MOVE.spe_ang = 0;
//				}
//			else MOVE.ctr_type = CTR_REMOTE;
//		}
//		if(ch[7]<1200) yaokong_k=1;
//		else if(ch[7]>=1200) yaokong_k=10;
//		}
	
		
	User_Move_4W_Mecanum();
}
/***************************************************************************************************************************************
																										    里程计解算
***************************************************************************************************************************************/
//里程计校正参数，根据实测距离进行修正
#define _SPEED_RATIO 0.170905f
float temp_x=0;
float temp_y=0;
//机器人里程数据重置
void User_MoveOdomSet(float x,float y ,float ang)
{	
	MOVE.odometer.pos_x = x;
	MOVE.odometer.pos_y = y;
	IMU1.angz = ang;
}

//清除里程计
void User_MoveOdomReset(void)
{	
	User_MoveOdomSet(0,0,0);
}

//里程计算
void User_MoveOdomCal(void)
{
	MOVE.odometer.v_angular = IMU1.gyroz * 0.0174533f;//3.14/180--rad/s
	MOVE.odometer.ang_rad = IMU1.angz*0.0174533f;//3.14/180--rad
	
//	MOVE.odometer.ang_rad = 0.0;
//	float v_x = (MOTOR_3508[0].spe_now + MOTOR_3508[1].spe_now - MOTOR_3508[2].spe_now -MOTOR_3508[3].spe_now)*0.603217158*_SPEED_RATIO/1000;
//	float v_y = (MOTOR_3508[0].spe_now - MOTOR_3508[1].spe_now +MOTOR_3508[2].spe_now- MOTOR_3508[3].spe_now)*0.603217158*_SPEED_RATIO/1000;
	
 MOVE.odometer.v_linear_x = (MOTOR_3508[0].spe_now + MOTOR_3508[1].spe_now - MOTOR_3508[2].spe_now -MOTOR_3508[3].spe_now)*0.603217158*_SPEED_RATIO/1000;
 MOVE.odometer.v_linear_y = (MOTOR_3508[0].spe_now - MOTOR_3508[1].spe_now +MOTOR_3508[2].spe_now- MOTOR_3508[3].spe_now)*0.603217158*_SPEED_RATIO/1000;

float	odom_k =1;

	temp_x +=  (MOVE.odometer.v_linear_y*User_Math_FastCos(MOVE.odometer.ang_rad)+MOVE.odometer.v_linear_x*User_Math_FastSin(MOVE.odometer.ang_rad))/1000/odom_k; //v*1000*0.002s*cos(ang)--mm
	temp_y +=  (MOVE.odometer.v_linear_y*User_Math_FastSin(MOVE.odometer.ang_rad)-MOVE.odometer.v_linear_x*User_Math_FastCos(MOVE.odometer.ang_rad))/1000/odom_k; //--mm	
	//临时矫正方向
//float temp = MOVE.odometer.pos_x;
	MOVE.odometer.pos_x = -temp_y;
  MOVE.odometer.pos_y = temp_x;
	
	
	
	
}
/***************************************************************************************************************************************
																											分离动作
***************************************************************************************************************************************/
//停止
uint8_t User_MoveStop(void) 
{
	int16_t speed1 = User_AbsFloat(MOTOR_3508[0].spe_now);
	int16_t speed2 = User_AbsFloat(MOTOR_3508[1].spe_now);
	int16_t speed3 = User_AbsFloat(MOTOR_3508[2].spe_now);
	int16_t speed4 = User_AbsFloat(MOTOR_3508[3].spe_now);
	
	if((speed1+speed2+speed3+speed4)>200)
	{
		MOVE.ctr_type = CTR_NONE;
		return 0;
	}
	else
		return 1;
}

/**
//机器人旋转到目标角度
//返回是否旋转到位
**/
uint8_t User_TurnToAngle(float ang)
{
	float ang_remain = 0;
	static uint8_t restart = 0;
	
	if(restart == 0)//首次启动旋转
	{
		User_MoveOdomSet(0,0,0);//重置里程计——车头当前方向为0°
		MOVE.angz_pid.target = ang;
		restart = 1;
	}
	
	MOVE.ctr_type = CTR_ROTATE;
	
	ang_remain = User_AbsFloat(IMU1.angz - ang);		
	if(ang_remain > 1)//剩余角度>1°则认为尚未旋转到位	
		return 0;
	else 
	{
		restart = 0;//完成旋转时复位该标记
		return 1;
	}		
}
//行驶到目标距离
uint8_t User_MoveToDist(uint8_t dir,float dist)
{
	dist_remain = 0;
	static uint8_t restart = 0;
	
	if(restart == 0)//首次启动平移
	{
		User_MoveOdomSet(0,0,90);//重置里程计——车头前进方向为90°-Y轴方向
		restart = 1;
	}
	MOVE.ctr_type = CTR_SLIDE;
	
	switch(dir)
	{
		case 0:
		{			
			MOVE.spe_x = 1000;
			MOVE.spe_y = 0;
			ver_diff=User_MoveVerticalPID(200);
			hor_diff=0;
			dist_remain = User_AbsFloat(User_AbsFloat(MOVE.odometer.pos_x) - dist);	
		}break;
		case 1:
		{
			MOVE.spe_x = -1000;
			MOVE.spe_y = 0;
			ver_diff=User_MoveVerticalPID(200);
			hor_diff=0;
			dist_remain = User_AbsFloat(User_AbsFloat(MOVE.odometer.pos_x) - dist);	
		}break;
		case 2:
		{
			MOVE.spe_x = 0;
			MOVE.spe_y = 1000;
			ver_diff=0;
			hor_diff=User_MoveHorizontalPID(150);
			dist_remain = User_AbsFloat(User_AbsFloat(MOVE.odometer.pos_y) - dist);	
		}break;
		case 3:
		{
			MOVE.spe_x = 0;
			MOVE.spe_y = 1000;
			ver_diff=0;
			hor_diff=User_MoveHorizontalPID(950);
			dist_remain = User_AbsFloat(User_AbsFloat(MOVE.odometer.pos_y) - dist);	
		}break;
		case 4:
		{
			MOVE.spe_x = 0;
			MOVE.spe_y = -1000;
			ver_diff=0;
			hor_diff=User_MoveHorizontalPID(950);
			dist_remain = User_AbsFloat(User_AbsFloat(MOVE.odometer.pos_y) - dist);	
		}break;
		default:MOVE.spe_x = 0;MOVE.spe_y = 0;break;
	}	
		
	if(dist_remain > 10)	
		return 0;
	else 
	{
		restart = 0;//完成平移时复位该标记
		ver_diff=0;
		hor_diff=0;
		return 1;
	}	
}
uint8_t User_ToDist(uint8_t dir,float dist)
{
	dist_remain2 = 0;
	static uint8_t restart2 = 0;
	MOVE.ctr_type = CTR_SLIDE;
	if(restart2 == 0)//首次启动平移
	{
		User_MoveOdomSet(0,0,90);//重置里程计——车头前进方向为90°-Y轴方向
		restart2 = 1;
	}
	switch(dir)
	{
		case 0:
		{			
			MOVE.spe_x = -1000;
			MOVE.spe_y = 0;
			ver_diff=User_MoveVerticalPID(200);
			hor_diff=0;
			dist_remain2 =dis5 - dist;	
		}break;
		case 1:
		{			
			MOVE.spe_x = 1000;
			MOVE.spe_y = 0;
			ver_diff=User_MoveVerticalPID(200);
			hor_diff=0;
			dist_remain2 = dis5 - dist;	
		}break;
		case 2:
		{
			MOVE.spe_x = 0;
			MOVE.spe_y = 1000;
			ver_diff=0;
			hor_diff=0;
			dist_remain2 =(dis4+dis1)/2 - dist;	
		}break;
		case 3:
		{
			MOVE.spe_x = 0;
			MOVE.spe_y = -1000;
			ver_diff=0;
			hor_diff=0;
			dist_remain2 =(dis4+dis1)/2 - dist;	
		}break;
		default:MOVE.spe_x = 0;MOVE.spe_y = 0;break;
	}	
		
	if(dist_remain2>0)	
		return 0;
	else 
	{
		restart2=0;
		ver_diff=0;
		hor_diff=0;
		return 1;
	}	
}
uint8_t User_AutoToDist_L(float dist)
{
	dist_remain3=(float)dis5-dist;
	MOVE.ctr_type=CTR_SLIDE;
	if(dist_remain3>-10&&dist_remain3<10) return 1;
	else if(dist_remain3>=10)
	{
		MOVE.spe_x=-1000;
		MOVE.spe_y=0;
		ver_diff=User_MoveVerticalPID(200);
		hor_diff=0;
		return 0;
	}
	else if(dist_remain3<=-10)
	{
		MOVE.spe_x=1000;
		MOVE.spe_y=0;
		ver_diff=User_MoveVerticalPID(200);
		hor_diff=0;
		return 0;
	}
}
uint8_t User_AutoToDist_R(float dist)
{
	dist_remain4=(float)dis2-dist;
	MOVE.ctr_type=CTR_SLIDE;
	if(dist_remain4>-10&&dist_remain4<10) return 1;
	else if(dist_remain4>=10)
	{
		MOVE.spe_x=1000;
		MOVE.spe_y=0;
		ver_diff=User_MoveVerticalPID(200);
		hor_diff=0;
		return 0;
	}
	else if(dist_remain4<=-10)
	{
		MOVE.spe_x=-1000;
		MOVE.spe_y=0;
		ver_diff=User_MoveVerticalPID(200);
		hor_diff=0;
		return 0;
	}
}
uint8_t User_AutoToDist_F(float dist)
{
	dist_remain5=(float)(dis1+dis4)/2.0-dist;
	MOVE.ctr_type=CTR_SLIDE;
	if(dist_remain5>-10&&dist_remain5<10) return 1;
	else if(dist_remain5>=10)
	{
		MOVE.spe_x=0;
		MOVE.spe_y=1000;
		ver_diff=0;
		hor_diff=0;
		return 0;
	}
	else if(dist_remain5<=-10)
	{
		MOVE.spe_x=0;
		MOVE.spe_y=-1000;
		ver_diff=0;
		hor_diff=0;
		return 0;
	}
}
uint8_t User_AutoToDist_C(float dist)
{
	dist_remain6=(float)dis5-dist;
	MOVE.ctr_type=CTR_SLIDE;
	if(dis3<500&&dis3!=0) 
	{
//		User_MoveOdomSet(0,0,90);
		return 1;
	}
	else if(dist_remain6>2)
	{
		MOVE.spe_x=-1000;
		MOVE.spe_y=0;
		ver_diff=User_MoveVerticalPID(200);
		hor_diff=0;
		return 0;
	}
	else if(dist_remain6<-2)
	{
		MOVE.spe_x=1000;
		MOVE.spe_y=0;
		ver_diff=User_MoveVerticalPID(200);
		hor_diff=0;
		return 0;
	}
}
uint8_t User_AngleCheck(void)
{
	ang_dif=(float)dis1-(float)dis4;
	MOVE.ctr_type=CTR_ROTATE;
	if(ang_dif>=15)
	{
		MOVE.spe_ang=-1000;
		return 0;
	}
	else if(ang_dif<=-15)
	{
		MOVE.spe_ang=1000;
		return 0;
	}
	else 
	{
		IMU1.angz=90;
		return 1;
	}
}