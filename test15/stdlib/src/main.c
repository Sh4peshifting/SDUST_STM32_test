#include "stm32f10x.h"
#include "delay.h"
#include "led.h"
#include "tim.h"
#include "uart.h"

extern uint8_t TIM5CH1_CAPTURE_STA;  // 输入捕获状态
extern uint16_t TIM5CH1_CAPTURE_VAL; // 输入捕获值

int main(void)
{
    uint32_t temp = 0;
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); // 设置 NVIC 中断分组 2
    uart_init(115200);                              // 串口初始化波特率为 115200
    LED_Init();                                     // LED 端口初始化
    TIM3_PWM_Init(899, 0);                          // 不分频。PWM 频率=72000/(899+1)=80Khz
    TIM5_Cap_Init(0XFFFF, 72 - 1);                  // 以 1Mhz 的频率计数
    while (1)
    {
        Delay_ms(10);
        TIM_SetCompare2(TIM3, TIM_GetCapture2(TIM3) + 1);
        if (TIM_GetCapture2(TIM3) == 300)
            TIM_SetCompare2(TIM3, 0);
        if (TIM5CH1_CAPTURE_STA & 0X80) // 成功捕获到了一次上升沿
        {
            temp = TIM5CH1_CAPTURE_STA & 0X3F;
            temp *= 65536;                  // 溢出时间总和
            temp += TIM5CH1_CAPTURE_VAL;    // 得到总的高电平时间
            char str[20];
            sprintf(str, "HIGH:%d us\r\n", temp); 
            uart_SendString(str);
            TIM5CH1_CAPTURE_STA = 0;        // 开启下一次捕获
        }
    }
}
