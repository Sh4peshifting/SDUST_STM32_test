#include "stm32f10x.h"
#include "delay.h"
#include "led.h"
#include "tim.h"

int main(void)
{
    uint16_t led0pwmval = 0;
    uint8_t dir = 1;
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); // 设置 NVIC 中断分组 2
    LED_Init();                                     // LED 端口初始化
    TIM3_PWM_Init(899, 0);                          // 不分频,PWM 频率=72000/900=80Khz
    while (1)
    {
        Delay_ms(10);
        if (dir)
            led0pwmval++;
        else
            led0pwmval--;
        if (led0pwmval > 300)
            dir = 0;
        if (led0pwmval == 0)
            dir = 1;
        TIM_SetCompare2(TIM3, led0pwmval);
    }
}
