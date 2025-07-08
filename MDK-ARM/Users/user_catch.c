//#include "user_catch.h"

//int catch_spe[4]={1000,1500,3500,3500};
//int catch_set_spe[4]={600,600,600,600};
//int set_flag[4]={0,0,0,0};
//int caughter_posnow[4]={0,0,0,0};
//catcher catchers[4];
//catcher caughter[4];
//int middle_pos=0;
//int mid_state=0;
//void Catcher_Init()
//{
//	for(int i=0;i<4;i++)
//	{
//		catchers[i].close=0;
//		catchers[i].open=0;
//		catchers[i].round=-1;
//		catchers[i].c_state=0;
//		catchers[i].o_state=0;
//	}
//}
//int Caught_Set(int id,int dir)
//{
//	if(dir==0)//正转
//	{
//		switch(caughter[id].s_state)
//		{
//			case 0:
//			{
//				if(id==0) caughter[id].close=MOTOR_3508[id+4].pos_total+200;
//				else caughter[id].close=MOTOR_3508[id+4].pos_total;
//				MOVE.speed[id+4]=catch_set_spe[id];
//				if(id<=1) User_M3510_SpetoCur_PID(id);
//				else if(id>1) User_M3508_SpetoCur_PID2(id+4);
//				if(MOTOR_3508[id+4].spe_now>100)
//				{
//					caughter[id].s_state=1;
//				}
//			}
//			break;
//			case 1:
//			{
//				if(MOTOR_3508[id+4].spe_now==0)
//				{
//					if(id==0) caughter[id].open=MOTOR_3508[id+4].pos_total-400;
//					else caughter[id].open=MOTOR_3508[id+4].pos_total;
//					if(id==0) middle_pos=MOTOR_3508[id+4].pos_total-1200;
//					caughter[id].s_state=2;
//					if(id==0) caughter_posnow[id]=caughter[id].close;
//					else caughter_posnow[id]=caughter[id].open;
//				}
//				else 
//				{
//					MOVE.speed[id+4]=catch_set_spe[id];
//					if(id<=1) User_M3510_SpetoCur_PID(id);
//					else if(id>1) User_M3508_SpetoCur_PID2(id+4);
//				}
//			}
//			break;
//			case 2:
//			{
//				set_flag[id]=1;
//			}
//			break;
//		}
//	}
//	else if(dir==1)//反转
//	{
//		switch(caughter[id].s_state)
//		{
//			case 0:
//			{
//				if(id==0)caughter[id].close=MOTOR_3508[id+4].pos_total+600;
//				else if(id==3)caughter[id].close=MOTOR_3508[id+4].pos_total+400;
//				else if(id==2) caughter[id].close=MOTOR_3508[id+4].pos_total+200;
//				else caughter[id].close=MOTOR_3508[id+4].pos_total;
//				MOVE.speed[id+4]=-catch_set_spe[id];
//				if(id<=1) User_M3510_SpetoCur_PID(id);
//				else if(id>1) User_M3508_SpetoCur_PID2(id+4);
//				if(MOTOR_3508[id+4].spe_now<-100)
//				{
//					caughter[id].s_state=1;
//				}
//			}
//			break;
//			case 1:
//			{
//				if(MOTOR_3508[id+4].spe_now==0)
//				{
//					if(id==0)caughter[id].open=MOTOR_3508[id+4].pos_total-300;
//					else caughter[id].open=MOTOR_3508[id+4].pos_total;
//					caughter[id].s_state=2;
//					if(id==0) caughter_posnow[id]=caughter[id].close;
//					else caughter_posnow[id]=caughter[id].open;
//					
//				}
//				else 
//				{
//					MOVE.speed[id+4]=-catch_set_spe[id];
//					if(id<=1) User_M3510_SpetoCur_PID(id);
//					else if(id>1) User_M3508_SpetoCur_PID2(id+4);
//				}
//			}
//			break;
//			case 2:
//			{
//				set_flag[id]=1;
//			}
//			break;
//		}
//	}
//	switch(id)
//	{
//		case 1:
//		{
//			MOVE.cur[4]=0;
//		}
//		break;
//		case 2:
//		{
//			MOVE.cur[4]=0;
//			MOVE.cur[5]=0;
//		}
//		break;
//		case 3:
//		{
//			MOVE.cur[4]=0;
//			MOVE.cur[5]=0;
//			MOVE.cur[6]=0;
//		}
//		break;
//	}
//	CAN_SendCurGroup3508(0x1FF,MOVE.cur[4],MOVE.cur[5],MOVE.cur[6],MOVE.cur[7]);
//	return set_flag[id];
//}
//void Rocker_MidSpin()
//{
//	User_M3510_PostoSpe_PID(0,middle_pos);
//}
//int Caughter_Spin(int id,int dir)
//{
//	if(dir==0)
//	{
//		switch(caughter[id].o_state)
//		{
//			case 0:
//			{
////				CAN_SendCurGroup3508(0x200,0,0,0,0);
//				if(caughter[id].open>caughter[id].close) 
//				{
////					if(id==0)MOVE.speed[id+4]=catch_spe[id]*User_AbsInt(caughter[id].open-MOTOR_3508[id+4].pos_total)
////					/User_AbsInt(caughter[id].open-caughter[id].close);
//					MOVE.speed[id+4]=catch_spe[id];
//					if(id<=1) User_M3510_SpetoCur_PID(id);
//					else if(id>1) User_M3508_SpetoCur_PID2(id+4);
//					if(id==0)
//					{
//						if(caughter[id].open-MOTOR_3508[id+4].pos_total<1000)caughter[id].o_state=1;
//					}
//					else 
//					{
//						if(caughter[id].open-MOTOR_3508[id+4].pos_total<200)caughter[id].o_state=1;
//					}
//				}
//				else if(caughter[id].open<caughter[id].close) 
//				{
////					if(id==0) MOVE.speed[id+4]=-catch_spe[id]*User_AbsInt(caughter[id].open-MOTOR_3508[id+4].pos_total)
////					/User_AbsInt(caughter[id].open-caughter[id].close);
//					 MOVE.speed[id+4]=-catch_spe[id];
//					if(id<=1) User_M3510_SpetoCur_PID(id);
//					else if(id>1) User_M3508_SpetoCur_PID2(id+4);
//					if(id==0)
//					{
//						if(MOTOR_3508[id+4].pos_total-caughter[id].open<1000) caughter[id].o_state=1;
//					}
//					else 
//					{
//						if(MOTOR_3508[id+4].pos_total-caughter[id].open<100) caughter[id].o_state=1;
//					}
//				}
//			}
//			break;
//			case 1:
//			{
//				if(id<=1) User_M3510_PostoSpe_PID(id,caughter[id].open);
//				else if(id>1) User_M3508_PostoSpe_PID(id+4,caughter[id].open);
//				caughter_posnow[id]=caughter[id].open;
//				caughter[id].c_state=0;
//			}
//			break;
//		}
//	}
//	else if(dir==1)
//	{
//		switch(caughter[id].c_state)
//		{
//			case 0:
//			{
////				CAN_SendCurGroup3508(0x200,0,0,0,0);
//				if(caughter[id].open>caughter[id].close) 
//				{
////					if(id==0) MOVE.speed[id+4]=-catch_spe[id]*User_AbsInt(caughter[id].close-MOTOR_3508[id+4].pos_total)
////					/User_AbsInt(caughter[id].open-caughter[id].close);
//					MOVE.speed[id+4]=-catch_spe[id];
//					if(id<=1) User_M3510_SpetoCur_PID(id);
//					else if(id>1) User_M3508_SpetoCur_PID2(id+4);
//					if(id==0) 
//					{
//						if(MOTOR_3508[id+4].pos_total-caughter[id].close<1000)caughter[id].c_state=1;
//					}
//					else 
//					{
//						if(MOTOR_3508[id+4].pos_total-caughter[id].close<100)caughter[id].c_state=1;
//					}
//				}
//				else if(caughter[id].open<caughter[id].close) 
//				{
////					if(id==0) MOVE.speed[id+4]=catch_spe[id]*User_AbsInt(caughter[id].close-MOTOR_3508[id+4].pos_total)
////					/User_AbsInt(caughter[id].open-caughter[id].close);
//					MOVE.speed[id+4]=catch_spe[id];
//					if(id<=1) User_M3510_SpetoCur_PID(id);
//					else if(id>1) User_M3508_SpetoCur_PID2(id+4);
//					if(id==0)
//					{
//						if(caughter[id].close-MOTOR_3508[id+4].pos_total<1000) caughter[id].c_state=1;
//					}
//					else 
//					{
//						if(caughter[id].close-MOTOR_3508[id+4].pos_total<100) caughter[id].c_state=1;
//					}
//				}
//			}
//			break;
//			case 1:
//			{
//				if(id<=1) User_M3510_PostoSpe_PID(id,caughter[id].close);
//				else if(id>1) User_M3508_PostoSpe_PID(id+4,caughter[id].close);
//				caughter_posnow[id]=caughter[id].close;
//				caughter[id].o_state=0;
//			}
//			break;
//		}
//	}
//}
//void Caughters_Set()
//{
//	if(Caught_Set(0,0))
//		{
//			if(Caught_Set(1,1))
//			{
//				if(Caught_Set(2,1))
//				{
//					if(Caught_Set(3,1))
//					{
//						MOVE.motor_en = 1;
//						MOVE.cur[4]=0;
//					}
//				}
//			}	
//		}
//}
//void Caughters_Control()
//{
//	if(main_control==0)
//		{
//			HAL_GPIO_WritePin(GPIOA,GPIO_PIN_8,GPIO_PIN_RESET);
//			if(sci_control==0)
//			{
//				Caughter_Spin(2,0);
//				Caughter_Spin(3,0);
//				User_M3510_PostoSpe_PID(0,caughter_posnow[0]);
//				User_M3510_PostoSpe_PID(1,caughter_posnow[1]);
//			}
//			else if(sci_control==1)
//			{
//				Caughter_Spin(2,1);
//				Caughter_Spin(3,1);
//				User_M3510_PostoSpe_PID(0,caughter_posnow[0]);
//				User_M3510_PostoSpe_PID(1,caughter_posnow[1]);
//			}
//			CAN_SendCurGroup3508(0x1FF,MOVE.cur[4],MOVE.cur[5],MOVE.cur[6],MOVE.cur[7]);
//		}
//		else if(main_control==1)
//		{
//			User_M3510_PostoSpe_PID(2,caughter_posnow[2]);
//			User_M3510_PostoSpe_PID(3,caughter_posnow[3]);
//			HAL_GPIO_WritePin(GPIOA,GPIO_PIN_8,GPIO_PIN_SET);
//			if(sci_control==0)
//			{
//				Caughter_Spin(1,0);
//			}
//			else if(sci_control==1)
//			{
//				Caughter_Spin(1,1);
//			}
//			if(rocker_control==0)
//			{
//				Caughter_Spin(0,0);
//			}
//			else if(rocker_control==1)
//			{
//				Rocker_MidSpin();
//			}
//			else if(rocker_control==2)
//			{
//				Caughter_Spin(0,1);
//			}
//			CAN_SendCurGroup3508(0x1FF,MOVE.cur[4],MOVE.cur[5],MOVE.cur[6],MOVE.cur[7]);
//		}
//	}

//		