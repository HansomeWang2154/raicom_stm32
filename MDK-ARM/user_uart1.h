#ifndef __USER_UART_H
#define __USER_UART_H

#include "stm32f1xx_hal.h"

// Ҫ���յĹ̶�����
#define UART_RX_LEN   16

// extern �� usart.h / usart.c �������� UART ���
// extern UART_HandleTypeDef huart1;  // ȷ�������� usart.h �д���

// ���ⲻ��Ҫ̫��ӿڣ�����ֻ�ṩһ�� init �������� User_Init() ����һ�Σ������жϽ���
void UART1_InterruptRxInit(void);

#endif // __USER_UART_H
