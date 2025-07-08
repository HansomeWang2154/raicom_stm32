/**
 * user_uart.c
 * 基于 HAL 库的 USART1 定长 16 字节——接收完成中断——原样回发 演示
 */

#include "user_uart1.h"
#include "usart.h"   // 必须能拿到 extern UART_HandleTypeDef huart1

// 接收缓冲区：用来存放 16 字节收到的数据
static uint8_t uart1_rx_buffer[UART_RX_LEN];

/**
 * @brief  初始化并启动 USART1 16 字节中断接收
 *         调用此函数后，底层会在收到 16 字节后进入 HAL_UART_RxCpltCallback。
 */
void UART1_InterruptRxInit(void)
{
    // 清空缓冲区（可选）
    for (int i = 0; i < UART_RX_LEN; i++)
    {
        uart1_rx_buffer[i] = 0;
    }
    // 启动一次接收：接收 16 字节，收满后触发 RX 完成中断
    HAL_UART_Receive_IT(&huart1, uart1_rx_buffer, UART_RX_LEN);
}

/**
 * @brief  HAL 库收到 16 字节后会自动调用此回调函数（因为我们用的是 HAL_UART_Receive_IT）
 * @param  huart: 被触发的 UART 句柄
 *
 *  本例里，只关心 huart->Instance == USART1，然后将 uart1_rx_buffer 中的 16 字节直接回发，
 *  完成后再重启下一轮 16 字节接收。
 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
//    if (huart->Instance == USART1)
//    {
//        // —— 原样回发：阻塞传输 16 字节 —— 
//        HAL_UART_Transmit(&huart1, uart1_rx_buffer, UART_RX_LEN, HAL_MAX_DELAY);
//        
//        // —— 重启下一轮 16 字节中断接收 —— 
//        // （注意：不要用 Receive_IT(&huart1, ..., len) 里第二个参数传 uart1_rx_buffer
//        //  之前有数据，若要覆盖，先清零或直接覆盖数据也可以。）
//        HAL_UART_Receive_IT(&huart1, uart1_rx_buffer, UART_RX_LEN);
//    }
}

/**
 * @brief  如需分辨“回调里得到的数据”有无意义，可以在这里提供一个“查询接口”。
 *         本例中暂不需要，因为我们只要每次收完就立刻回发，不在其他地方再访问。
 *         如果你想把 uart1_rx_buffer 给用户代码查看，可以自行加上读取函数。
 */

