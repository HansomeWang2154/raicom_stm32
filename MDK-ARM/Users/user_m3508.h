#ifndef _USER_M3508_H	
#define _USER_M3508_H	

#include "main.h"
#include "user_movebase.h"
extern PID_TYPEDEF M3508_Spe_PID[8];
extern int circle_close[];
extern int circle_open[];
void User_M3508_Init(void);
void User_M3508_SpetoCur_PID(void);
void User_M3508_SpetoCur_PID(void);
void User_M3508_SpetoCur_PID2(int i);
void User_M3508_PostoSpe_PID(int i,int pos);
int User_Motor3508_Spin(int id,int dir);
#endif
