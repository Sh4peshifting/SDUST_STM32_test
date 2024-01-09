#ifndef __LED_H
#define __LED_H

#include "stm32f10x.h"

#define LED0 GPIO_ResetBits(GPIOB, GPIO_Pin_5)
#define LED1 GPIO_SetBits(GPIOE, GPIO_Pin_5)

void LED_Init(void);  // 初始化

#endif
