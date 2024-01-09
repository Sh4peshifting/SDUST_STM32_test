#ifndef __UART_H
#define __UART_H

#include "stm32f10x.h"

void uart_init(uint32_t bound);
void uart_SendByte(uint8_t Byte);
void uart_SendArray(uint8_t *Array, uint16_t Length);
void uart_SendString(char *String);
void uart_SendNumber(uint32_t Number, uint8_t Length);

#endif