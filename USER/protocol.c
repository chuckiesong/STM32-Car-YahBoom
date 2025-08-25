#include "protocol.h"
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
#include <stdlib.h>
#include <string.h>

#define 	run_car     '1'//按键前
#define 	back_car    '2'//按键后
#define 	left_car    '3'//按键左
#define 	right_car   '4'//按键右
#define 	stop_car    '0'//按键停

u8 ProtocolString[80] = {0};
int CarSpeedControl = 500;

enum {

  enSTOP = 0,
  enRUN,
  enBACK,
  enLEFT,
  enRIGHT,
  enTLEFT,
  enTRIGHT

}enCarState;

int g_CarState = enSTOP; //1前2后3左4右0停

//pSrc:源字符串; pDst:查找的字符串; v_iStartPos:源字符串起始位置
int StringFind(const char *pSrc, const char *pDst, int v_iStartPos)  
{  
    int i, j;  
    for (i = v_iStartPos; pSrc[i]!='\0'; i++)  
    {  
        if(pSrc[i]!=pDst[0])  
            continue;         
        j = 0;  
        while(pDst[j] !='\0' && pSrc[i+j]!='\0')  
        {  
            j++;  
            if(pDst[j]!=pSrc[i+j])  
            break;  
        }  
        if(pDst[j]=='\0')  
            return i;  
    }  
    return -1;  
} 

//串口数据拷贝到新的buf
void ProtocolCpyData(void)
{
	memcpy(ProtocolString, inputString, g_packnum + 1);
	memset(inputString, 0x00, sizeof(inputString));
}

//协议处理函数
void Protocol(void)
{
	ProtocolCpyData();

	if(StringFind((const char *)ProtocolString, (const char *)"MODE", 0) > 0 
		&& StringFind((const char *)ProtocolString, (const char *)"4WD", 0) > 0)
	{
		
	    if (ProtocolString[10] == '0') //Í£Ö¹Ä£Ê½
	    {
	    	Car_Stop();
			g_CarState = enSTOP;
			g_modeSelect = 0;
			SPEAKER_FAST();
	    }
	    else
	    {
			switch (ProtocolString[9])
			{
				case '0': g_modeSelect = 0; ModeBEEP(0); NVIC_EnableIRQ(TIM1_UP_IRQn); break;
				case '1': g_modeSelect = 1; ModeBEEP(1); break;
				case '2': g_modeSelect = 2; ModeBEEP(2); NVIC_DisableIRQ(TIM1_UP_IRQn); break;
				case '3': g_modeSelect = 3; ModeBEEP(3); break;
				case '4': g_modeSelect = 4; ModeBEEP(4); break;
				case '5': g_modeSelect = 5; ModeBEEP(5); break;
				case '6': g_modeSelect = 6; ModeBEEP(6); break;
				default: g_modeSelect = 0; break;
			}
			SPEAKER_FAST();
	    }
	    newLineReceived = 0;  
		memset(ProtocolString, 0x00, sizeof(ProtocolString));
	    return;
  	}

//控制舵机角度
	if (StringFind((const char *)ProtocolString, (const char *)"PTZ", 0) > 0)
	{
		int m_kp, i, ii;
		int Angle_J1 = 90;

		i = StringFind((const char *)ProtocolString, (const char *)"PTZ", 0);
		ii = StringFind((const char *)ProtocolString, (const char *)"#", i);
		if (ii > i)
		{
			char m_skp[5] = {0};
			memcpy(m_skp, ProtocolString + i + 3, ii - i -3);
			
			m_kp = atoi(m_skp);

			Angle_J1 = (180 - m_kp);
			
			Servo_SetAngle(Angle_J1);

			newLineReceived = 0;  
			memset(ProtocolString, 0x00, sizeof(ProtocolString));
			return;
		}
  	}

//七彩探照灯
	else if (StringFind((const char *)ProtocolString, (const char *)"CLR", 0) > 0)
	{
		int m_kp, i, ii, red, green, blue;
		char m_skp[5] = {0};
		
		i = StringFind((const char *)ProtocolString, (const char *)"CLR", 0);
		ii = StringFind((const char *)ProtocolString, (const char *)",", i);
		if (ii > i)
		{			
			memcpy(m_skp, ProtocolString + i + 3, ii - i -3);
			m_kp = atoi(m_skp);
			red =   m_kp;
		}
		i = StringFind((const char *)ProtocolString, (const char *)"CLG", 0);
		ii = StringFind((const char *)ProtocolString, (const char *)",", i);
		if (ii > i)
		{
			memcpy(m_skp, ProtocolString + i + 3, ii - i -3);
			m_kp = atoi(m_skp);
			green =   m_kp;
		}
		i = StringFind((const char *)ProtocolString, (const char *)"CLB", 0);
		ii = StringFind((const char *)ProtocolString, (const char *)"#", i);
		if (ii > i)
		{
			memcpy(m_skp, ProtocolString + i + 3, ii - i -3);
			m_kp = atoi(m_skp);
			blue =  m_kp;
			Set_RGB_Brightness(red, green, blue);//
			newLineReceived = 0;  
			memset(ProtocolString, 0x00, sizeof(ProtocolString));
			return;
		}
	}

//执行动作
	if (StringFind((const char *)ProtocolString, (const char *)"4WD", 0) == -1)
	{
		//原地转弯
		if (ProtocolString[3] == '1')      //原地左转
		{
			g_CarState = enTLEFT;
		}
		else if (ProtocolString[3] == '2') //原地右转
		{
			g_CarState = enTRIGHT;
		}
		else
		{
			g_CarState = enSTOP;
		}

		//鸣笛判断
		if (ProtocolString[5] == '1')     //鸣笛
		{
			whistle();
		}

		//加减速
		if (ProtocolString[7] == '1')    //加速100
		{
			CarSpeedControl += 100;
			if (CarSpeedControl > 1000)
			{
				CarSpeedControl = 1000;
			}
		}
		if (ProtocolString[7] == '2')	//减速100
		{
			CarSpeedControl -= 100;
			if (CarSpeedControl < 200)
			{
				CarSpeedControl = 200;
			}
		}

		//舵机旋转方向
		if (ProtocolString[9] == '1')
		{
			Servo_SetAngle(180);
		}
		if (ProtocolString[9] == '2')
		{
			Servo_SetAngle(0);
		}

		
		//点灯
		if (ProtocolString[13] == '1')//白
		{
			Set_RGB_Brightness(255, 255, 255);
		}
		else if (ProtocolString[13] == '2')//红
		{
			Set_RGB_Brightness(255, 0, 0);
		}
		else if (ProtocolString[13] == '3')//绿
		{
			Set_RGB_Brightness(0, 255, 0);
		}
		else if (ProtocolString[13] == '4') //蓝
		{
			Set_RGB_Brightness(0, 0, 255);
		}
		else if (ProtocolString[13] == '5') //青
		{
			Set_RGB_Brightness(0, 255, 255);
		}
		else if (ProtocolString[13] == '6') //品红
		{
			Set_RGB_Brightness(255, 0, 255);
		}
		else if (ProtocolString[13] == '7') //黄
		{
			Set_RGB_Brightness(255, 255, 0);
		}
		else if (ProtocolString[13] == '8') //关灯
		{
			Set_RGB_Brightness(0, 0, 0);
		}

//风扇
		if (ProtocolString[15] == '1')
		{
			if(g_Boolfire == 0){
				FAN_ON();
				g_Boolfire = 1;			
			}
			else {
				FAN_OFF();
				g_Boolfire = 0;
			}
		}
		else if (ProtocolString[15] == '0')
		{
			FAN_OFF();
			g_Boolfire = 0;
		}


		//舵机归中
		if (ProtocolString[17] == '1') //90
		{
			Servo_SetAngle(90);
		}

		//前后左右停
		if (g_CarState != enTLEFT && g_CarState != enTRIGHT)
		{
			switch (ProtocolString[1])
			{
				case run_car:   g_CarState = enRUN;  break;
				case back_car:  g_CarState = enBACK;  break;
				case left_car:  g_CarState = enLEFT;  break;
				case right_car: g_CarState = enRIGHT;  break;
				case stop_car:  g_CarState = enSTOP;  break;
				default: g_CarState = enSTOP; break;
			}
		}

		newLineReceived = 0;  
		memset(ProtocolString, 0x00, sizeof(ProtocolString));  

		//做对应动作
		switch (g_CarState)
		{
			case enSTOP: Car_Stop(); break;
			case enRUN: Car_Forward(CarSpeedControl); break;
			case enLEFT: Car_Left_Turn(CarSpeedControl); break;
			case enRIGHT: Car_Right_Turn(CarSpeedControl); break;
			case enBACK: Car_Backward(CarSpeedControl); break;
			case enTLEFT: Car_Turn_Left_InPlace(CarSpeedControl * 0.4); break;
			case enTRIGHT: Car_Turn_Right_InPlace(CarSpeedControl * 0.4); break;
			default: Car_Stop(); break;
		}
	}
}
