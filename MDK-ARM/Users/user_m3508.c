#include "user_m3508.h"
#include "user_can_motor.h"
#include "user_math.h"
#include "user_catch.h"
int pos_dif=100;
PID_TYPEDEF M3508_Spe_PID[8];
PID_TYPEDEF M3508_Pos_PID[8];


int circle_close[4]={0,0,0,0};
int circle_open[4]={0,0,0,0};
int timer3508[4]={0,0,0,0};
void User_M3508_Init(void)
{
	for(int i=0; i<8; i++)
	{
		M3508_Spe_PID[i].err_last = 0;
		M3508_Spe_PID[i].err_now = 0;
		M3508_Spe_PID[i].target = 0;
		M3508_Spe_PID[i].kp = 7.5;
		M3508_Spe_PID[i].ki = 2;
		M3508_Spe_PID[i].kd = 2;
		M3508_Pos_PID[i].kp = 0.3;
		M3508_Pos_PID[i].ki = 0;
		M3508_Pos_PID[i].kd = 0;
	}
}

void User_M3508_SpetoCur_PID(void) 
{	
	for(int i=0;i<4;i++)
	{
		M3508_Spe_PID[i].err_now = MOVE.speed[i] - MOTOR_3508[i].spe_now;
		
		M3508_Spe_PID[i].p_out = M3508_Spe_PID[i].kp * M3508_Spe_PID[i].err_now;
		M3508_Spe_PID[i].i_sum+=M3508_Spe_PID[i].err_now;
		M3508_Spe_PID[i].i_sum=LimitMax(M3508_Spe_PID[i].i_sum,10000);
		M3508_Spe_PID[i].i_out = M3508_Spe_PID[i].ki * M3508_Spe_PID[i].i_sum;
		M3508_Spe_PID[i].i_out = LimitMax(M3508_Spe_PID[i].i_out, 8000); 
		
		M3508_Spe_PID[i].d_out = M3508_Spe_PID[i].kd * (M3508_Spe_PID[i].err_now - M3508_Spe_PID[i].err_last);
		
		M3508_Spe_PID[i].pid_out = M3508_Spe_PID[i].p_out + M3508_Spe_PID[i].i_out + M3508_Spe_PID[i].d_out;
		M3508_Spe_PID[i].pid_out = LimitMax(M3508_Spe_PID[i].pid_out,8000);
		
		M3508_Spe_PID[i].err_last = M3508_Spe_PID[i].err_now;
		MOVE.cur[i] = (int16_t)M3508_Spe_PID[i].pid_out;
	}
	CAN_SendCurGroup3508(0x200, MOVE.cur[0], MOVE.cur[1], MOVE.cur[2], MOVE.cur[3]);
}
void User_M3508_SpetoCur_PID2(int i) 
{	
		M3508_Spe_PID[i].err_now = MOVE.speed[i] - MOTOR_3508[i].spe_now;
		
		M3508_Spe_PID[i].p_out = M3508_Spe_PID[i].kp * M3508_Spe_PID[i].err_now;
		M3508_Spe_PID[i].i_sum+=M3508_Spe_PID[i].err_now;
		M3508_Spe_PID[i].i_sum=LimitMax(M3508_Spe_PID[i].i_sum,5000);
		M3508_Spe_PID[i].i_out = M3508_Spe_PID[i].ki * M3508_Spe_PID[i].i_sum;
		M3508_Spe_PID[i].i_out = LimitMax(M3508_Spe_PID[i].i_out, 5000); 
		
		M3508_Spe_PID[i].d_out = M3508_Spe_PID[i].kd * (M3508_Spe_PID[i].err_now - M3508_Spe_PID[i].err_last);
		
		M3508_Spe_PID[i].pid_out = M3508_Spe_PID[i].p_out + M3508_Spe_PID[i].i_out + M3508_Spe_PID[i].d_out;
		M3508_Spe_PID[i].pid_out = LimitMax(M3508_Spe_PID[i].pid_out,5000);
		
		M3508_Spe_PID[i].err_last = M3508_Spe_PID[i].err_now;
		MOVE.cur[i] = (int16_t)M3508_Spe_PID[i].pid_out;
}
void User_M3508_PostoSpe_PID(int i,int pos) 
{	
		M3508_Pos_PID[i].err_now = pos - MOTOR_3508[i].pos_total;
		
		M3508_Pos_PID[i].p_out = M3508_Pos_PID[i].kp * M3508_Pos_PID[i].err_now;
		
		M3508_Pos_PID[i].i_sum+=M3508_Pos_PID[i].err_now;
		M3508_Pos_PID[i].i_sum=LimitMax(M3508_Pos_PID[i].i_sum,1000);
		M3508_Pos_PID[i].i_out = M3508_Pos_PID[i].ki * M3508_Pos_PID[i].i_sum;
		M3508_Pos_PID[i].i_out = LimitMax(M3508_Pos_PID[i].i_out, 200); 
		
		M3508_Pos_PID[i].d_out = M3508_Pos_PID[i].kd * (M3508_Pos_PID[i].err_now - M3508_Pos_PID[i].err_last);
		
		M3508_Pos_PID[i].pid_out = M3508_Pos_PID[i].p_out + M3508_Pos_PID[i].i_out + M3508_Pos_PID[i].d_out;
		M3508_Pos_PID[i].pid_out = LimitMax(M3508_Pos_PID[i].pid_out,200);
		
		M3508_Pos_PID[i].err_last = M3508_Pos_PID[i].err_now;
		MOVE.speed[i] = (int16_t)M3508_Pos_PID[i].pid_out;
		User_M3508_SpetoCur_PID2(i); 
}	
//int User_Motor3508_Spin(int id,int dir)
//{	
//		if(dir==0) 
//		{
//			if(circle_close[id]>0)
//			{
//				if(MOTOR_3508[id+4].pos_last-MOTOR_3508[id+4].pos_now>pos_dif) circle_close[id]--;
//				MOVE.speed[id+4]=catch_spe[id];//正转
//				MOTOR_3508[id+4].pos_last=MOTOR_3508[id+4].pos_now;
//				User_M3508_SpetoCur_PID2(id+4);
//				return 0;
//			}
//			else if(circle_close[id]<=0) return 1;
//		}
//		else if(dir==1) 
//		{
//			if(circle_open[id]>0)
//			{
//				if(MOTOR_3508[id+4].pos_now-MOTOR_3508[id+4].pos_last>pos_dif) circle_close[id]--;
//				MOVE.speed[id+4]=-catch_spe[id];//反转
//				MOTOR_3508[id+4].pos_last=MOTOR_3508[id+4].pos_now;
//				User_M3508_SpetoCur_PID2(id+4);
//				return 0;
//			}
//			else if(circle_open[id]<=0) return 1;
//		}
//}
int User_Motor3508_Spin(int id,int dir)
{	;
}
//	int flag=0;	
//	if(dir==0) 
//		{
//			if(circle_close[id]>0)
//			{
//				if(MOTOR_3508[id+4].pos_last-MOTOR_3508[id+4].pos_now>100) 
//				{
//					circle_close[id]--;
//					timer3508[id]=0;
//				}
//				if(MOTOR_3508[id+4].pos_now-MOTOR_3508[id+4].pos_last<100) timer3508[id]++;
//				if(timer3508[id]>100) 
//				{
////					catchers[id].round=catchers[id].round-circle_close[id];
//					circle_close[id]=0;
//				}
//				MOVE.speed[id+4]=catch_spe[id];//正转
//				MOTOR_3508[id+4].pos_last=MOTOR_3508[id+4].pos_now;
//				User_M3508_SpetoCur_PID2(id+4);
//			}
//			else if(circle_close[id]<=0) 
//			{
//				circle_open[id]=catchers[id].round;
//				circle_open[2]=catchers[2].round+1;
//				circle_open[3]=catchers[3].round+1;
//				timer3508[id]=0;
//				MOVE.pos[id+4]=4000;
//				User_M3508_PostoSpe_PID(id+4);
//				flag=1;
//			}
//		}
//		else if(dir==1) 
//		{
//			if(circle_open[id]>0)
//			{
//				if(MOTOR_3508[id+4].pos_now-MOTOR_3508[id+4].pos_last>100) 
//				{
//					circle_open[id]--;
//					timer3508[id]=0;
//				}
//				if(MOTOR_3508[id+4].pos_last-MOTOR_3508[id+4].pos_now<100) timer3508[id]++;
//				if(timer3508[id]>100) 
//				{
////					catchers[id].round=catchers[id].round-circle_open[id];
//					circle_open[id]=0;
//				}
//				MOVE.speed[id+4]=-catch_spe[id];//反转
//				MOTOR_3508[id+4].pos_last=MOTOR_3508[id+4].pos_now;
//				User_M3508_SpetoCur_PID2(id+4);
//			}
//			else if(circle_open[id]<=0) 
//			{
//				circle_close[id]=catchers[id].round;
//				
//				timer3508[id]=0;
//				MOVE.pos[id+4]=4000;
//				User_M3508_PostoSpe_PID(id+4);
//				flag=1;
//			}
//		}
//		return flag;
//}


