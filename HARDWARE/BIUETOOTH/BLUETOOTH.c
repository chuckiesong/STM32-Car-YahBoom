#include "stm32f10x.h"
#include "usart.h"
#include "delay.h"
#include "SPEAKER.h"
#include "FAN.h"
#include "ULTRASONIC.h"
#include "LEDBYPWM.h"
#include "MOTOR.h"
#include "INFRARED.h"
#include "GRAY.h"
#include "SERVO.h"
#include "4LF.h"
#include "BLUETOOTH.h"
#include "PROTOCOL.h"
#include <stdlib.h>
#include <string.h>

int g_modeSelect = 0;//0：apk上位机 1：红外遥控 2：巡线 3：超声波 4：七彩探照 5：寻光 6：红外跟踪
u8 g_Boolfire = 0;

//发送传感器数据
void serial_data_postback(void)
{
//超声波 电压 灰度 巡线 红外避障 寻光 
	float csbLen=0.0f, power; 
	u16 gs, lf; 
	u8 hw, gm;
	//超声波
	csbLen = Ultrasonic_GetDistance();
	power = 125;
	gs = Read_Gray_Sensor();
	lf = Get_4FL();
	hw = Get_IR();
	gm = 00;
	printf("$4WD,CSB%.2f,PV%1.2f,GS%d,LF%04d,HW%02d,GM%02d#",csbLen, power, gs, lf, hw, gm);																				
	return;
}

//蓝牙处理
void bluetooth_deal(void)
{
	if (newLineReceived)
   	{
		Protocol();
	}

	//是否模式选择
	switch (g_modeSelect)
	{
		case 1: break; 													//保留
		case 2: Line_Following_Control(); break; 			//巡线
		case 3: ULTRASONIC_SERVO_Avoidance(300);break; 	//超声波避障
		case 4: colour_mode();break;  //颜色随舵机转动变化
		case 5: break; 	//寻光
		case 6: break;  //跟随模式
		default:break;
	}
}
