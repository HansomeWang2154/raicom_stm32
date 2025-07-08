/**
 * user_uart.c
 * ���� HAL ��� USART1 ���� 16 �ֽڡ�����������жϡ���ԭ���ط� ��ʾ
 */

#include "user_uart1.h"
#include "usart.h"   // �������õ� extern UART_HandleTypeDef huart1

// ���ջ�������������� 16 �ֽ��յ�������
static uint8_t uart1_rx_buffer[UART_RX_LEN];

/**
 * @brief  ��ʼ�������� USART1 16 �ֽ��жϽ���
 *         ���ô˺����󣬵ײ�����յ� 16 �ֽں���� HAL_UART_RxCpltCallback��
 */
void UART1_InterruptRxInit(void)
{
    // ��ջ���������ѡ��
    for (int i = 0; i < UART_RX_LEN; i++)
    {
        uart1_rx_buffer[i] = 0;
    }
    // ����һ�ν��գ����� 16 �ֽڣ������󴥷� RX ����ж�
    HAL_UART_Receive_IT(&huart1, uart1_rx_buffer, UART_RX_LEN);
}

/**
 * @brief  HAL ���յ� 16 �ֽں���Զ����ô˻ص���������Ϊ�����õ��� HAL_UART_Receive_IT��
 * @param  huart: �������� UART ���
 *
 *  �����ֻ���� huart->Instance == USART1��Ȼ�� uart1_rx_buffer �е� 16 �ֽ�ֱ�ӻط���
 *  ��ɺ���������һ�� 16 �ֽڽ��ա�
 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART1)
    {
        // ���� ԭ���ط����������� 16 �ֽ� ���� 
        HAL_UART_Transmit(&huart1, uart1_rx_buffer, UART_RX_LEN, HAL_MAX_DELAY);
        
        // ���� ������һ�� 16 �ֽ��жϽ��� ���� 
        // ��ע�⣺��Ҫ�� Receive_IT(&huart1, ..., len) ��ڶ��������� uart1_rx_buffer
        //  ֮ǰ�����ݣ���Ҫ���ǣ��������ֱ�Ӹ�������Ҳ���ԡ���
        HAL_UART_Receive_IT(&huart1, uart1_rx_buffer, UART_RX_LEN);
    }
}

/**
 * @brief  ����ֱ桰�ص���õ������ݡ��������壬�����������ṩһ������ѯ�ӿڡ���
 *         �������ݲ���Ҫ����Ϊ����ֻҪÿ����������̻ط������������ط��ٷ��ʡ�
 *         �������� uart1_rx_buffer ���û�����鿴���������м��϶�ȡ������
 */

