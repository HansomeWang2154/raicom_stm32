#include "main.h"
#include "usart.h"
void User_RemoteInit(void);
void User_Remote_Control_IRQHandler(void);
void sbus_process ();
extern int16_t ch[16];//ң����ͨ��
//ch0 ��ת��
//ch1 ��ǰ��
//ch2 ǰ��
//ch3 ƽ��
//ch4 ����
//ch5 ���Զ�
//ch6 ΢������
//ch7 ��λ�л�
extern int main_control;//ģʽ�л�
extern int sci_control;//�����л�
extern int rocker_control;//ҡ���л�
extern int ch6_now;
extern int ch6_last;