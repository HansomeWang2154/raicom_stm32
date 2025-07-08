#ifndef __USER_UART_H
#define __USER_UART_H

#include "stm32f1xx_hal.h"

// 要接收的固定长度
#define UART_RX_LEN   16

// extern 在 usart.h / usart.c 中声明的 UART 句柄
// extern UART_HandleTypeDef huart1;  // 确保此行在 usart.h 中存在

// 对外不需要太多接口：这里只提供一个 init 函数，供 User_Init() 调用一次，启动中断接收
void UART1_InterruptRxInit(void);

#endif // __USER_UART_H
