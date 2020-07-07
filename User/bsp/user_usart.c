#include "stm32f10x.h"
#include "stm32f10x_usart.h"
#include "user_usart.h"

/**************************************************************************************
 * 描  述 : 初始化USART1并配置USART1中断优先级
 * 入  参 : 无
 * 返回值 : 无
 **************************************************************************************/
void USART1_Init(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;
	USART_InitTypeDef				USART_InitStructure;
	USART_ClockInitTypeDef  USART_ClockInitStructure;

  //打开所用GPIO及串口1的时钟
  RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA | RCC_APB2Periph_USART1 , ENABLE); 
  
  //配置的IO是PA9，串口1的TXD
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;                
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;           //推挽输出
	GPIO_Init(GPIOA, &GPIO_InitStructure);
  //配置的IO是PA10，串口1的RXD
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;     //浮空输入
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
  //配置串口1的硬件参数
	USART_DeInit(USART1);                                            //将外设USART1寄存器重设为缺省值
	USART_InitStructure.USART_BaudRate = 19200;                      //设置串口1波特率为19200
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;      //设置一个帧中传输数据位 
	USART_InitStructure.USART_StopBits = USART_StopBits_1;           //定义发送的停止位数目为1
	USART_InitStructure.USART_Parity = USART_Parity_No;              //奇偶失能
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;     //硬件流控制失能
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;                     //发送接收使能
	USART_Init(USART1, &USART_InitStructure);    

  USART_ClockInitStructure.USART_Clock = USART_Clock_Disable;      //时钟低电平活动
  USART_ClockInitStructure.USART_CPOL = USART_CPOL_Low;            //设置SCLK引脚上时钟输出的极性为低电平
  USART_ClockInitStructure.USART_CPHA = USART_CPHA_1Edge;          //时钟第一个边沿进行数据捕获
  USART_ClockInitStructure.USART_LastBit = USART_LastBit_Disable;  //最后一位数据的时钟脉冲不从SCLK输出
  USART_ClockInit(USART1, &USART_ClockInitStructure); 
	
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);                   //使能USART1接收中断
	USART_Cmd(USART1, ENABLE);                                       //使能USART1外设

}

int fputc_usart1(int ch, FILE *f)
{
		USART_SendData(USART1, (uint8_t) ch);
		while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);		
		return (ch);
}

int fgetc_usart1(FILE *f)
{
		while (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET);
		return (int)USART_ReceiveData(USART1);
}

void USART1_SendByte(uint8_t byte)
{
	while(USART_GetFlagStatus(USART1,USART_FLAG_TC) != SET);
	USART_SendData(USART1, byte);
}


void USART1_SendStr(uint8_t *str)
{
  while(0!=*str)
	{
    USART1_SendByte(*str);
    str++;
  }
}


void USART1_IRQHandler(void)
{  
  if(USART_GetITStatus(USART1,USART_IT_RXNE)!= RESET)
  {  
    USART_ClearITPendingBit(USART1, USART_IT_RXNE);
  }  
}

