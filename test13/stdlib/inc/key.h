#ifndef __KEY_H
#define __KEY_H

#include "stm32f10x.h"

#define KEY0 GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_4)  // 读取按键 0
#define KEY1 GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_3)  // 读取按键 1
#define KEY2 GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_2)  // 读取按键 2
#define WK_UP GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0) // 读取按键 3(WK_UP)
#define KEY0_PRES 1                                    // KEY0 按下
#define KEY1_PRES 2                                    // KEY1 按下
#define KEY2_PRES 3                                    // KEY2 按下
#define WKUP_PRES 4                                    // WK_UP 按下(即 WK_UP/WK_UP)
void KEY_Init(void);                                   // IO 初始化
uint8_t KEY_Scan(uint8_t);                                       // 按键扫描函数

#endif
