#include "stm32f10x.h"
#ifndef _SERVO_H_
#define _SERVO_H_			   

void SERVO_TEST(void);
void Servo_Init(void);
void Servo_SetAngle(uint8_t angle);
void Servo_SetPulse(uint16_t pulse_us);
void Servo_SmoothMove(uint8_t start_angle, uint8_t end_angle, uint16_t steps, uint16_t delaytime_ms);
void color_servo(void);

#endif
