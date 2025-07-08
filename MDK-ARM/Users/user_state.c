#include "user_state.h" 
#include "user_imu.h"
#include "user_movebase.h"
#include "user_comm_wifi.h" 
#include "user_math.h"
#include "user_can_motor.h"
#include "user_tasks.h"
#define Start 0
uint8_t sta_count=0;
STATE_TYPEDEF STATE;

POINT_TYPEDEF POINT[8] = { 
   0,0,			//0//ԭ��
   0,6950,	//1//��ʼ��
	737.5,6950, //2//���˵�
	3868.25,6950,//3//���������һ����
	100,6950,	//4//��������ڶ�����
  100,6950,		//5//���������������
	100,6950,		//6//����������ĸ���
	737.5,0, //7//��
};
POINT_TYPEDEF TUI_POINT[8] = { 
   0,0,			//0//ԭ��
   0,6950,	//1//��ʼ��
	737.5,6950, //2//���˵�
	3868.25,6950,//3//���������һ����
	3306.25,6950,	//4//��������ڶ�����
  2743.75,6950,		//5//���������������
	2181.25,6950,		//6//����������ĸ���
	737.5,0, //7//��
};
POINT_TYPEDEF TOF_POINT[8]={
		0,0,			//0
   0,6950,	//1
	737.5,6950, //2
	120,6950,//3
	682.5,6950,	//4
	1245,6950,		//5
	1807.5,6950,		//6
	737.5,0, //7
};
int target=3;
uint32_t count=0;
extern uint8_t STATE_MACHINE_EN;
/*****************************************************************************************
��ʼ��
*****************************************************************************************/
void User_StateMachineInit(void)
{
	STATE.angle_now=90;
	STATE.location_now[0]=0;
}
/*****************************************************************************************
״̬��
*****************************************************************************************/
void User_test(void)
{
	static uint8_t steps = 0;
	switch(steps)
	{
		case Start:
			{
				if(User_MoveToDist(2,User_AbsFloat(POINT[1].y-POINT[STATE.location_now[0]].y)))
				{
					steps++;
					sta_count++;
					STATE.location_now[0]=1;
				}
			}
			break;
		case Start+1:
			{
				if(User_MoveToDist(1,User_AbsFloat(POINT[target].x-POINT[STATE.location_now[0]].x)))//������Ŀ�����λ��
				{
						steps++;
						sta_count++;
						STATE.location_now[0]=target;
				}
			}
			break;
			case Start+2:
			{
				if(User_ToDist(0,User_AbsFloat(TOF_POINT[target].x)))//������Ŀ��㾫ȷλ��
					{
						steps++;
						sta_count++;
				}
			}
			break;
			case Start+3:
			{
				if(User_ToDist(2,80))//�������λ��
					{
						steps++;
						sta_count++;
				}
			}
			break;
			case Start+4:
			{
				if(User_MoveStop())//ִ�м�ƻ��
				{
					count++;
				if(jian_count<30) jian_count++;
				if(jian_count<10)
				{
					CAN_SendPosSingle(1,jian1);
				}
				else if(jian_count>=10&&jian_count<20)
				{
					CAN_SendPosSingle(2,jian2);
				}
				else 
				{
					jian_count=40;
				}
					if(count>1000)
					{
						steps++;
						sta_count++;
						count=0;
					}
				}
			}
			break;
			case Start+5:
			{
				if(User_MoveToDist(0,User_AbsFloat(TUI_POINT[2].x-TUI_POINT[STATE.location_now[0]].x)))//���һص����˵�
				{
					steps++;
					sta_count++;
					STATE.location_now[0]=2;
				}
			}
			break;
			case Start+6:
			{
				if(User_MoveToDist(4,User_AbsFloat(POINT[7].y-POINT[STATE.location_now[0]].y)))//��������
				{
					steps++;
					sta_count++;
					STATE.location_now[0]=7;
				}
			}
			break;
			case Start+7:
			{
				if(User_TurnToAngle(90))//�����   ��������ת ��������ת
				{
					steps++;
					sta_count++;
				}
			}
			break;
			case Start+8:
			{
				if(User_ToDist(2,10))//�����λ��
					{
						steps++;
						sta_count++;
				}
			}
			break;
			case Start+9:
			{
				if(User_MoveStop())//ִ�з�ƻ��
				{
					count++;
					CAN_SendPosAll(0x03,-20000);
					if(target<7)
					{
						if(count>1000)
						{
							steps++;
							sta_count++;
							target++;
							count=0;
						}
					}
				}
			}
			break;
			case Start+10:
			{
				if(User_ToDist(3,100))//�Կ��λ��������
					{
						steps++;
						sta_count++;
				}
			}
			break;
			case Start+11:
			{
				if(User_TurnToAngle(-90))//����y��  ��������ת ��������ת
				{
					steps++;
					sta_count++;
				}
			}
			break;
			case Start+12:
			{
				if(User_MoveToDist(3,User_AbsFloat(POINT[2].y-POINT[STATE.location_now[0]].y)))//������˵�
				{
					STATE.location_now[0]=2;
					if(target<7) 
					{
						steps=Start+1;
						sta_count=Start+1;
					}
					else STATE_MACHINE_EN=0;
				}
			}
			break;
		}
	
}


