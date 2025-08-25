#include "stm32f10x.h"
#ifndef _4LF_H_
#define _4LF_H_			   


void LF_GPIO_Init(void);
void HandleSensorState(uint8_t state);
void Line_Following_Control(void);
int Get_4FL(void);


#endif
