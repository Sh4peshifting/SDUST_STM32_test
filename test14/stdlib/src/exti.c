#include "exti.h"
#include "led.h"
#include "key.h"
#include "delay.h"

// 外部中断 0 服务程序
void EXTIX_Init(void)
{
    EXTI_InitTypeDef EXTI_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    KEY_Init();                                          // 按键端口初始化
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE); // 开启 AFIO 时钟
    // GPIOE.2 中断线以及中断初始化配置,下降沿触发
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOE, GPIO_PinSource2); //
    EXTI_InitStructure.EXTI_Line = EXTI_Line2;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling; // 下降沿触发
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);                              // 初始化中断线参数
    NVIC_InitStructure.NVIC_IRQChannel = EXTI2_IRQn;             // 使能按键外部中断通道
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02; // 抢占优先级 2，
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;        // 子优先级 2
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;              // 使能外部中断通道
    NVIC_Init(&NVIC_InitStructure);                              // 初始化 NVIC
}

// 外部中断 2 服务程序
void EXTI2_IRQHandler(void)
{
    Delay(20);     // 消抖
    if (KEY2 == 0) // 按键 KEY2
    {
        if (GPIO_ReadOutputDataBit(GPIOB, GPIO_Pin_5))
            GPIO_ResetBits(GPIOB, GPIO_Pin_5);
        else
            GPIO_SetBits(GPIOB, GPIO_Pin_5);
    }
    EXTI_ClearITPendingBit(EXTI_Line2); // 清除 LINE2 上的中断标志位
}
