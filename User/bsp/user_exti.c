#include "stm32f10x.h"
#include "stm32f10x_exti.h"
#include "user_exti.h"
#include "ov7670.h" 
#include "delay.h"

 
uint8_t frame_flag;                                     //֡�жϱ��   

 /**************************************************************************************
 * ��  �� : ��ʼ��OV7670ģ��VSYNC����Ϊ�ⲿ�ж�����
 * ��  �� : ��
 * ����ֵ : ��
 **************************************************************************************/
void OV7670_PA1_Init(void)
{		
	EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource1);   //����PA1�ܽ�Ϊ�ⲿ�ж���·��
	  
	EXTI_InitStructure.EXTI_Line = EXTI_Line1;                    //����Ϊ�ⲿ�ж���1
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;           //����Ϊ�ж�����
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;        //������·������Ϊ�ж�����
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;	                    //ʹ���ж�
  EXTI_Init(&EXTI_InitStructure);
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);               //NVIC_Group:��ռ���ȼ�2λ�������ȼ�2λ  
  NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn;              //����Ϊ�ⲿ�ж�1�ж�
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;     //��ռ���ȼ�Ϊ0
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;            //�����ȼ�Ϊ1
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;               //ʹ���ж�ͨ��
  NVIC_Init(&NVIC_InitStructure); 
}

/**************************************************************************************
 * ��  �� : �ⲿ�жϷ������
 * ��  �� : ��
 * ����ֵ : ��
 **************************************************************************************/
void EXTI1_IRQHandler(void)
{		 		
 
	if(EXTI_GetITStatus(EXTI_Line1)==SET)//��8�ߵ��ж�
	{     
		OV7670_WRST_L;	   //FIFOдָ�븴λ         
		OV7670_WRST_H;	
		OV7670_WEN_H;		   //FIFOдʹ�ܣ�������д��FIFO
		frame_flag++;      //֡�жϱ���ۼ�1 
	}
 
	EXTI_ClearITPendingBit(EXTI_Line1);  //���EXTI1��·����λ						  
} 
