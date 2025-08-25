#include "stm32f10x.h"
#include "GRAY.h"

//fan_needed  1=需要风扇, 0=不需要
//gpio_mode  0=传感器模式, 1=风扇模式

void FAN_OFF(void)
{
fan_needed = 0;
if (gpio_mode != 0) {
PA1_Switch_To_Sensor();//变回传感器模式
GPIO_SetBits(GPIOA,GPIO_Pin_1);
}
else GPIO_SetBits(GPIOA,GPIO_Pin_1);
}

void FAN_ON(void)
{
fan_needed = 1;
if (gpio_mode != 1) {
PA1_Switch_To_Fan();//变成风扇模式
GPIO_ResetBits(GPIOA,GPIO_Pin_1);
}
else
GPIO_ResetBits(GPIOA,GPIO_Pin_1);
}


