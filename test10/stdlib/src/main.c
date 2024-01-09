#include "stm32f10x.h"
#include "led.h"
#include "delay.h"
#include "key.h"
#include "exti.h"


int main(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //设置 NVIC 中断分组 2
    LED_Init(); //初始化与 LED 连接的硬件接口
    KEY_Init(); //初始化与按键连接的硬件接口
    EXTIX_Init(); //外部中断初始化
    GPIO_SetBits(GPIOB, GPIO_Pin_5);
		GPIO_ResetBits(GPIOE, GPIO_Pin_5);
		while(1)
		{
		
		}
}
