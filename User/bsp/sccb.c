#include "stm32f10x.h"
#include "sccb.h"
#include "delay.h"

/**************************************************************************************
 * ��  �� : OV7670оƬSCCB�ӿڶ�ӦIO�ڳ�ʼ��
 * ��  �� : ��
 * ����ֵ : ��
 **************************************************************************************/
void SCCB_GPIO_Init(void)
{											   	 
 	GPIO_InitTypeDef  GPIO_InitStructure;
	
 	//ʹ��OV7670оƬSCCB�ӿ����Ŷ�ӦIO�˿�ʱ��	
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOG, ENABLE);	 
	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;				     //����SDA��Ӧ����IO���
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //����SDA��Ӧ����IO����״̬Ϊ�������
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //����SDA��Ӧ����IO�����ٶ�Ϊ50MHZ
  GPIO_Init(GPIOG, &GPIO_InitStructure);					     //��ʼ��SDA��Ӧ����IO 

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;				     //����SCL��Ӧ����IO���
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //����SCL��Ӧ����IO����״̬Ϊ�������
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //����SCL��Ӧ����IO�����ٶ�Ϊ50MHZ
  GPIO_Init(GPIOG, &GPIO_InitStructure);					     //��ʼ��SCL��Ӧ����IO			
}			 

/**************************************************************************************
 * ��  �� : ����OV7670оƬSCCB�ӿ�SDAΪ���
 * ��  �� : ��
 * ����ֵ : ��
 **************************************************************************************/
void SCCB_SDA_OUT(void)
{											   	 
 	GPIO_InitTypeDef  GPIO_InitStructure;
	
 	//ʹ��OV7670оƬSCCB�ӿ�SDA���Ŷ�ӦIO�˿�ʱ��	
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOG, ENABLE);	 
	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;				     //����SDA��Ӧ����IO���
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //����SDA��Ӧ����IO����״̬Ϊ�������
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //����SDA��Ӧ����IO�����ٶ�Ϊ50MHZ
  GPIO_Init(GPIOG, &GPIO_InitStructure);					     //��ʼ��SDA��Ӧ����IO 
}	

/**************************************************************************************
 * ��  �� : ����OV7670оƬSCCB�ӿ�SDAΪ����
 * ��  �� : ��
 * ����ֵ : ��
 **************************************************************************************/
void SCCB_SDA_IN(void)
{											   	 
 	GPIO_InitTypeDef  GPIO_InitStructure;
	
 	//ʹ��OV7670оƬSCCB�ӿ�SDA���Ŷ�ӦIO�˿�ʱ��	
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOG, ENABLE);	 
	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;				     //����SDA��Ӧ����IO���
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 		   //����SDA��Ӧ����IO����״̬Ϊ��������
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //����SDA��Ӧ����IO�����ٶ�Ϊ50MHZ
  GPIO_Init(GPIOG, &GPIO_InitStructure);					     //��ʼ��SDA��Ӧ����IO
}	

/**************************************************************************************
 * ��  �� : ����OV7670оƬSCCB�ӿ���ʼ�ź�(ģ��SCCB)
 * ��  �� : ��
 * ����ֵ : ��
 **************************************************************************************/
void SCCB_Start(void)
{
    SCCB_SDA_H;     
    SCCB_SCL_H;	    
    sw_delay_us(50);  
    SCCB_SDA_L;
    sw_delay_us(50);	 
    SCCB_SCL_L;	   
}

/**************************************************************************************
 * ��  �� : ����OV7670оƬSCCB�ӿ�ֹͣ�ź�(ģ��SCCB)
 * ��  �� : ��
 * ����ֵ : ��
 **************************************************************************************/
void SCCB_Stop(void)
{
    SCCB_SDA_L;
    sw_delay_us(50);	 
    SCCB_SCL_H;	
    sw_delay_us(50); 
    SCCB_SDA_H;	
    sw_delay_us(50);
} 

/**************************************************************************************
 * ��  �� : ����OV7670оƬSCCB�ӿڲ���NAK�ź�(ģ��SCCB)
 * ��  �� : ��
 * ����ֵ : ��
 **************************************************************************************/
void SCCB_No_Ack(void)
{
	sw_delay_us(50);
	SCCB_SDA_H;	
	SCCB_SCL_H;	
	sw_delay_us(50);
	SCCB_SCL_L;	
	sw_delay_us(50);
	SCCB_SDA_L;	
	sw_delay_us(50);
}

/**************************************************************************************
 * ��  �� : OV7670оƬSCCB�ӿ�дһ���ֽ�����(ģ��SCCB)
 * ��  �� : ��
 * ����ֵ : 0,�ɹ�;1,ʧ��. 
 **************************************************************************************/
uint8_t SCCB_WriteAbyte(uint8_t dat)
{
	uint8_t i,temp;	 
	for(i=0;i<8;i++)     //ѭ��8�η���һ���ֽ�����
	{
		if(dat&0x80)
			SCCB_SDA_H;	
		else 
			SCCB_SDA_L;
		dat<<=1;
		sw_delay_us(50);
		SCCB_SCL_H;	
		sw_delay_us(50);
		SCCB_SCL_L;		   
	}			 
	SCCB_SDA_IN();		  //����SDAΪ���� 
	sw_delay_us(50);
	SCCB_SCL_H;			    //���յھ�λ,���ж��Ƿ��ͳɹ�
	sw_delay_us(50);
	if(SCCB_Read_SDA)
		temp=1;           //SDA�ź�Ϊ�ߵ�ƽ��ʾ����ʧ�ܣ�����1
	else 
		temp=0;           //SDA�ź�Ϊ�͵�ƽ��ʾ���ͳɹ�������0
	SCCB_SCL_L;		 
	SCCB_SDA_OUT();		  //��ȡSDA�źź�������SDAΪ���    
	return temp;  
}	 

/**************************************************************************************
 * ��  �� : OV7670оƬSCCB�ӿڶ�һ���ֽ�����(ģ��SCCB)
 * ��  �� : ��
 * ����ֵ : ������һ���ֽ�����
 **************************************************************************************/
uint8_t SCCB_ReadAbyte(void)
{
	uint8_t i,temp;    
	temp=0;             //����ʱ������ʼ��ֵΪ0
	SCCB_SDA_IN();		  //����SDAΪ����  
	for(i=8;i>0;i--) 	  //ѭ��8�ν�������
	{		     	  
		sw_delay_us(50);
		SCCB_SCL_H;
		temp=temp<<1;
		if(SCCB_Read_SDA)
			temp++;   
		sw_delay_us(50);
		SCCB_SCL_L;
	}	
	SCCB_SDA_OUT();		 //��ȡSDA�źź�������SDAΪ���     
	return temp;
} 							    

/**************************************************************************************
 * ��  �� : OV7670оƬSCCB�ӿ�д�Ĵ�������(ģ��SCCB)
 * ��  �� : ��
 * ����ֵ : 0,�ɹ�;1,ʧ��.
 **************************************************************************************/
uint8_t SCCB_Write_Reg(uint8_t reg,uint8_t data)
{
	uint8_t temp=0;
	SCCB_Start(); 					            //����SCCB����
	if(SCCB_WriteAbyte(OV7670_ID))
		temp=1;	                          //д����ID	  
	sw_delay_us(100);
  if(SCCB_WriteAbyte(reg))
		temp=1;		                        //д�Ĵ�����ַ	  
	sw_delay_us(100);
  if(SCCB_WriteAbyte(data))
		temp=1; 	                        //д����	 
  SCCB_Stop();	                      //����SCCB����
  return	temp;
}		  					    

/**************************************************************************************
 * ��  �� : OV7670оƬSCCB�ӿڶ��Ĵ�������(ģ��SCCB)
 * ��  �� : ��
 * ����ֵ : �����ļĴ���ֵ
 **************************************************************************************/
uint8_t SCCB_Read_Reg(uint8_t reg)
{
	uint8_t temp=0;
	SCCB_Start(); 				             //����SCCB����
	SCCB_WriteAbyte(OV7670_ID);		       //д����ID	  
	sw_delay_us(100);	 
  SCCB_WriteAbyte(reg);		           //д�Ĵ�����ַ	  
	sw_delay_us(100);	  
	SCCB_Stop();   	                   //����SCCB����
	sw_delay_us(100);	   
	SCCB_Start(); 				             //����SCCB����
	SCCB_WriteAbyte(OV7670_ID|0X01);   	 //���Ͷ�����	  
	sw_delay_us(100);
  temp=SCCB_ReadAbyte();		         //��ȡ����
  SCCB_No_Ack();                     //����NAK�ź� 
  SCCB_Stop();  	                   //����SCCB����
  return temp;
}

