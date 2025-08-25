#include "stm32f10x.h"
#ifndef _INFRARED_H_
#define _INFRARED_H_			   

void IR_Sensor_Init(void) ;
void IR_Avoidance(uint16_t speed);
int Get_IR(void);

#endif
