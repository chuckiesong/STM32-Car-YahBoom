#include "stm32f10x.h"
#ifndef _MOTOR_H_
#define _MOTOR_H_			   

void Motor_Init(void);
void Set_Left_Speed(uint16_t speed);
void Set_Right_Speed(uint16_t speed);
void Car_Forward(uint16_t speed);
void Car_Backward(uint16_t speed);
void Car_Left_Turn(uint16_t speed);
void Car_Right_Turn(uint16_t speed);
void Car_Turn_Left_InPlace(uint16_t speed);
void Car_Turn_Right_InPlace(uint16_t speed);
void Car_Stop(void);
void MOTOR_TEST(void);
#endif
