#include "stm32f10x.h"
#ifndef _LEDBYPWM_H_
#define _LEDBYPWM_H_			   

void TIM3_PWM_Init(void);
void Set_RGB_Brightness(uint8_t red, uint8_t green, uint8_t blue);
void breath_led(void);
void rainbow_led(void);
void colour_mode(void);

#endif
