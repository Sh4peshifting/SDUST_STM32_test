#include "delay.h"
#include "stm32f10x.h"

void Delay(uint32_t nCount)
{
		int x;
    for (; nCount != 0; nCount--)
        for(x = 114; x > 0 ; x--);
}
