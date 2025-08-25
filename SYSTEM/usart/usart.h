#ifndef __USART_H
#define __USART_H
#include "stdio.h"	
#include "sys.h" 

extern u8 newLineReceived;
extern int g_packnum;
extern u8 inputString[80];

void USART2_SendChar(uint8_t ch);
void USART2_SendString(char *str);

void Uart2_init(u32 bound);
void USART2_IRQHandler(void);

#endif


