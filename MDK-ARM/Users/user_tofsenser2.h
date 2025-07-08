#include "usart.h"
#include "main.h"
void User_Tofsenser2Init(void);
void User_Tofsenser2_IRQHandler(void);
void Tofsenser_Paras();//数据处理
extern uint32_t dis1;//前右
extern uint32_t dis2;//右
extern uint32_t dis3;//柱子
extern uint32_t dis4;//前左
extern uint32_t dis5;//左