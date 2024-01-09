#include "stm32f10x.h"
#include "delay.h"
#include "led.h"
#include "tim.h"

int main(void)
{
    
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    // uart_init(115200);
    LED_Init();
    TIM3_Int_Init(4999, 7199);
    while (1)
    {
        if (LED0 == 1)
        {
					GPIO_ResetBits(GPIOB, GPIO_Pin_5);
            
        }
        else
        {
            GPIO_SetBits(GPIOB, GPIO_Pin_5);
        }
				Delay_ms(200);

        
    }
}
