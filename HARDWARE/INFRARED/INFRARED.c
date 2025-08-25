#include "stm32f10x.h"
#include "delay.h"
#include "MOTOR.h"

#define LEFT_IR_STATE  (!GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_6))  // PA6: 1=有障碍, 0=无障碍
#define RIGHT_IR_STATE (!GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_4))  // PA4: 1=有障碍, 0=无障碍

//红外GPIO初始化
void IR_Sensor_Init(void) 
{
    GPIO_InitTypeDef GPIO_InitStruct;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_4;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStruct);
    
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6;
    GPIO_Init(GPIOA, &GPIO_InitStruct);
}

//红外避障
void IR_Avoidance(uint16_t speed) {
    //0=无障碍, 1=有障碍
    uint8_t left_obstacle = LEFT_IR_STATE;
    uint8_t right_obstacle = RIGHT_IR_STATE;

    if (left_obstacle && right_obstacle) {
        Car_Backward(speed);
        delay_ms(300);
        Car_Turn_Right_InPlace(speed);
        delay_ms(200);
    }
    else if (left_obstacle) {
        Car_Turn_Right_InPlace(speed * 0.8);
    }
    else if (right_obstacle) {
        Car_Turn_Left_InPlace(speed * 0.8);
    }
    else {
        Car_Forward(speed);
    }
    
    delay_ms(10);
}

//获取红外传感器参数
int Get_IR(void)
{
	int IRL1,IRR1,ReturnValue;
	IRL1 = LEFT_IR_STATE;
	IRR1 = RIGHT_IR_STATE;
	ReturnValue = (IRL1 == 0 ? 10 : 0);
	ReturnValue += (IRR1 == 0 ? 1 : 0);
	return ReturnValue;
}
