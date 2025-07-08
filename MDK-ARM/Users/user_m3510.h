#ifndef _USER_M3510_H	
#define _USER_M3510_H	

#include "main.h"
#include "user_movebase.h"
#include "user_can_motor.h"
#include "user_math.h"
#include "user_m3508.h"
#include "user_catch.h"
void User_M3510_Init();
void User_M3510_SpetoCur_PID(int i);
//void User_M3510_PostoSpe_PID(int i);
int User_Motor3510_Spin(int id,int dir);
void User_M3510_PostoCur_PID(int i,int pos);
#endif