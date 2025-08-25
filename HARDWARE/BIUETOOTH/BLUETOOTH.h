#include "stm32f10x.h"
#ifndef _BLUETOOTH_H_
#define _BLUETOOTH_H_			   

extern int g_modeSelect;
extern u8 g_Boolfire;

void bluetooth_deal(void);
void serial_data_postback(void);

#endif
