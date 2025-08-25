#include "sys.h"
#include "usart.h"

u8 newLineReceived = 0;
u8 inputString[80] = {0};  
u8 startBit = 0;
int g_num = 0;
int g_packnum = 0;

//printf
int fputc(int ch, FILE *f) {
    while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);
    USART_SendData(USART2, (uint8_t)ch);
    return ch;
}


void USART2_SendChar(uint8_t ch)
{
    while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);
    USART_SendData(USART2, ch);
}

void USART2_SendString(char *str)
{
    while (*str != '\0')
    {
        USART2_SendChar(*str++);
    }
}

void Uart2_init(u32 bound){
  	//GPIO端口设置
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	//使能USART1，GPIOA时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	 
	//USART2_TX   GPIOA.2
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; //PA2
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
	GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.9
	
	//USART2_RX	  GPIOA.3初始化
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;//PA3
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
	GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.10  
	
	//USART 初始化设置	
	USART_InitStructure.USART_BaudRate = bound;//串口波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
	USART_Init(USART2, &USART_InitStructure); //初始化串口2	
	USART_Cmd(USART2, ENABLE);                    //使能串口2

//Usart2 NVIC 配置
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0 ;//抢占优先级0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;		//子优先级1
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器	
//	USART_ClearFlag(USART2, USART_FLAG_ORE);
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//开启串口接受中断
}

void USART2_IRQHandler(void)
	{		
	u8 uartvalue = 0;

	if (USART_GetFlagStatus(USART2, USART_FLAG_ORE) != RESET)
    {  
		USART_ClearFlag(USART2, USART_FLAG_ORE);
    USART_ReceiveData(USART2); 
    }
		
	if(USART_GetFlagStatus(USART2, USART_FLAG_RXNE)!=RESET)
	{
		uartvalue = USART_ReceiveData(USART2);
		USART_ClearITPendingBit(USART2, USART_IT_RXNE);				


		if(uartvalue == '$')
	    {
	      	startBit = 1;
		    g_num = 0;
	    }
	    if(startBit == 1)
	    {
	       	inputString[g_num] = uartvalue;     
	    }  
	    if (startBit == 1 && uartvalue == '#') 
	    {
	    	
			newLineReceived = 1; 
			startBit = 0;
			g_packnum = g_num;	
		
	    }
		g_num++;
		if(g_num >= 80)
		{
			g_num = 0;
			startBit = 0;
			newLineReceived	= 0;
		}	 
	}
} 
	
