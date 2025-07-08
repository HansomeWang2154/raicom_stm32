#include "user_m3510.h"

PID_TYPEDEF M3510_Spe_PID[4];
PID_TYPEDEF	M3510_Pos_PID[4];

MOVE_TYPEDEF MOVE_3510;
int spin10_state_c[4]={0,0,0,0};
int spin10_state_o[4]={0,0,0,0};
int timer3510[4]={0,0,0,0};
void User_M3510_Init()
{
	for(int i=0; i<4; i++)
	{
		M3510_Spe_PID[i].err_last = 0;
		M3510_Spe_PID[i].err_now = 0;
		M3510_Spe_PID[i].target = 0;
		MOTOR_3508[i+4].pos_last=MOTOR_3508[i+4].pos_now;
		M3510_Spe_PID[i].kp = 4;
		M3510_Spe_PID[i].ki = 0;
		M3510_Spe_PID[i].kd = 0.5;
	}
	for(int i=0; i<4; i++)
	{
		M3510_Pos_PID[i].err_last = 0;
		M3510_Pos_PID[i].err_now = 0;
		M3510_Pos_PID[i].target = 0;
		M3510_Pos_PID[i].kp = 0.3;
		M3510_Pos_PID[i].ki = 0;
		M3510_Pos_PID[i].kd = 0;
	}
	M3510_Spe_PID[0].kp = 7;
	M3510_Spe_PID[0].ki = 0;
	M3510_Spe_PID[0].kd = 0.9;
	M3510_Pos_PID[0].kp = 2;
	M3510_Pos_PID[0].ki = 0;
	M3510_Pos_PID[0].kd = 0.1;
}
void User_M3510_SpetoCur_PID(int i) 
{	
		M3510_Spe_PID[i].err_now = MOVE.speed[i+4] - MOTOR_3508[i+4].spe_now;
		
		M3510_Spe_PID[i].p_out = M3510_Spe_PID[i].kp * M3510_Spe_PID[i].err_now;
		
		M3510_Spe_PID[i].i_sum+=M3510_Spe_PID[i].err_now;
		M3510_Spe_PID[i].i_sum=LimitMax(M3510_Spe_PID[i].i_sum,5000);
		M3510_Spe_PID[i].i_out = M3510_Spe_PID[i].ki * M3510_Spe_PID[i].i_sum;
		M3510_Spe_PID[i].i_out = LimitMax(M3510_Spe_PID[i].i_out, 8000); 
		
		M3510_Spe_PID[i].d_out = M3510_Spe_PID[i].kd * (M3510_Spe_PID[i].err_now - M3510_Spe_PID[i].err_last);
		
		M3510_Spe_PID[i].pid_out = M3510_Spe_PID[i].p_out + M3510_Spe_PID[i].i_out + M3510_Spe_PID[i].d_out;
		M3510_Spe_PID[i].pid_out = LimitMax(M3510_Spe_PID[i].pid_out,8000);
		
		M3510_Spe_PID[i].err_last = M3510_Spe_PID[i].err_now;
		MOVE.cur[i+4] = (int16_t)M3510_Spe_PID[i].pid_out;
//	CAN_SendCurGroup3508(0x1FF, MOVE_3510.cur[0], MOVE_3510.cur[1], MOVE_3510.cur[2], MOVE_3510.cur[3]);
}	
void User_M3510_PostoSpe_PID(int i,int pos) 
{	
		M3510_Pos_PID[i].err_now =pos- MOTOR_3508[i+4].pos_total;
		
		M3510_Pos_PID[i].p_out = M3510_Pos_PID[i].kp * M3510_Pos_PID[i].err_now;
		
		M3510_Pos_PID[i].i_sum+=M3510_Pos_PID[i].err_now;
		M3510_Pos_PID[i].i_sum=LimitMax(M3510_Pos_PID[i].i_sum,10000);
		M3510_Pos_PID[i].i_out = M3510_Pos_PID[i].ki * M3510_Pos_PID[i].i_sum;
		M3510_Pos_PID[i].i_out = LimitMax(M3510_Pos_PID[i].i_out, 2000); 
		
		M3510_Pos_PID[i].d_out = M3510_Pos_PID[i].kd * (M3510_Pos_PID[i].err_now - M3510_Pos_PID[i].err_last);
		
		M3510_Pos_PID[i].pid_out = M3510_Pos_PID[i].p_out + M3510_Pos_PID[i].i_out + M3510_Pos_PID[i].d_out;
		M3510_Pos_PID[i].pid_out = LimitMax(M3510_Pos_PID[i].pid_out,2000);
		
		M3510_Pos_PID[i].err_last = M3510_Pos_PID[i].err_now;
		MOVE.speed[i+4] = (int16_t)M3510_Pos_PID[i].pid_out;
		User_M3510_SpetoCur_PID(i); 
}	
//int User_Motor3510_Spin(int id,int dir)
//{
//	if(circle[id]>0)
//	{
//		if(MOTOR_3508[id+4].pos_last-MOTOR_3508[id+4].pos_now>100) circle[id]--;
//		MOTOR_3508[id+4].pos_last=MOTOR_3508[id+4].pos_now;
//		if(dir==0) MOVE_3510.speed[id]=200;//正转
//		else if(dir==1) MOVE_3510.speed[id]=-200;//反转
//		User_M3510_SpetoCur_PID(id);
//		return 0;
//	}
//	else if(circle[id]<=0) return 1;
//}
int User_Motor3510_Spin(int id,int dir)
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
//					timer3510[id]=0;
//				}
//				if(MOTOR_3508[id+4].pos_now-MOTOR_3508[id+4].pos_last<100) timer3510[id]++;
//				if(timer3510[id]>300) 
//				{
////					catchers[id].round=catchers[id].round-circle_close[id];
//					circle_close[id]=0;
//				}
//				MOVE.speed[id+4]=catch_spe[id];//正转
//				MOTOR_3508[id+4].pos_last=MOTOR_3508[id+4].pos_now;
//				User_M3510_SpetoCur_PID(id);
//			}
//			else if(circle_close[id]<=0) 
//			{
//				switch(spin10_state_c[id])
//				{
//					case 0:
//					{
//						circle_open[id]=catchers[id].round;
//						timer3510[id]=0;
//						if(MOTOR_3508[id+4].pos_now>7000) MOVE.pos[id+4]=7000;
//						else if(MOTOR_3508[id+4].pos_now<1000) MOVE.pos[id+4]=1000;
//						else MOVE.pos[id+4]=MOTOR_3508[id+4].pos_now;
//						spin10_state_c[id]=1;
//						spin10_state_o[id]=0;
//						flag=1;
//					}
//					break;
//					case 1:
//					{
//							User_M3510_PostoSpe_PID(id);
//					}
//					break;
//				}
//			}
//		}
//		else if(dir==1) 
//		{
//			if(circle_open[id]>0)
//			{
//				if(MOTOR_3508[id+4].pos_now-MOTOR_3508[id+4].pos_last>100) 
//				{
//					circle_open[id]--;
//					timer3510[id]=0;
//				}
//				if(MOTOR_3508[id+4].pos_last-MOTOR_3508[id+4].pos_now<100) timer3510[id]++;
//				if(timer3510[id]>300) 
//				{
////					catchers[id].round=catchers[id].round-circle_open[id];
//					circle_open[id]=0;
//				}
//				MOVE.speed[id+4]=-catch_spe[id];//反转
//				MOTOR_3508[id+4].pos_last=MOTOR_3508[id+4].pos_now;
//				User_M3510_SpetoCur_PID(id);
//			}
//			else if(circle_open[id]<=0) 
//			{
//				switch(spin10_state_o[id])
//				{
//					case 0:
//					{
//						circle_close[id]=catchers[id].round;
//						timer3510[id]=0;
//						if(MOTOR_3508[id+4].pos_now>7000) MOVE.pos[id+4]=7000;
//						else if(MOTOR_3508[id+4].pos_now<1000) MOVE.pos[id+4]=1000;
//						else MOVE.pos[id+4]=MOTOR_3508[id+4].pos_now;
//						spin10_state_o[id]=1;
//						spin10_state_c[id]=0;
//						flag=1;
//					}
//					break;
//					case 1:
//					{
//							User_M3510_PostoSpe_PID(id);
//					}
//					break;
//				}
//			}
//		}
//		return flag;
//}
