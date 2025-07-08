#ifndef _USER_CATCH_H
#define _USER_CATCH_H

#include "main.h"
#include "user_m3508.h"
#include "user_m3510.h"
#include "user_can_motor.h"
#include "remote_control.h"

typedef struct
{
	int c_state;
	int s_state;
	int o_state;
	int close;
	int open;
	int round;
}catcher;
extern catcher catchers[];
extern int catch_spe[];
int Catcher_Close(int id,int dir);
int Catcher_Close2(int id,int dir);
void Catcher_Open(int id);
int cal_round(int id);
int Catch_Set();//夹手圈数统计
int Caughter_Spin(int id,int dir);
void Caughters_Set();//初始化
#endif