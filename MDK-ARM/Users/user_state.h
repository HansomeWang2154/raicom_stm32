#ifndef _user_state_h
#define _user_state_h

#include "main.h"


typedef struct
{
	uint8_t now;
	
	uint16_t angle_now;
	uint16_t angle_target;
	
	uint8_t location_now[2];
	uint8_t location_target[2];
	
	uint8_t in_now;
	uint8_t in_target;
	
}STATE_TYPEDEF;
extern STATE_TYPEDEF STATE;

typedef struct
{
	float x;
	float y;
}POINT_TYPEDEF;
extern POINT_TYPEDEF POINT[8];

void User_StateMachineInit(void);
void User_StateMachineRun(void);
void User_StateMachineTest(void);
void User_test(void);

#endif

