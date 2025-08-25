#include "stm32f10x.h"
#include "delay.h"

#define AIN1_PIN    GPIO_Pin_8
#define AIN2_PIN    GPIO_Pin_9
#define BIN1_PIN    GPIO_Pin_5
#define BIN2_PIN    GPIO_Pin_4
#define PWMA_PIN    GPIO_Pin_7
#define PWMB_PIN    GPIO_Pin_6

#define AIN1_PORT   GPIOB
#define AIN2_PORT   GPIOB
#define BIN1_PORT   GPIOB
#define BIN2_PORT   GPIOB

#define LEFT_MOTOR_FORWARD  {GPIO_SetBits(AIN2_PORT, AIN2_PIN); GPIO_ResetBits(AIN1_PORT, AIN1_PIN);}
#define LEFT_MOTOR_BACKWARD {GPIO_ResetBits(AIN2_PORT, AIN2_PIN); GPIO_SetBits(AIN1_PORT, AIN1_PIN);}
#define LEFT_MOTOR_BRAKE    {GPIO_SetBits(AIN2_PORT, AIN2_PIN); GPIO_SetBits(AIN1_PORT, AIN1_PIN);}
#define LEFT_MOTOR_COAST    {GPIO_ResetBits(AIN2_PORT, AIN2_PIN); GPIO_ResetBits(AIN1_PORT, AIN1_PIN);}

#define RIGHT_MOTOR_FORWARD  {GPIO_SetBits(BIN2_PORT, BIN2_PIN); GPIO_ResetBits(BIN1_PORT, BIN1_PIN);}
#define RIGHT_MOTOR_BACKWARD {GPIO_ResetBits(BIN2_PORT, BIN2_PIN); GPIO_SetBits(BIN1_PORT, BIN1_PIN);}
#define RIGHT_MOTOR_BRAKE    {GPIO_SetBits(BIN2_PORT, BIN2_PIN); GPIO_SetBits(BIN1_PORT, BIN1_PIN);}
#define RIGHT_MOTOR_COAST    {GPIO_ResetBits(BIN2_PORT, BIN2_PIN); GPIO_ResetBits(BIN1_PORT, BIN1_PIN);}

#define PWM_PERIOD  999  // 1000-1 (0.1ms周期)
#define PWM_PRESCALER 71 // 72MHz/(72) = 1MHz计数频率

// 电机GPIO/TIM4初始化
void Motor_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_OCInitTypeDef TIM_OCInitStructure;
    
	  RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
	  GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);

    GPIO_InitStructure.GPIO_Pin = AIN1_PIN | AIN2_PIN | BIN1_PIN | BIN2_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin = PWMA_PIN | PWMB_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    
    TIM_TimeBaseStructure.TIM_Period = PWM_PERIOD;
    TIM_TimeBaseStructure.TIM_Prescaler = PWM_PRESCALER;
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);
    
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse = 0;  // 初始占空比0
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
    
    TIM_OC1Init(TIM4, &TIM_OCInitStructure);  // CH1 (右电机)
    TIM_OC2Init(TIM4, &TIM_OCInitStructure);  // CH2 (左电机)
    
    TIM_Cmd(TIM4, ENABLE);
    TIM_OC1PreloadConfig(TIM4, TIM_OCPreload_Enable);
    TIM_OC2PreloadConfig(TIM4, TIM_OCPreload_Enable);
}

// 设置左电机速度 (0-1000)
void Set_Left_Speed(uint16_t speed)
{
    if(speed > PWM_PERIOD) speed = PWM_PERIOD;
    TIM_SetCompare2(TIM4, speed);  // TIM4_CH2 (PB7)
}

// 设置右电机速度 (0-1000)
void Set_Right_Speed(uint16_t speed)
{
    if(speed > PWM_PERIOD) speed = PWM_PERIOD;
    TIM_SetCompare1(TIM4, speed);  // TIM4_CH1 (PB6)
}

//前进
void Car_Forward(uint16_t speed)
{
    LEFT_MOTOR_FORWARD;
    RIGHT_MOTOR_FORWARD;
    Set_Left_Speed(speed);
    Set_Right_Speed(speed);
}

//后退
void Car_Backward(uint16_t speed)
{
    LEFT_MOTOR_BACKWARD;
    RIGHT_MOTOR_BACKWARD;
    Set_Left_Speed(speed);
    Set_Right_Speed(speed);
}

// 普通左转
void Car_Left_Turn(uint16_t speed)
{
    LEFT_MOTOR_FORWARD;
    RIGHT_MOTOR_FORWARD;
    Set_Left_Speed(speed * 0.3);
    Set_Right_Speed(speed);
}

// 普通右转
void Car_Right_Turn(uint16_t speed)
{
    LEFT_MOTOR_FORWARD;
    RIGHT_MOTOR_FORWARD;
    Set_Left_Speed(speed);
    Set_Right_Speed(speed * 0.3);
}

// 原地左转
void Car_Turn_Left_InPlace(uint16_t speed)
{
    LEFT_MOTOR_BACKWARD;
    RIGHT_MOTOR_FORWARD;
    Set_Left_Speed(speed);
    Set_Right_Speed(speed);
}

// 原地右转
void Car_Turn_Right_InPlace(uint16_t speed)
{
    LEFT_MOTOR_FORWARD;
    RIGHT_MOTOR_BACKWARD;
    Set_Left_Speed(speed);
    Set_Right_Speed(speed);
}

//停车
void Car_Stop(void)
{
    LEFT_MOTOR_BRAKE;
    RIGHT_MOTOR_BRAKE;
    Set_Left_Speed(0);
    Set_Right_Speed(0);
}

//电机测试
void MOTOR_TEST(void)
{
Car_Forward(300);			//前进
delay_ms(3000);        
Car_Backward(300);    // 后退
delay_ms(3000);        
Car_Left_Turn(300);   // 普通左转
delay_ms(3000);        
Car_Right_Turn(300);  // 普通右转
delay_ms(3000);        
Car_Turn_Left_InPlace(300);  // 原地左转
delay_ms(3000);        
Car_Turn_Right_InPlace(300); // 原地右转
delay_ms(3000);      
Car_Stop();           // 停止
delay_ms(3000);
}
