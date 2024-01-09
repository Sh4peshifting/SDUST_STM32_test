#ifndef __TIM_H
#define __TIM_H

#include "stm32f10x.h"

void TIM3_Int_Init(uint16_t arr,uint16_t psc);
void TIM3_PWM_Init(u16 arr, u16 psc);
void TIM5_Cap_Init(uint16_t arr, uint16_t psc);


#endif
