#include "stm32f10x.h"  
#include "stm32f10x_usart.h"  
#include "esp8266.h"
#include "led.h"

volatile uint8_t Count=0;     
char Rx232buffer[Buf_Max];


void esp8266_init(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;

  RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOE, ENABLE); 
  

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;                
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
     

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;                
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOE, &GPIO_InitStructure);

  ESP8266_RST_H;
}

void USART3_Init(void)
{
	USART_InitTypeDef				USART_InitStructure;
	USART_ClockInitTypeDef  USART_ClockInitStructure;
	NVIC_InitTypeDef				NVIC_InitStructure;

  
  RCC_APB1PeriphClockCmd( RCC_APB1Periph_USART3 , ENABLE); 
	
  
	USART_DeInit(USART3);
	USART_InitStructure.USART_BaudRate = 115200;                      //设置串口3波特率为115200
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;      //设置一个帧中传输数据位 
	USART_InitStructure.USART_StopBits = USART_StopBits_1;           //定义发送的停止位数目为1
	USART_InitStructure.USART_Parity = USART_Parity_No;              //奇偶失能
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;     //硬件流控制失能
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;                     //发送接收使能
	USART_Init(USART3, &USART_InitStructure);    

  USART_ClockInitStructure.USART_Clock = USART_Clock_Disable;      //时钟低电平活动
  USART_ClockInitStructure.USART_CPOL = USART_CPOL_Low;            //设置SCLK引脚上时钟输出的极性为低电平
  USART_ClockInitStructure.USART_CPHA = USART_CPHA_1Edge;          //时钟第一个边沿进行数据捕获
  USART_ClockInitStructure.USART_LastBit = USART_LastBit_Disable;  //最后一位数据的时钟脉冲不从SCLK输出
  USART_ClockInit(USART3, &USART_ClockInitStructure); 
	
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);                   //使能USART3接收中断
	USART_Cmd(USART3, ENABLE);                                       //使能USART3外设
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);                  //NVIC_Group:先占优先级2位，从优先级2位
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;	               //配置为串口3中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2;          //先占优先级为2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;	             //从优先级为3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}


void USART3_SendByte(uint8_t byte)
{
	while(USART_GetFlagStatus(USART3,USART_FLAG_TC) != SET);
	USART_SendData(USART3, byte);
}

int fputc(int ch, FILE *f)
{
		USART_SendData(USART3, (uint8_t) ch);
		
		while (USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);		
	
		return (ch);
}

int fgetc(FILE *f)
{
		while (USART_GetFlagStatus(USART3, USART_FLAG_RXNE) == RESET);

		return (int)USART_ReceiveData(USART3);
}

bool Hand( char *a)
{ 
  if(strstr(Rx232buffer,a)!=NULL)
	   return true;
	else
		 return false;
}

void CLR_Buf(void)
{
	uint8_t k;
  for(k=0;k<Buf_Max;k++)      
  {
			Rx232buffer[k] = 0;
	}      
  Count = 0;
}

void USART3_IRQHandler(void)
{  
  if(USART_GetITStatus(USART3,USART_IT_RXNE)!= RESET)
  {  
    Rx232buffer[Count]= USART_ReceiveData(USART3);
		Count++;               
	  if(Count>Buf_Max)
		{
			 Count = 0;
		} 
    USART_ClearITPendingBit(USART3, USART_IT_RXNE);
  }  
}

