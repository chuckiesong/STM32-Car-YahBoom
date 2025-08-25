#include "stm32f10x.h"
#include "delay.h"
#include "core_cm3.h"
#include "LED.h"
#include "SPEAKER.h"
#include "MOTOR.h"
#include "SERVO.h"

#define ECHO_PIN GPIO_Pin_12

//超声波GPIO初始化
void USWAVE_GPIO_Init(void)
{
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
 GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
 GPIO_InitTypeDef GPIO_InitStructure;
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 GPIO_Init(GPIOA, &GPIO_InitStructure);
	
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
 GPIO_Init(GPIOA, &GPIO_InitStructure);
	
 GPIO_ResetBits(GPIOA,GPIO_Pin_15);
}

//超声波计时器TIM2初始化
void TIM2_Init(void)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
 
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

    TIM_TimeBaseStructure.TIM_Prescaler = 71;          // 72MHz/(71+1)=1MHz
    TIM_TimeBaseStructure.TIM_Period = 0xFFFF;         // 65535
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
    TIM_Cmd(TIM2, DISABLE);
    TIM2->CNT = 0;
}
//超声波测距（5次平均值）
float Ultrasonic_GetDistance(void)
{
    uint32_t total_pulse = 0;  // 累加5次脉冲宽度us
    const uint8_t MEAS_COUNT = 5;//平均次数
    uint8_t valid_count = 0;   // 有效测量次数计数器
    
    for (uint8_t i = 0; i < MEAS_COUNT; i++) {
        GPIO_SetBits(GPIOA, GPIO_Pin_15);
        delay_us(15);
        GPIO_ResetBits(GPIOA, GPIO_Pin_15);

        uint32_t timeout = 50000;  // 50ms超时（50,000 us）
        while (GPIO_ReadInputDataBit(GPIOA, ECHO_PIN) == RESET) {
            if (--timeout == 0) goto next_measurement;  // 超时跳过本次
            delay_us(1);
        }

        TIM2->CNT = 0;
        TIM_Cmd(TIM2, ENABLE);
        
        timeout = 50000;//50ms超时保护
        while (GPIO_ReadInputDataBit(GPIOA, ECHO_PIN) == SET) {
            if (--timeout == 0) {
                TIM_Cmd(TIM2, DISABLE);  // 超时需停止定时器
                goto next_measurement;
            }
            delay_us(1);
        }
        TIM_Cmd(TIM2, DISABLE);
        
        total_pulse += TIM2->CNT;
        valid_count++;        
    next_measurement:
        delay_ms(10);// 6. 10ms测量间隔
    }
    
    if (valid_count == 0) return 0.0f;  // 无有效测量
    
    float avg_pulse = (float)total_pulse / valid_count;  // 平均脉冲宽度(us)
    return avg_pulse * 0.017f;  // 距离 = (us * 0.034) / 2 = us * 0.017
}

//超声波初始化（GPIO/TIM2）
void USWAVE_Init(void)
{
USWAVE_GPIO_Init();
TIM2_Init();
}

//测距闪灯报警
void USWAVE(void)
{
uint16_t distance = Ultrasonic_GetDistance();
	if (distance <= 25) {
    if (distance <= 15) {
        if (distance <= 8) {
            SPEAKER_ON();
            R_FLASH_FAST();
        } else {
            SPEAKER_FAST();
            R_FLASH_MIDDLE();
        }
    } else {
        SPEAKER_SLOW();
        R_FLASH_SLOW();
    }
}else {
RLED_OFF();
SPEAKER_OFF();
}
}

//超声波避障
void USWAVE_Avoidance(uint16_t speed)
{
uint16_t distance = Ultrasonic_GetDistance();
	
if (distance <= 15)
{
Car_Backward(speed * 0.8);
delay_ms(300);
Car_Turn_Right_InPlace(speed * 0.8);
delay_ms(200);
}
else 
{
Car_Forward(speed);
}
}

//超声波避障（带舵机）
void ULTRASONIC_SERVO_Avoidance(uint16_t UAspeed)
{
	int lenth = 0;
	int LeftDistance = 0, RightDistance = 0;

	lenth = (u16)Ultrasonic_GetDistance();

    if(lenth <= 30)
    {
		Car_Stop();
		
		Servo_SetAngle(180);
		delay_ms(500);
		lenth = Ultrasonic_GetDistance();			
		LeftDistance = lenth;	  
	 
		Servo_SetAngle(0);
		delay_ms(500);
		lenth = Ultrasonic_GetDistance();					
		RightDistance = lenth;

		Servo_SetAngle(90);
		delay_ms(500);

		if((LeftDistance < 22 ) &&( RightDistance < 22 ))
		{
			Car_Turn_Right_InPlace(200);
			delay_ms(500);
		}
		else if(LeftDistance >= RightDistance)
		{      
			Car_Turn_Left_InPlace(200);
			delay_ms(500);
		}
		else
		{
			Car_Turn_Right_InPlace(200);
			delay_ms(500);
		}
    }
    else if(lenth > 30)
    {
		Car_Forward(UAspeed);   
    }
}

//红外超声波综合避障
void Com_Avoidance(uint16_t CAspeed)
{
  int lenth = 0;
	int LeftDistance = 0, RightDistance = 0;
  int LIR,RIR;
	
	lenth = (u16)Ultrasonic_GetDistance();
	LIR = !GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_6);//1=有障碍, 0=无障碍
	RIR = !GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_4);//1=有障碍, 0=无障碍
	
	if(lenth <= 30)
    {
		Car_Stop();
		
		Servo_SetAngle(180);
		delay_ms(500);
		lenth = Ultrasonic_GetDistance();			
		LeftDistance = lenth;	  
	 
		Servo_SetAngle(0);
		delay_ms(500);
		lenth = Ultrasonic_GetDistance();					
		RightDistance = lenth;

		Servo_SetAngle(90);
		delay_ms(500);

		if((LeftDistance < 22 ) &&( RightDistance < 22 ))
		{
			Car_Turn_Right_InPlace(200);
			delay_ms(500);
		}
		else if(LeftDistance >= RightDistance)
		{      
			Car_Turn_Left_InPlace(200);
			delay_ms(500);
		}
		else
		{
			Car_Turn_Right_InPlace(200);
			delay_ms(500);
		}
    }
    else if(lenth > 30)
    {		
			if (LIR && RIR) {
        Car_Backward(CAspeed);
        delay_ms(300);
        Car_Turn_Right_InPlace(200);
        delay_ms(200);
    }
    else if (LIR) {
        Car_Turn_Right_InPlace(200);
    }
    else if (RIR) {
        Car_Turn_Left_InPlace(200);
    }
    else {
        Car_Forward(CAspeed);
    }   
    }
	
}


