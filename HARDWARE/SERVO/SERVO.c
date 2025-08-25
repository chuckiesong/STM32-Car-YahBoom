#include "stm32f10x.h"
#include "delay.h"

#define SERVO_MIN_ANGLE 0			//最小角度
#define SERVO_MAX_ANGLE 180		//中间角度
#define SERVO_MID_ANGLE 90		//最大角度

#define PWM_PERIOD 20000      // 20ms周期 (1MHz时钟下)
#define PWM_MIN_PULSE 500     // 0.5ms = 0° 500
#define PWM_MID_PULSE 1500    // 1.5ms = 90° 1500
#define PWM_MAX_PULSE 2500    // 2.5ms = 180° 2500

void Servo_Init(void);
void Servo_SetAngle(uint8_t angle);
void Servo_SetPulse(uint16_t pulse_us);

void SERVO_TEST(void) 
{
Servo_SetPulse(500);  // 最左位置（应为0°）		调整PWM_MIN_PULSE
delay_ms(1000);
Servo_SetPulse(1500); // 中位（应为90°）			调整PWM_MID_PULSE
delay_ms(1000);
Servo_SetPulse(2500); // 最右位置（应为180°）	调整PWM_MAX_PULSE
delay_ms(1000);
}

//设置舵机角度（0-180°）
void Servo_SetAngle(uint8_t angle) {
    uint16_t pulse;

    if (angle > SERVO_MAX_ANGLE) {
        angle = SERVO_MAX_ANGLE;
    }
    // 脉冲宽度 = 500 + (angle * 2000 / 180)
    pulse = PWM_MIN_PULSE + (uint32_t)angle * (PWM_MAX_PULSE - PWM_MIN_PULSE) / SERVO_MAX_ANGLE;

    TIM_SetCompare4(TIM1, pulse);
}

//设置PWM脉冲宽度（微秒）
void Servo_SetPulse(uint16_t pulse_us) {
    // 限制脉冲范围
    if (pulse_us < PWM_MIN_PULSE) {
        pulse_us = PWM_MIN_PULSE;
    } else if (pulse_us > PWM_MAX_PULSE) {
        pulse_us = PWM_MAX_PULSE;
    }

    TIM_SetCompare4(TIM1, pulse_us);
}

//平滑转动
void Servo_SmoothMove(uint8_t start_angle, uint8_t end_angle, uint16_t steps, uint16_t delaytime_ms) 
{
    int16_t step_size = (end_angle - start_angle) / (int16_t)steps;
    for (uint16_t i = 0; i < steps; i++) {
        Servo_SetAngle(start_angle + i * step_size);
        delay_ms(delaytime_ms);
    }
    Servo_SetAngle(end_angle);
}
//Servo_SmoothMove(0, 180, 36,50）// 从0°平滑转到180°，分36步，每步50ms

//调试
//Servo_SetAngle(45);// 转到45°位置
//Servo_SetAngle(135);// 转到135°位置
//Servo_SetPulse(1200);// 直接设置1.2ms脉冲（约36°）
//Servo_SetPulse(2000);// 直接设置2.0ms脉冲（约126°）
//Servo_SetAngle(90);//归中
//Servo_SetPulse(1500);//归中

//校准
//Servo_SetPulse(500);  // 最左位置（应为0°）		调整PWM_MIN_PULSE
//Servo_SetPulse(1500); // 中位（应为90°）			调整PWM_MID_PULSE
//Servo_SetPulse(2500); // 最右位置（应为180°）	调整PWM_MAX_PULSE

