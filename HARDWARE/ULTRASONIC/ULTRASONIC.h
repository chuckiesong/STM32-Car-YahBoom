#include "stm32f10x.h"
#ifndef _ULTRASONIC_H_
#define _ULTRASONIC_H_

void USWAVE_Init(void);
void USWAVE_GPIO_Init(void);
void TIM2_Init(void);
float Ultrasonic_GetDistance(void);
void USWAVE(void);
void USWAVE_Avoidance(uint16_t speed);
void ULTRASONIC_SERVO_Avoidance(uint16_t UAspeed);
void Com_Avoidance(uint16_t UAspeed);

#endif
