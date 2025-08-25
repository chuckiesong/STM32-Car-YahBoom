#include "stm32f10x.h"
#ifndef _LED_H_
#define _LED_H_			   

void RLED(void);
void GLED(void);
void BLED(void);
void RLED_OFF(void);
void GLED_OFF(void);
void BLED_OFF(void);
void R_FLASH_FAST(void);
void R_FLASH_MIDDLE(void);
void R_FLASH_SLOW(void);
#endif
