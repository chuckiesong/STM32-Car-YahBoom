#include "stm32f10x.h"
#include "delay.h"
#include "LEDBYPWM.h"

void RLED(void)
{
Set_RGB_Brightness(255,0,0);
}

void GLED(void)
{
Set_RGB_Brightness(0,255,0);
}

void BLED(void)
{
Set_RGB_Brightness(0,0,255);
}

void RLED_OFF(void)
{
Set_RGB_Brightness(0,0,0);
}

void GLED_OFF(void)
{
Set_RGB_Brightness(0,0,0);
}

void BLED_OFF(void)
{
Set_RGB_Brightness(0,0,0);
}

void R_FLASH_FAST(void)
{
Set_RGB_Brightness(255,0,0);
delay_ms(100);
Set_RGB_Brightness(0,0,0);
delay_ms(100);
}

void R_FLASH_MIDDLE(void)
{
Set_RGB_Brightness(255,0,0);
delay_ms(150);
Set_RGB_Brightness(0,0,0);
delay_ms(150);
}

void R_FLASH_SLOW(void)
{
Set_RGB_Brightness(255,0,0);
delay_ms(200);
Set_RGB_Brightness(0,0,0);
delay_ms(200);
}
