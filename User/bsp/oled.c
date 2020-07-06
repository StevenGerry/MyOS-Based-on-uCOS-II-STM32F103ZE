#include "stm32f10x.h"
#include "stm32f10x_spi.h"
#include "oled.h"
#include "delay.h"


/**************************************************************************************
 * ��  �� : ��ʼ��OLED���õ���IO��
 * ��  �� : ��
 * ����ֵ : ��
 **************************************************************************************/
void SPI_GPIO_Init(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;

  //������GPIO��ʱ��
  RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOG , ENABLE); 
  
  //���õ�IO��PB5
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;                
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;           //�������
	GPIO_Init(GPIOB, &GPIO_InitStructure);
  //���õ�IO��PB13 PB15��SPI��CLK MOSI
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_15;                
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;           //�����������
	GPIO_Init(GPIOB, &GPIO_InitStructure);
  //���õ�IO��PG4 PG11
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_11;                
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;           //�������
	GPIO_Init(GPIOG, &GPIO_InitStructure);
}

/***************************************************************************
 * ��  �� : SPI2��д���ݺ�������дһ���ֽ�
 * ��  �� : Dat��д������� mode��=0��д�����=1��д������
 * ����ֵ : ��ȡ������
 **************************************************************************/
uint8_t SPI2_ReadWriteByte(uint8_t Dat,uint8_t mode)                                       
{		
	uint8_t retry=0;	

  if(mode == OLED_WR_CMD) 
    OLED_DC_0;
  else 
    OLED_DC_1;
  
	/* Loop while DR register in not emplty */
	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET)      //���ͻ����־λΪ��
	{
		retry++;
		if(retry>200)return 0;
	}			  
	SPI_I2S_SendData(SPI2, Dat);                                        //ͨ������SPI2����һ������
	retry=0;
	/* Wait to receive a byte */
	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET)     //���ջ����־λ��Ϊ��
	{
		retry++;
		if(retry>200)return 0;
	}	  						    
	/* Return the byte read from the SPI bus */
	return SPI_I2S_ReceiveData(SPI2);                                   //ͨ��SPI2���ؽ�������				    
}

/***************************************************************************
 * ��  �� : ����SPI2����
 * ��  �� : ��
 * ����ֵ : ��
 **************************************************************************/
void SPI2_Init(void)
{	 
	SPI_InitTypeDef  SPI_InitStructure;
    
  // Enable SPI2 clocks
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE); //SPI2ʱ��ʹ��

	SPI_Cmd(SPI2, DISABLE); 
	/* SPI2 configuration */                                              //��ʼ��SPI�ṹ��
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;    //SPI����Ϊ˫��ȫ˫��
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;		                      //����SPIΪ��ģʽ
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;		                  //SPI���ͽ���8λ֡�ṹ
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;		                        //SPIʱ�ӿ���ʱΪ�͵�ƽ
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;	                        //��һ��ʱ���ؿ�ʼ��������
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;		                          //NSS�ź��������ʹ��SSIλ������
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_32;   //SPI2������Ԥ��ƵֵΪ32
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;	                  //���ݴ����MSBλ��ʼ
	SPI_InitStructure.SPI_CRCPolynomial = 7;	                            //CRCֵ����Ķ���ʽ

	SPI_Init(SPI2, &SPI_InitStructure);      //����SPI_InitStruct��ָ���Ĳ�����ʼ������SPI2�Ĵ���
	SPI_Cmd(SPI2, ENABLE);                   //ʹ��SPI2����
	SPI2_ReadWriteByte(0xff,0);              //��������	
} 
 
/******************************************************************************
 * ��  �� : ��������
 * ��  �� : x��x���ꣻy��y����
 * ����ֵ : ��
 ******************************************************************************/
void OLED_Set_Pos(uint8_t x, uint8_t y) 
{ 
    SPI2_ReadWriteByte((0xb0+y),OLED_WR_CMD);
    SPI2_ReadWriteByte(((x&0xf0)>>4)|0x10,OLED_WR_CMD);
    SPI2_ReadWriteByte((x&0x0f)|0x01,OLED_WR_CMD); 
} 

/******************************************************************************
 * ��  �� : OLED��������
 * ��  �� : ��
 * ����ֵ : ��
 ******************************************************************************/
void OLED_Fill(uint8_t dat) 
{
    uint8_t y,x;
    for(y=0;y<8;y++)
    {
        SPI2_ReadWriteByte(0xb0+y,OLED_WR_CMD);//����ҳ��ַ��0~7��
        SPI2_ReadWriteByte(0x02,OLED_WR_CMD); //������ʾλ�á��е͵�ַ
        SPI2_ReadWriteByte(0x10,OLED_WR_CMD); //������ʾλ�á��иߵ�ַ
        for(x=0; x<X_WIDTH; x++)
            SPI2_ReadWriteByte(dat,OLED_WR_DAT);
    }
}

/******************************************************************************
 * ��  �� : ָ��λ����ʾһ���ַ�
 * ��  �� : x:��0~127��y:ҳ��ַ0~7��
 * ����ֵ : ��
 ******************************************************************************/
void OLED_ShowChar(uint8_t x,uint8_t y,uint8_t chr)
{      	
  uint8_t c=0,i=0;	
  
  c = chr-' ';//�õ�ƫ�ƺ��ֵ			
  if(x > Max_Column-1)
  {
    x = 0;
    y = y+2;
  }

  OLED_Set_Pos(x,y);	
  for(i=0; i<8; i++) SPI2_ReadWriteByte(codeF8X16[c*16+i],OLED_WR_DAT); 
  OLED_Set_Pos(x,y+1);
  for(i=0;i<8;i++)SPI2_ReadWriteByte(codeF8X16[c*16+i+8],OLED_WR_DAT);
}

/******************************************************************************
 * ��  �� : ��ʾ8*16һ���׼ASCII�ַ���
 * ��  �� : x:��0~127��y:ҳ��ַ0~7��
 * ����ֵ : ��
 ******************************************************************************/
void LCD_P8x16Str(uint8_t x, uint8_t y,uint8_t ch[])
{
  uint8_t c=0,i=0,j=0;
    
  while (ch[j] != '\0')
  {    
    c = ch[j]-32;
    if(x>120){x=0;y++;}
    OLED_Set_Pos(x,y);    
    for(i=0; i<8; i++)SPI2_ReadWriteByte(codeF8X16[c*16+i],OLED_WR_DAT);          
    OLED_Set_Pos(x,y+1);    
    for(i=0;i<8;i++) SPI2_ReadWriteByte(codeF8X16[c*16+i+8],OLED_WR_DAT);        
    x += 8;
    j++;
  }
}

/******************************************************************************
 * ��  �� : ��ʾ����
 * ��  �� : x:��0~127��y:ҳ��ַ0~7��
 * ����ֵ : ��
 ******************************************************************************/
void LCD_P16x16Ch(uint8_t x, uint8_t y, uint8_t N)
{
    uint8_t wm=0;
    unsigned int adder=32*N;        
    OLED_Set_Pos(x , y);
    for(wm = 0;wm < 16;wm++)               
    {
        SPI2_ReadWriteByte(codeF16x16[adder],OLED_WR_DAT);    
        adder += 1;
    }      
    OLED_Set_Pos(x,y + 1); 
    for(wm = 0;wm < 16;wm++)         
    {
        SPI2_ReadWriteByte(codeF16x16[adder],OLED_WR_DAT);
        adder += 1;
    }           
}

/******************************************************************************
 * ��  �� : ��ʾBMPͼƬ128��64
 * ��  �� : ��ʼ������(x,y),x�ķ�Χ0��127��yΪҳ�ķ�Χ0��7
 * ����ֵ : ��
 ******************************************************************************/
void OLED_DrawBMP(uint8_t x0, uint8_t y0,uint8_t x1, uint8_t y1,const uint8_t BMP[])
{ 	
  uint16_t j=0;
  uint8_t x,y;
  
  if((y1 % 8) == 0) y = y1/8;      
  else y = y1/8+1;
  for(y=y0;y<y1;y++)
  {
    OLED_Set_Pos(x0,y);
    for(x=x0; x<x1; x++)
    {      
      SPI2_ReadWriteByte(BMP[j++],OLED_WR_DAT);	    	
    }
  }
} 

/******************************************************************************
 * ��  �� : OLED��ʼ��
 * ��  �� : ��
 * ����ֵ : ��
 ******************************************************************************/
void OLED_Init(void)     
{    
		OLED_RES_0;    //OLED��λ��������
    OLED_CS_1;     //OLEDʹ�������øߵ�ƽ
	  sw_delay_ms(10);       //����ʱ�ز�����,�Ǹ���ʼ���㹻��ʱ��
	  OLED_RES_1;    //OLED��λ��������
    OLED_CS_0;     //OLEDʹ�������õ͵�ƽ
    sw_delay_ms(200);

    SPI2_ReadWriteByte(0xae,OLED_WR_CMD);//--turn off oled panel
    SPI2_ReadWriteByte(0x00,OLED_WR_CMD);//---set low column address
    SPI2_ReadWriteByte(0x10,OLED_WR_CMD);//---set high column address
    SPI2_ReadWriteByte(0x40,OLED_WR_CMD);//--set start line address  Set Mapping RAM Display Start Line (0x00~0x3F)
    SPI2_ReadWriteByte(0x81,OLED_WR_CMD);//--set contrast control register
    SPI2_ReadWriteByte(0xcf,OLED_WR_CMD); // Set SEG Output Current Brightness
	
    SPI2_ReadWriteByte(0xa1,OLED_WR_CMD);//--Set SEG/Column Mapping     0xa0���ҷ��� 0xa1����
    SPI2_ReadWriteByte(0xc8,OLED_WR_CMD);//Set COM/Row Scan Direction   0xc0���·��� 0xc8����
    SPI2_ReadWriteByte(0xa6,OLED_WR_CMD);//--set normal display
    SPI2_ReadWriteByte(0xa8,OLED_WR_CMD);//--set multiplex ratio(1 to 64)
    SPI2_ReadWriteByte(0x3f,OLED_WR_CMD);//--1/64 duty
	
    SPI2_ReadWriteByte(0xd3,OLED_WR_CMD);//-set display offset    Shift Mapping RAM Counter (0x00~0x3F)
    SPI2_ReadWriteByte(0x00,OLED_WR_CMD);//-not offset
		
    SPI2_ReadWriteByte(0xd5,OLED_WR_CMD);//--set display clock divide ratio/oscillator frequency
    SPI2_ReadWriteByte(0x80,OLED_WR_CMD);//--set divide ratio, Set Clock as 100 Frames/Sec
    SPI2_ReadWriteByte(0xd9,OLED_WR_CMD);//--set pre-charge period
		
    SPI2_ReadWriteByte(0xf1,OLED_WR_CMD);//Set Pre-Charge as 15 Clocks & Discharge as 1 Clock
		//OLED_WrByte(0x22,OLED_WR_CMD);
    SPI2_ReadWriteByte(0xda,OLED_WR_CMD);//--set com pins hardware configuration
    SPI2_ReadWriteByte(0x12,OLED_WR_CMD);
    SPI2_ReadWriteByte(0xdb,OLED_WR_CMD);//--set vcomh
		
    SPI2_ReadWriteByte(0x40,OLED_WR_CMD);//Set VCOM Deselect Level
		
    SPI2_ReadWriteByte(0x20,OLED_WR_CMD);//-Set Page Addressing Mode (0x00/0x01/0x02)
    SPI2_ReadWriteByte(0x02,OLED_WR_CMD);//
    SPI2_ReadWriteByte(0x8d,OLED_WR_CMD);//--set Charge Pump enable/disable
    SPI2_ReadWriteByte(0x14,OLED_WR_CMD);//--set(0x10) disable
    SPI2_ReadWriteByte(0xa4,OLED_WR_CMD);// Disable Entire Display On (0xa4/0xa5)
    SPI2_ReadWriteByte(0xa6,OLED_WR_CMD);// Disable Inverse Display On (0xa6/a7) 
    SPI2_ReadWriteByte(0xaf,OLED_WR_CMD);//--turn on oled panel
    OLED_Fill(0xff);  //��ʼ����
    OLED_Set_Pos(0,0);     
}


/*********************************END FILE*************************************/
