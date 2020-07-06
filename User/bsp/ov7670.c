#include "stm32f10x.h"
#include "ov7670.h" 
#include "sccb.h"
#include "delay.h"		 
#include "ov7670config.h"	

uint8_t OV7670_Init(void)
{
	uint8_t temp;
	uint16_t i=0;
	 
 	GPIO_InitTypeDef  GPIO_InitStructure;
	//ʹ��OV7670оƬʹ�����Ŷ�ӦIO�˿�ʱ��	
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOE|RCC_APB2Periph_GPIOF|RCC_APB2Periph_GPIOG|RCC_APB2Periph_AFIO, ENABLE);	 //????????
  
	//VSY�ź�(PA1)
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;				         //����PA1��Ӧ����IO���
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 		       //����PA1��Ӧ����IO����״̬Ϊ��������
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		     //����PA1��Ӧ����IO�����ٶ�Ϊ50MHZ
  GPIO_Init(GPIOA, &GPIO_InitStructure);					         //��ʼ��PA1��Ӧ����IO
	
	//OE�ź�(PG8)  WEN�ź�(PG5)	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5|GPIO_Pin_8;		 //����OE��WEN��Ӧ����IO���
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		     //����OE��WEN��Ӧ����IO����״̬Ϊ�������
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		     //����OE��WEN��Ӧ����IO�����ٶ�Ϊ50MHZ
  GPIO_Init(GPIOG, &GPIO_InitStructure);					         //��ʼ��OE��WEN��Ӧ����IO 

	//���ݿ�(PC0~PC7)  	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4\
	                               |GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7;	    //����PC0~PC7��Ӧ����IO���
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 		       //����PC0~PC7��Ӧ����IO����״̬Ϊ��������
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		     //����PC0~PC7��Ӧ����IO�����ٶ�Ϊ50MHZ
  GPIO_Init(GPIOC, &GPIO_InitStructure);
	 
	//WRST�ź�(PC13)
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;		           //����WRST��Ӧ����IO���
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		     //����WRST��Ӧ����IO����״̬Ϊ�������
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		     //����WRST��Ӧ����IO�����ٶ�Ϊ50MHZ
  GPIO_Init(GPIOC, &GPIO_InitStructure);					         //��ʼ��WRST��Ӧ����IO 
	
	//RCLK�ź�(PF15)
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;		           //����RCLK��Ӧ����IO���
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		     //����RCLK��Ӧ����IO����״̬Ϊ�������
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		     //����RCLK��Ӧ����IO�����ٶ�Ϊ50MHZ
  GPIO_Init(GPIOF, &GPIO_InitStructure);					         //��ʼ��RCLK��Ӧ����IO 
	
	//RRST�ź�(PE6)
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;		             //����WRST��Ӧ����IO���
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		     //����WRST��Ӧ����IO����״̬Ϊ�������
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		     //����WRST��Ӧ����IO�����ٶ�Ϊ50MHZ
  GPIO_Init(GPIOE, &GPIO_InitStructure);					         //��ʼ��WRST��Ӧ����IO 
	
	GPIO_SetBits(GPIOG,GPIO_Pin_5|GPIO_Pin_8);						   //����PG5��PG8Ϊ�ߵ�ƽ����ʼ��OE��WEN����Ϊ��
	GPIO_SetBits(GPIOC,GPIO_Pin_13);					               //����PC13Ϊ�ߵ�ƽ����ʼ��WRST����Ϊ��
	GPIO_SetBits(GPIOF,GPIO_Pin_15);					               //����PF15Ϊ�ߵ�ƽ����ʼ��RCLK����Ϊ��
	GPIO_SetBits(GPIOE,GPIO_Pin_6);					                 //����PE6Ϊ�ߵ�ƽ����ʼ��RRST����Ϊ��

 	SCCB_GPIO_Init();        		                             //��ʼ��OV7670оƬSCCB�ӿڶ�ӦIO��  	  
 	if(SCCB_Write_Reg(0x12,0x80))                            //SCCB����д����OV7670оƬCOM7�Ĵ���
		return 1;	                                             //SCCB�Ĵ�����λ	  
	sw_delay_ms(50); 
	//��ȡ��Ʒʶ���(���λʶ��ź͵�λʶ��Ŷ����㷽��)
 	temp=SCCB_Read_Reg(0x0b);                                //SCCB���߶�����OV7670оƬVER�Ĵ��� 
	if(temp!=0x73)                                           //�ж϶����Ĳ�Ʒ��λʶ����ǲ���0x73
		return 2;                                              //������0x73˵��оƬ����OV7670��Ӳ������
 	temp=SCCB_Read_Reg(0x0a);                                //SCCB���߶�����OV7670оƬPID�Ĵ���   
	if(temp!=0x76)                                           //�ж϶����Ĳ�Ʒ��λʶ����ǲ���0x76
		return 2;                                              //������0x76˵��оƬ����OV7670��Ӳ������
	
	//SCCB����д����OV7670оƬʣ��N���Ĵ�����������OV7670���Ч��
	for(i=0;i<sizeof(ov7670_init_reg_table)/sizeof(ov7670_init_reg_table[0]);i++)
	{
	  SCCB_Write_Reg(ov7670_init_reg_table[i][0],ov7670_init_reg_table[i][1]);
 	}
   	return 0x00; 	                                         //��ɶ�OV7670�Ĵ�������������00
} 

/**************************************************************************************
 * ��  �� : ����OV7670оƬ��ؼĴ�����ƽ��ɫ��
 * ��  �� : uint8_t mode ����ģʽ
 * ����ֵ : ��
 **************************************************************************************/
void OV7670_Light_Mode(uint8_t mode)
{
	uint8_t reg01val=0;
	uint8_t reg02val=0;
	uint8_t reg13val=0xE7;//Ĭ�Ͼ�������Ϊ�Զ���ƽ��
	
	switch(mode)
	{
		case 1:                  //����
			reg01val=0x5A;
			reg02val=0x5C;
		  reg13val=0xE5;
			break;	
		case 2:                  //����
			reg01val=0x58;
			reg02val=0x60;
			reg13val=0xE5;
			break;	
		case 3:                  //�칫��
			reg01val=0x84;
			reg02val=0x4c;
			reg13val=0xE5;
			break;	
		case 4:                  //�Ӽ�
			reg01val=0x96;
			reg02val=0x40;
		  reg13val=0xE5;
			break;	
	}
	SCCB_Write_Reg(0x13,reg13val);     //SCCB����д����OV7670оƬCOM7�Ĵ��� 
	SCCB_Write_Reg(0x01,reg01val);     //SCCB����д����OV7670оƬBLUE�Ĵ���
	SCCB_Write_Reg(0x02,reg02val);     //SCCB����д����OV7670оƬRED�Ĵ���
}				  

/**************************************************************************************
 * ��  �� : ����OV7670оƬ��ؼĴ����Խ���ɫ������
 * ��  �� : uint8_t tone ɫ��
 * ����ֵ : ��
 **************************************************************************************/
void OV7670_Color_Saturation(uint8_t tone)
{
	uint8_t reg4fval=0x80;              //0
	uint8_t reg50val=0x80;
 	uint8_t reg52val=0x22;
	uint8_t reg53val=0x5E;
	uint8_t reg54val=0x80;
 	switch(tone)
	{
		case 0:                      //-2
			reg4fval=0x40; 
      reg50val=0x40;		
			reg52val=0x11;
			reg53val=0x2F;
		  reg54val=0x40;
			break;	
		case 1:                      //-1
			reg4fval=0x66;
      reg50val=0x66;		
			reg52val=0x1B;
			reg53val=0x4B;	
      reg54val=0x66;		
			break;	
		case 3:                      //1
			reg4fval=0x99;
      reg50val=0x99;			
			reg52val=0x28;
			reg53val=0x71;	
      reg54val=0x99;			
			break;	
		case 4:                      //2
			reg4fval=0xC0;
      reg50val=0xC0;		
			reg52val=0x33;
			reg53val=0x8D;
      reg54val=0xC0;		
			break;	
	}
	SCCB_Write_Reg(0x4F,reg4fval);	   //SCCB����д����OV7670оƬMTX1�Ĵ��� 
	SCCB_Write_Reg(0x50,reg50val);     //SCCB����д����OV7670оƬMTX2�Ĵ���  
	SCCB_Write_Reg(0x51,0x00);	       //SCCB����д����OV7670оƬMTX3�Ĵ���  
	SCCB_Write_Reg(0x52,reg52val);     //SCCB����д����OV7670оƬMTX4�Ĵ��� 
	SCCB_Write_Reg(0x53,reg53val);     //SCCB����д����OV7670оƬMTX5�Ĵ��� 
	SCCB_Write_Reg(0x54,reg54val);     //SCCB����д����OV7670оƬMTX6�Ĵ���  
	SCCB_Write_Reg(0x58,0x9E);         //SCCB����д����OV7670оƬMTXS�Ĵ��� 
}

/**************************************************************************************
 * ��  �� : ����OV7670оƬ��ؼĴ����Խ�����������
 * ��  �� : uint8_t bright ����
 * ����ֵ : ��
 **************************************************************************************/
void OV7670_Brightness(uint8_t bright)
{
	uint8_t reg55val=0x00;                      //0
  switch(bright)
	{
		case 0:                              //-2
			reg55val=0xB0;	 	 
			break;	
		case 1:                              //-1
			reg55val=0x98;	 	 
			break;	
		case 3:                              //1
			reg55val=0x18;	 	 
			break;	
		case 4:                              //2
			reg55val=0x30;	 	 
			break;	
	}
	SCCB_Write_Reg(0x55,reg55val);         //SCCB����д����OV7670оƬBRIGHT�Ĵ��� 
}

/**************************************************************************************
 * ��  �� : ����OV7670оƬ��ؼĴ����Խ��жԱȶ�����
 * ��  �� : uint8_t contrast �Աȶ�
 * ����ֵ : ��
 **************************************************************************************/
void OV7670_Contrast(uint8_t contrast)
{
	uint8_t reg56val=0x40;                      //0
  	switch(contrast)
	{
		case 0:                              //-2
			reg56val=0x30;	 	 
			break;	
		case 1:                              //-1
			reg56val=0x38;	 	 
			break;	
		case 3:                              //1
			reg56val=0x50;	 	 
			break;	
		case 4:                              //2
			reg56val=0x60;	 	 
			break;	
	}
	SCCB_Write_Reg(0x56,reg56val);         //SCCB����д����OV7670оƬCONTRAS�Ĵ��� 
}
	
/**************************************************************************************
 * ��  �� : ����OV7670оƬ��ؼĴ����Խ�����Ч����
 * ��  �� : uint8_t effect ��Ч
 * ����ֵ : ��
 **************************************************************************************/
void OV7670_Special_Effects(uint8_t effect)
{
	uint8_t reg3aval=0x04;                      //��ͨģʽ
	uint8_t reg67val=0xC0;
	uint8_t reg68val=0x80;
	switch(effect)
	{
		case 1:                              //��Ƭ
			reg3aval=0x24;
			reg67val=0x80;
			reg68val=0x80;
			break;	
		case 2:                              //�ڰ�
			reg3aval=0x14;
			reg67val=0x80;
			reg68val=0x80;
			break;	
		case 3:                              //ƫ��ɫ
			reg3aval=0x14;
			reg67val=0xC0;
			reg68val=0x80;
			break;	
		case 4:                              //ƫ��ɫ
			reg3aval=0x14;
			reg67val=0x40;
			reg68val=0x40;
			break;	
		case 5:                              //ƫ��ɫ
			reg3aval=0x14;
			reg67val=0x80;
			reg68val=0xC0;
			break;	
		case 6:                              //����
			reg3aval=0x14;
			reg67val=0xA0;
			reg68val=0x40;
			break;	 
	}
	SCCB_Write_Reg(0x3A,reg3aval);         //SCCB����д����OV7670оƬTSLB�Ĵ���
	SCCB_Write_Reg(0x67,reg67val);         //SCCB����д����OV7670оƬMANU�Ĵ���
	SCCB_Write_Reg(0x68,reg68val);         //SCCB����д����OV7670оƬMANV�Ĵ���
}	

/**************************************************************************************
 * ��  �� : ����OV7670оƬ��ؼĴ���������ͼ���������
 * ��  �� : uint16_t sx,uint16_t sy,uint16_t width,uint16_t height
 * ����ֵ : ��
 **************************************************************************************/
void OV7670_Window_Set(uint16_t sx,uint16_t sy,uint16_t width,uint16_t height)
{
	uint16_t endx;
	uint16_t endy;
	uint8_t temp; 
	endx=sx+width*2;	               
 	endy=sy+height*2;
	if(endy>784)endy-=784;
	temp=SCCB_Read_Reg(0x03);			         //SCCB���߶�����OV7670оƬVREF�Ĵ���	
	temp&=0xF0;
	temp|=((endx&0x03)<<2)|(sx&0x03);
	SCCB_Write_Reg(0x03,temp);			       //SCCB����д����OV7670оƬVREF�Ĵ���	
	SCCB_Write_Reg(0x19,sx>>2);		         //SCCB����д����OV7670оƬVSTRT�Ĵ���
	SCCB_Write_Reg(0x1A,endx>>2);	         //SCCB����д����OV7670оƬVSTOP�Ĵ���

	temp=SCCB_Read_Reg(0x32);			         //SCCB���߶�����OV7670оƬHREF�Ĵ���	
	temp&=0xC0;
	temp|=((endy&0x07)<<3)|(sy&0x07);
	SCCB_Write_Reg(0x17,sy>>3);		         //SCCB����д����OV7670оƬHSTART�Ĵ���
	SCCB_Write_Reg(0x18,endy>>3);	         //SCCB����д����OV7670оƬHSTOP�Ĵ���
}

