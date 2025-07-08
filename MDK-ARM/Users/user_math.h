#ifndef _user_math_h
#define _user_math_h

#include "main.h"

#define _PI 3.1415926f


uint32_t __FastIntAtan2__(int16_t x, int16_t y);
float LimitMax(float data,float max);
float User_Math_FastSin(float theta);
float User_Math_FastCos(float theta);
float User_AbsFloat(float data);
int User_AbsInt(int data);

#endif

