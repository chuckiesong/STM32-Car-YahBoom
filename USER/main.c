#include "stm32f10x.h"
#include "time.h"
#include "delay.h"
#include "LED.h"
#include "SPEAKER.h"
#include "FAN.h"
#include "ULTRASONIC.h"
#include "LEDBYPWM.h"
#include "MOTOR.h"
#include "INFRARED.h"
#include "GRAY.h"
#include "SERVO.h"
#include "4LF.h"
#include "usart.h"
#include "BLUETOOTH.h"
#include "stdio.h"
#include <stdlib.h>
#include <string.h>

int main(void)
{
 SystemInit();
 NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
 delay_init();
 Uart2_init(9600);//蓝牙IO
 SPEAKER_Init();//蜂鸣器
 USWAVE_GPIO_Init();//超声波
 TIM2_Init();//超声波计时
 TIM3_PWM_Init();//led pwm
 Motor_Init();//电机
 IR_Sensor_Init();//红外避障
 GRAY_ADC_Init();//灰度传感器
 PA1_Init_As_Sensor();//PA1（灰度/风扇）
 LF_GPIO_Init();//巡线
 serial_data_postback();//传感器数据
 TIM1_Init();//舵机/定时中断

while(1)
{
bluetooth_deal();
delay_us(60);
} 
}
