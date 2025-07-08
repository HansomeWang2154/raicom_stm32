#ifndef _user_timer_h
#define _user_timer_h

#include "main.h"

extern uint8_t cross_flag;

void User_TimerInit(void);
void User_Timer100usIRQ(void);
void User_Timer1msIRQ(void);
void TimerDelayUs(uint16_t us);
void User_DelayMs(uint16_t ms);

#endif










