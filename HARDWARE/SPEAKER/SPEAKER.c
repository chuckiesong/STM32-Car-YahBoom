#include "stm32f10x.h"
#include "delay.h"

//蜂鸣器GPIO初始化
void SPEAKER_Init(void)
{
 GPIO_InitTypeDef GPIO_InitStructure;
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 GPIO_Init(GPIOA, &GPIO_InitStructure);
 GPIO_SetBits(GPIOA,GPIO_Pin_0);
}

//蜂鸣器开
void SPEAKER_ON(void)
{
GPIO_ResetBits(GPIOA,GPIO_Pin_0);
}

//蜂鸣器关
void SPEAKER_OFF(void)
{
GPIO_SetBits(GPIOA,GPIO_Pin_0);
}

//蜂鸣器慢响
void SPEAKER_SLOW(void)
{
SPEAKER_ON();
delay_ms(200);
SPEAKER_OFF();
delay_ms(200);
}

//蜂鸣器快响
void SPEAKER_FAST(void)
{
SPEAKER_ON();
delay_ms(100);
SPEAKER_OFF();
delay_ms(100);
}

//模式提示声
void ModeBEEP(int mode)
{
	int i;
	for (i = 0; i < mode + 1; i++)
	{
		SPEAKER_ON();
		delay_ms(100);
		SPEAKER_OFF();
		delay_ms(100);
	}
	delay_ms(100);
	SPEAKER_OFF();
}

//鸣笛一次
void whistle(void)
{
  SPEAKER_ON();
  delay_ms(100);
  SPEAKER_OFF();
  delay_ms(1);
  SPEAKER_ON();
  delay_ms(200);
  SPEAKER_OFF();
  delay_ms(2);
  return;
}
