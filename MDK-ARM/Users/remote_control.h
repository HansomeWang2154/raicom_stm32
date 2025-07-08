#include "main.h"
#include "usart.h"
void User_RemoteInit(void);
void User_Remote_Control_IRQHandler(void);
void sbus_process ();
extern int16_t ch[16];//遥控器通道
//ch0 右转弯
//ch1 右前进
//ch2 前进
//ch3 平移
//ch4 剪刀
//ch5 手自动
//ch6 微调开关
//ch7 档位切换
extern int main_control;//模式切换
extern int sci_control;//剪刀切换
extern int rocker_control;//摇杆切换
extern int ch6_now;
extern int ch6_last;