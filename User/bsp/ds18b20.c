#include "stm32f10x.h"
#include "ds18b20.h"
#include "delay.h"

volatile uint8_t ReadTempFlag;


static void DS18B20_GPIO_Config(void)
{		
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(DS18B20_CLK, ENABLE); 
															   
   GPIO_InitStructure.GPIO_Pin = DS18B20_PIN;	
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;     //推挽输出  
   GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
   GPIO_Init(DS18B20_PORT, &GPIO_InitStructure);
	
	GPIO_SetBits(DS18B20_PORT, DS18B20_PIN);
}


static void DS18B20_Mode_IPU(void)
{
 	GPIO_InitTypeDef GPIO_InitStructure;
	
	GPIO_InitStructure.GPIO_Pin = DS18B20_PIN; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;	  //上拉输入
	GPIO_Init(DS18B20_PORT, &GPIO_InitStructure);
}


static void DS18B20_Mode_Out_PP(void)
{
 	GPIO_InitTypeDef GPIO_InitStructure;
															   
   GPIO_InitStructure.GPIO_Pin = DS18B20_PIN;	

   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   //推挽输出   
   GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
   GPIO_Init(DS18B20_PORT, &GPIO_InitStructure);
}


static void DS18B20_Rst(void)
{
	DS18B20_Mode_Out_PP();
	
	DS18B20_DATA_OUT(LOW);
	sw_delay_us(750);
	DS18B20_DATA_OUT(HIGH);
	sw_delay_us(15);
}


static uint8_t DS18B20_Presence(void)
{
	uint8_t pulse_time = 0;
	
	DS18B20_Mode_IPU();
	 
	while( DS18B20_DATA_IN() && pulse_time<100 )
	{
		pulse_time++;
		sw_delay_us(1);
	}	

	if( pulse_time >=100 )
		return 1;
	else
		pulse_time = 0;

	while( !DS18B20_DATA_IN() && pulse_time<240 ) 
	{
		pulse_time++;
		sw_delay_us(1);
	}	
   
	if( pulse_time >=240 )
		return 1;
	else
		return 0;
}

static uint8_t DS18B20_Read_Bit(void)
{
	uint8_t dat;

	DS18B20_Mode_Out_PP();
	DS18B20_DATA_OUT(LOW);
	sw_delay_us(10);
	DS18B20_Mode_IPU();
	
	if( DS18B20_DATA_IN() == SET )
		dat = 1;
	else
		dat = 0;
	sw_delay_us(45);
	
	return dat;
}


uint8_t DS18B20_Read_Byte(void)
{
	uint8_t i, j, dat = 0;	
	
	for(i=0; i<8; i++) 
	{
		j = DS18B20_Read_Bit();
		dat = (dat) | (j<<i);
	}
	
	return dat;																																																																																
}

void DS18B20_Write_Byte(uint8_t dat)
{
	uint8_t i, testb;
	DS18B20_Mode_Out_PP();
	
	for( i=0; i<8; i++ )
	{
		testb = dat&0x01;
		dat = dat>>1;		
		if (testb)
		{			
			DS18B20_DATA_OUT(LOW);
			sw_delay_us(8);
			
			DS18B20_DATA_OUT(HIGH);
			sw_delay_us(58);
		}		
		else
		{			
			DS18B20_DATA_OUT(LOW);  
			sw_delay_us(70);
			
			DS18B20_DATA_OUT(HIGH);			
			sw_delay_us(2);
		}
	}
}

uint8_t DS18B20_Init(void)
{
	DS18B20_GPIO_Config();
	DS18B20_Rst();
	
	return DS18B20_Presence();
}

float DS18B20_Get_Temp(void)
{
	uint8_t tpmsb, tplsb;
	short s_tem;
	float f_tem;
	
	DS18B20_Rst();	   
	DS18B20_Presence();	 
	DS18B20_Write_Byte(0XCC);	
	DS18B20_Write_Byte(0X44);
	
	DS18B20_Rst();
  DS18B20_Presence();
	DS18B20_Write_Byte(0XCC);
  DS18B20_Write_Byte(0XBE);
	
	tplsb = DS18B20_Read_Byte();
	tpmsb = DS18B20_Read_Byte(); 
	
	s_tem = tpmsb<<8;
	s_tem = s_tem | tplsb;
	
	if( s_tem < 0 )
  {
    ReadTempFlag=1;
		f_tem = (~s_tem+1) * 0.0625;	
  }
	else
  {
    ReadTempFlag=0;
		f_tem = s_tem * 0.0625;
  }
	
	return f_tem; 	
}



