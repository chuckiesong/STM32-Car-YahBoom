#include "stm32f10x.h"
#include "SERVO.h"
#include "delay.h"

// RGB亮度(0-255)
volatile uint16_t red_level   = 128;  // 红色亮度 (PB1)
volatile uint16_t green_level = 64;   // 绿色亮度 (PB0)
volatile uint16_t blue_level  = 32;   // 蓝色亮度 (PA7)

// PWM参数
#define PWM_PERIOD    255            // PWM周期值 (8位分辨率)
#define PWM_PRESCALER 71             // 预分频值 (72MHz/72=1MHz)


void TIM3_PWM_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
    TIM_OCInitTypeDef TIM_OCInitStruct;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB, ENABLE);
    
    // 蓝色通道: PA7 (TIM3_CH2)
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_7;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStruct);
    
    // 绿色通道: PB0 (TIM3_CH3)
    // 红色通道: PB1 (TIM3_CH4)
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
    GPIO_Init(GPIOB, &GPIO_InitStruct);
    GPIO_ResetBits(GPIOA,GPIO_Pin_7);
	  GPIO_ResetBits(GPIOB,GPIO_Pin_0);
	  GPIO_ResetBits(GPIOB,GPIO_Pin_1);

    TIM_TimeBaseInitStruct.TIM_Prescaler = PWM_PRESCALER;     // 1MHz计数频率
    TIM_TimeBaseInitStruct.TIM_Period = PWM_PERIOD;           // 255周期
    TIM_TimeBaseInitStruct.TIM_ClockDivision = 0;
    TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStruct);
    
    TIM_OCInitStruct.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStruct.TIM_OCPolarity = TIM_OCPolarity_High;
    
    // 蓝色通道: CH2 (PA7)
    TIM_OCInitStruct.TIM_Pulse = 0;
    TIM_OC2Init(TIM3, &TIM_OCInitStruct);
    TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable);
    
    // 绿色通道: CH3 (PB0)
    TIM_OCInitStruct.TIM_Pulse = 0;
    TIM_OC3Init(TIM3, &TIM_OCInitStruct);
    TIM_OC3PreloadConfig(TIM3, TIM_OCPreload_Enable);
    
    // 红色通道: CH4 (PB1)
    TIM_OCInitStruct.TIM_Pulse = 0;
    TIM_OC4Init(TIM3, &TIM_OCInitStruct);
    TIM_OC4PreloadConfig(TIM3, TIM_OCPreload_Enable);
    
    TIM_Cmd(TIM3, ENABLE);
    TIM_ARRPreloadConfig(TIM3, ENABLE);
}


void Set_RGB_Brightness(uint8_t red, uint8_t green, uint8_t blue)
{
 
    // CCR2 -> CH2 -> PA7 (蓝色)
    // CCR3 -> CH3 -> PB0 (绿色)
    // CCR4 -> CH4 -> PB1 (红色)
    TIM3->CCR2 = blue;    // 蓝色通道
    TIM3->CCR3 = green;   // 绿色通道
    TIM3->CCR4 = red;     // 红色通道
}


//呼吸灯
void breath_led(void)
{
for(uint16_t i = 0; i <= 255; i++) {
            Set_RGB_Brightness(i, 0, 0);  // 红色渐变
            for(volatile uint32_t d = 0; d < 20000; d++);
        }
        for(uint16_t i = 255; i > 0; i--) {
            Set_RGB_Brightness(i, 0, 0);
            for(volatile uint32_t d = 0; d < 20000; d++);
        }
}
//彩虹灯
void rainbow_led(void)
{
for(uint16_t i = 0; i < 256; i++) {
            // 红色->黄色
            Set_RGB_Brightness(255, i, 0);
            for(volatile uint32_t d = 0; d < 500000; d++);
            
            // 黄色->绿色
            Set_RGB_Brightness(255-i, 255, 0);
            for(volatile uint32_t d = 0; d < 500000; d++);
            
            // 绿色->青色
            Set_RGB_Brightness(0, 255, i);
            for(volatile uint32_t d = 0; d < 500000; d++);
            
            // 青色->蓝色
            Set_RGB_Brightness(0, 255-i, 255);
            for(volatile uint32_t d = 0; d < 500000; d++);
            
            // 蓝色->品红
            Set_RGB_Brightness(i, 0, 255);
            for(volatile uint32_t d = 0; d < 500000; d++);
            
            // 品红->红色
            Set_RGB_Brightness(255, 0, 255-i);
            for(volatile uint32_t d = 0; d < 500000; d++);
        }
}


//七彩探照灯
int angle = 90;

static const int step = 5;
static int direction = 1;// 方向：1 增加，-1 减少
   
void colour_mode(void)
{
	
	angle += step * direction;

    if (angle >= 180) {
        angle = 180;
        direction = -1;
    } else if (angle <= 0) {
        angle = 0;
        direction = 1;
    }
	
	Servo_SetAngle(angle);
	
	if(angle <= 10)
	{
		Set_RGB_Brightness(255,0,0);
	}
	else if(angle <= 20)
	{
		Set_RGB_Brightness(0,255,0);	
	}
	else if(angle <= 30)
	{
		Set_RGB_Brightness(0,0,255);	
	}
	else if(angle <= 40)
	{
		Set_RGB_Brightness(255,255,0);	
	}
	else if(angle <= 50)
	{
		Set_RGB_Brightness(0,255,255);	
	}
	else if(angle <= 60)
	{
		Set_RGB_Brightness(255,0,255);	
	}
	else if(angle <= 70)
	{
		Set_RGB_Brightness(255,255,255);	
	}
	else if(angle <= 80)
	{
		Set_RGB_Brightness(125,255,0);	
	}
 	else if(angle <= 90)
	{
		Set_RGB_Brightness(125,255,0);	
	}
	else if(angle <= 100)
	{
		Set_RGB_Brightness(15,255,0);	
	}
	else if(angle <= 110)
	{
		Set_RGB_Brightness(58,255,58);	
	}
	else if(angle <= 120)
	{
		Set_RGB_Brightness(125,12,255);	
	}
	else if(angle <= 130)
	{
		Set_RGB_Brightness(10,255,0);	
	}
	else if(angle <= 140)
	{
		Set_RGB_Brightness(255,12,125);	
	}
	else if(angle <= 150)
	{
		Set_RGB_Brightness(125,125,255);	
	}
	else if(angle <= 160)
	{
		Set_RGB_Brightness(190,15,210);	
	}
		
	delay_ms(100);	
}
