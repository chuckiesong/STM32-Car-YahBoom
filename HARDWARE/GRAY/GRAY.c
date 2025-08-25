#include "stm32f10x.h"
#include "delay.h"
#include "FAN.h"

volatile uint8_t fan_needed = 0;      // 1=需要风扇, 0=不需要
volatile uint8_t gpio_mode = 0;				// 0=传感器模式, 1=风扇模式

void PA1_Init_As_Sensor(void);
void PA1_Switch_To_Fan(void);
void PA1_Switch_To_Sensor(void);
uint16_t Read_Gray_Sensor(void);

//ADC初始化
void GRAY_ADC_Init(void)
{
ADC_InitTypeDef ADC_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
    
    ADC_DeInit(ADC1);
    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
    ADC_InitStructure.ADC_ScanConvMode = DISABLE;
    ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_InitStructure.ADC_NbrOfChannel = 1;
    ADC_Init(ADC1, &ADC_InitStructure);
    
    ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 1, ADC_SampleTime_239Cycles5);
    ADC_Cmd(ADC1, ENABLE);
    
    // ADC校准
    ADC_ResetCalibration(ADC1);
    while(ADC_GetResetCalibrationStatus(ADC1));
    ADC_StartCalibration(ADC1);
    while(ADC_GetCalibrationStatus(ADC1));
}


// PA1_Init_GRAY
void PA1_Init_As_Sensor(void) {
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;  // 模拟输入
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    gpio_mode = 0;
		fan_needed = 0;
}

// PA1>>FAN
void PA1_Switch_To_Fan(void) {
    
    GPIO_SetBits(GPIOA, GPIO_Pin_1);//关闭风扇
    
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  // 推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);    
    gpio_mode = 1;
}

// PA1>>GRAY
void PA1_Switch_To_Sensor(void) {
    
    GPIO_SetBits(GPIOA, GPIO_Pin_1);//关闭风扇

    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOA, &GPIO_InitStructure);   
    gpio_mode = 0;
}

//读取灰度传感器
uint16_t Read_Gray_Sensor(void) 
{
		if (!fan_needed){
		ADC_SoftwareStartConvCmd(ADC1, ENABLE);
    while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC));
    return ADC_GetConversionValue(ADC1);
		}
		else {return 200;}
}

