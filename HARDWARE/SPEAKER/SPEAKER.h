#include "stm32f10x.h"
#ifndef _SPEAKER_H_
#define _SPEAKER_H_			   

void SPEAKER_Init(void);
void SPEAKER_ON(void);
void SPEAKER_OFF(void);
void SPEAKER_SLOW(void);
void SPEAKER_MIDDLE(void);
void SPEAKER_FAST(void);
void ModeBEEP(int mode);
void whistle(void);
#endif
