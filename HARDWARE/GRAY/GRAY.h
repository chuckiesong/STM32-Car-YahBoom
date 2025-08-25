#include "stm32f10x.h"
#ifndef _GRAY_H_
#define _GRAY_H_	

extern volatile uint8_t fan_needed;// 1=需要风扇, 0=不需要
extern volatile uint8_t gpio_mode;// 0=传感器模式, 1=风扇模式

void GRAY_ADC_Init(void);
void GRAY_FAN_SWITCH(void);
void PA1_Init_As_Sensor(void);
void PA1_Switch_To_Fan(void);
void PA1_Switch_To_Sensor(void);
uint16_t Read_Gray_Sensor(void);

#endif
