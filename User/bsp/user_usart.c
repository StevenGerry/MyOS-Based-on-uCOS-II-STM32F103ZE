#include "stm32f10x.h"
#include "stm32f10x_usart.h"
#include "user_usart.h"

/**************************************************************************************
 * ��  �� : ��ʼ��USART1������USART1�ж����ȼ�
 * ��  �� : ��
 * ����ֵ : ��
 **************************************************************************************/
void USART1_Init(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;
	USART_InitTypeDef				USART_InitStructure;
	USART_ClockInitTypeDef  USART_ClockInitStructure;

  //������GPIO������1��ʱ��
  RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA | RCC_APB2Periph_USART1 , ENABLE); 
  
  //���õ�IO��PA9������1��TXD
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;                
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;           //�������
	GPIO_Init(GPIOA, &GPIO_InitStructure);
  //���õ�IO��PA10������1��RXD
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;     //��������
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
  //���ô���1��Ӳ������
	USART_DeInit(USART1);                                            //������USART1�Ĵ�������Ϊȱʡֵ
	USART_InitStructure.USART_BaudRate = 19200;                      //���ô���1������Ϊ19200
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;      //����һ��֡�д�������λ 
	USART_InitStructure.USART_StopBits = USART_StopBits_1;           //���巢�͵�ֹͣλ��ĿΪ1
	USART_InitStructure.USART_Parity = USART_Parity_No;              //��żʧ��
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;     //Ӳ��������ʧ��
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;                     //���ͽ���ʹ��
	USART_Init(USART1, &USART_InitStructure);    

  USART_ClockInitStructure.USART_Clock = USART_Clock_Disable;      //ʱ�ӵ͵�ƽ�
  USART_ClockInitStructure.USART_CPOL = USART_CPOL_Low;            //����SCLK������ʱ������ļ���Ϊ�͵�ƽ
  USART_ClockInitStructure.USART_CPHA = USART_CPHA_1Edge;          //ʱ�ӵ�һ�����ؽ������ݲ���
  USART_ClockInitStructure.USART_LastBit = USART_LastBit_Disable;  //���һλ���ݵ�ʱ�����岻��SCLK���
  USART_ClockInit(USART1, &USART_ClockInitStructure); 
	
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);                   //ʹ��USART1�����ж�
	USART_Cmd(USART1, ENABLE);                                       //ʹ��USART1����

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

