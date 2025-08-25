#include "stm32f10x.h"
#include "time.h"
#include "delay.h"
#include "MOTOR.h"

#define NORMAL_SPEED      500   // 正常直行速度
#define SMALL_CORRECTION  500   // 小幅度修正速度
#define MEDIUM_CORRECTION 550		// 中幅度修正速度
#define LARGE_CORRECTION  400  	// 大角度修正速度
#define RT_ANGLE					300   // 直角速度

#define READ_LEFT1()  GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_14)  // PC14 - 最左
#define READ_LEFT2()  GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_13)  // PC13 - 左中
#define READ_RIGHT1() GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_15)  // PC15 - 右中
#define READ_RIGHT2() GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_12)  // PB12 - 最右

uint8_t Area = 1;
uint8_t Last_Area;

//寻线传感器GPIO初始化
void LF_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC, ENABLE);
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
}

//巡线控制
void Line_Following_Control(void)
{
		  
		//0=检测到黑线, 1=未检测到
    uint8_t l1  = READ_LEFT1();  // PC14
    uint8_t l2  = READ_LEFT2();  // PC13
    uint8_t r1  = READ_RIGHT1(); // PC15
    uint8_t r2  = READ_RIGHT2(); // PB12

    // 状态1: (1,0,0,1) - 直行
    if (l1 == 1 && l2 == 0 && r1 == 0 && r2 == 1) {
        Car_Forward(NORMAL_SPEED);
        Area = 1;
					if(Last_Area == 4)//左直角
						{ 
						 Car_Turn_Left_InPlace(RT_ANGLE);
						}
					else if(Last_Area == 5)//右直角
  					{
						 Car_Turn_Right_InPlace(RT_ANGLE);
						}
					else
						{
						 Car_Forward(NORMAL_SPEED);
						 Area = 1;				
						}
    }
    // 状态2: (1,0,1,1) - 小左转
    else if (l1 == 1 && l2 == 0 && r1 == 1 && r2 == 1) {
        Car_Left_Turn(SMALL_CORRECTION);
				Area = 2;
    }
    // 状态3: (1,1,0,1) - 小右转
    else if (l1 == 1 && l2 == 1 && r1 == 0 && r2 == 1) {
        Car_Right_Turn(SMALL_CORRECTION);
				Area = 3;
    }
    // 状态4: (0,0,0,1) - 左直角弯
    else if (l1 == 0 && l2 == 0 && r1 == 0 && r2 == 1) {
        Car_Turn_Left_InPlace(RT_ANGLE);
				Area = 4;
    }
    // 状态5: (1,0,0,0) - 右直角弯
    else if (l1 == 1 && l2 == 0 && r1 == 0 && r2 == 0) {
        Car_Turn_Right_InPlace(RT_ANGLE);
				Area = 5;
    }
    // 状态6: 左锐角转弯 (0,1,0,1) 或 (0,0,1,1) 或 (0,0,1,0)
    else if ((l1 == 0 && l2 == 1 && r1 == 0 && r2 == 1) || 
             (l1 == 0 && l2 == 0 && r1 == 1 && r2 == 1) ||
						 (l1 == 0 && l2 == 0 && r1 == 1 && r2 == 0)) {
        Car_Turn_Left_InPlace(LARGE_CORRECTION);
				Area = 6;
    }
    // 状态7: 右锐角转弯 (1,1,0,0) 或 (1,0,1,0) 或 (0,1,0,0)
    else if ((l1 == 1 && l2 == 1 && r1 == 0 && r2 == 0) || 
             (l1 == 1 && l2 == 0 && r1 == 1 && r2 == 0) ||
						 (l1 == 0 && l2 == 1 && r1 == 0 && r2 == 0)) {
        Car_Turn_Right_InPlace(LARGE_CORRECTION);
				Area = 7;
    }
    // 状态8:左大弯
    else if ((l1 == 0 && l2 == 1 && r1 == 1 && r2 == 1)) {  // 仅左1在线
        Car_Left_Turn(MEDIUM_CORRECTION);
				Area = 8;
    }
    // 状态9:右大弯
		else if (l1 == 1 && l2 == 1 && r1 == 1 && r2 == 0)// 仅右2在线 
		{ 
        Car_Right_Turn(MEDIUM_CORRECTION);
				Area = 9;
		}

		else if ((l1 == 1 && l2 == 1 && r1 == 1 && r2 == 1) ||
						 (l1 == 0 && l2 == 0 && r1 == 0 && r2 == 0) ||
						 (l1 == 0 && l2 == 1 && r1 == 1 && r2 == 0)) {  //全1 全0 
						if(Last_Area == 2 || Last_Area == 6 || Last_Area == 8)//左转
						{ 
						Car_Turn_Left_InPlace(LARGE_CORRECTION);
						}
						else if(Last_Area == 3 || Last_Area == 7 || Last_Area == 9)//右转
  					{
						Car_Turn_Right_InPlace(LARGE_CORRECTION);
						}
						else if(Last_Area == 4){
						Car_Turn_Left_InPlace(RT_ANGLE);//左直角
						}
						else if(Last_Area == 5){
						Car_Turn_Right_InPlace(RT_ANGLE);//右直角
						}
						else if(Last_Area == 1){
						Car_Forward(NORMAL_SPEED);//直行
						}

        
    else {
						if(Last_Area == 2 || Last_Area == 6 || Last_Area == 8)//左转
						{ 
						 Car_Left_Turn(LARGE_CORRECTION);
						}
						else if(Last_Area == 3 || Last_Area == 7 || Last_Area == 9)//右转
  					{
						 Car_Right_Turn(LARGE_CORRECTION);
						}
						else if(Last_Area == 4){
						Car_Turn_Left_InPlace(RT_ANGLE);//左直角
						}
						else if(Last_Area == 5){
						Car_Turn_Right_InPlace(RT_ANGLE);//右直角
						}
            else
						{
						Car_Forward(NORMAL_SPEED);
						}			
        }
        
			Last_Area = Area;
}
}

//获取巡线传感器参数
int Get_4FL(void)
{
//0=检测到黑线, 1=未检测到
	int L1, L2, R1, R2, returnValue;

  L1  = READ_LEFT1();  // PC14
  L2  = READ_LEFT2();  // PC13
  R1  = READ_RIGHT1(); // PC15
  R2  = READ_RIGHT2(); // PB12

	returnValue = (L1 == 1?0:1000);
	returnValue += (L2 == 1?0:100);
	returnValue += (R1 == 1?0:10);
	returnValue += (R2 == 1?0:1);
	return returnValue;
}
