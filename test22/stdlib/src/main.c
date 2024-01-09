#include "adc.h"
#include "led.h"
#include "uart.h"
#include "delay.h"
#include "stdio.h"

int main(void)
{

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    uart_init(115200);
    LED_Init();

    Adc_Init();

    while (1)
    {

        char str[20];
        sprintf(str, "voltage=%.2fV\r\n", Get_Adc_Average(ADC_Channel_1, 10) * (3.3 / 4096));
        uart_SendString(str);
        if (LED0 == 1)
        {
            GPIO_ResetBits(GPIOB, GPIO_Pin_5);
        }
        else
        {
            GPIO_SetBits(GPIOB, GPIO_Pin_5);
        }
        Delay_ms(250);
    }
}
