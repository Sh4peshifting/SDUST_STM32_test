#include "dma.h"
#include "led.h"
#include "uart.h"
#include "key.h"
#include "delay.h"
#include "stdio.h"

#define SEND_BUF_SIZE 1900
uint8_t SendBuff[SEND_BUF_SIZE];
const uint8_t TEXT_TO_SEND[] = {"STM32F1 DMA Test"};
int main(void)
{
    uint16_t i;
    uint8_t t = 0;
    uint8_t j, mask = 0;

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    uart_init(115200);
    LED_Init();
    KEY_Init();
    MYDMA_Config(DMA1_Channel4, (uint32_t)&USART1->DR, (uint32_t)SendBuff, SEND_BUF_SIZE);

    j = sizeof(TEXT_TO_SEND);
    for (i = 0; i < SEND_BUF_SIZE; i++)
    {
        if (t >= j)
        {
            if (mask)
            {
                SendBuff[i] = 0x0a;
                t = 0;
            }
            else
            {
                SendBuff[i] = 0x0d;
                mask++;
            }
        }
        else
        {
            mask = 0;
            SendBuff[i] = TEXT_TO_SEND[t];
            t++;
        }
    }
    i = 0;
    while (1)
    {
        t = KEY_Scan(0);
        if (t == KEY0_PRES)
        {
            char str[20];
            sprintf(str, "\r\nDMA DATA:\r\n");
            uart_SendString(str);
            USART_DMACmd(USART1, USART_DMAReq_Tx, ENABLE);

            MYDMA_Enable(DMA1_Channel4);

            while (1)
            {
                if (DMA_GetFlagStatus(DMA1_FLAG_TC4) != RESET)
                {
                    DMA_ClearFlag(DMA1_FLAG_TC4);
                    break;
                }
            }
        }
        i++;
        Delay_ms(10);
        if (i == 20)
        {
            if (LED0 == 1)
            {
                GPIO_ResetBits(GPIOB, GPIO_Pin_5);
            }
            else
            {
                GPIO_SetBits(GPIOB, GPIO_Pin_5);
            }
            i = 0;
        }
    }
}
