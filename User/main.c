#include "stm32f10x.h"
#include "stm32f10x_USART.h"
#include "stm32f10x_GPIO.h"
#include <includes.h>
#include <ucos_ii.h>
#include <os_cpu.h>
#include "esp8266.h"
#include "delay.h"
#include "led.h"
#include "oled.h"
#include "oledfont.h"
#include "user_ili9341_lcd.h"
#include "ov7670.h" 
#include "user_exti.h"
#include "user_usart.h"
#include "user_iic.h"	
#include "ds18b20.h"


static OS_STK taskbutton_stk[TASKBTN_STK_SIZE];
static OS_STK taskwifi_stk[TASKWIFI_STK_SIZE];
static OS_STK tasklcd1_stk[TASKLCD1_STK_SIZE];
static OS_STK tasktime_stk[TASKTIME_STK_SIZE];
static OS_STK tasktemp_stk[TASKTEMP_STK_SIZE];

void led_config(void);
void GPIO_Config(void);
static void startup_task(void);
static void taskbutton(void*p_arg);
static void tasklcd1(void*p_arg);
static void taskwifi(void*p_arg);
static void tasktime(void*p_arg);
static void tasktemp(void*p_arg);
void delay(__IO u32 nCount);

int MENU_STATUS=-1;
int CLR=1;
int CAMERA_SIGN=0;

uint8_t	i;
uint8_t strTemp[5];
uint8_t strTemp2[5];
uint8_t strHumi[5];
float fTemp;
float temp;
float humi;
char  strtemp[5];
char  strtemp2[5];
char  strhumi[5];

extern uint8_t frame_flag;

static void startup_task()
{
	RCC_ClocksTypeDef rcc_clocks;
	RCC_GetClocksFreq(&rcc_clocks);
	SysTick_Config(rcc_clocks.HCLK_Frequency / OS_TICKS_PER_SEC);
	GPIO_Config();
	GPIO_ResetBits(GPIOF,GPIO_Pin_12);
	OS_CPU_SysTickInit();
	OSTaskCreate(taskbutton, (void *)0, &taskbutton_stk[TASKBTN_STK_SIZE - 1], 7);
	OSTaskCreate(taskwifi, (void *)0, &taskwifi_stk[TASKWIFI_STK_SIZE - 1], 8);
	OSTaskCreate(tasktime, (void *)0, &tasktime_stk[TASKTIME_STK_SIZE - 1], 9);
	OSTaskCreate(tasklcd1, (void *)0, &tasklcd1_stk[TASKLCD1_STK_SIZE - 1], 10);
	OSTaskCreate(tasktemp, (void *)0, &tasktemp_stk[TASKTEMP_STK_SIZE - 1], 11);
	//OSTaskDel(OS_PRIO_SELF);
}

void GPIO_Config(void){
	GPIO_InitTypeDef GPIO_InitStructure; 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOF, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOF, &GPIO_InitStructure);
	GPIO_SetBits(GPIOF, GPIO_Pin_12);
}

void OV7670_tftlcd(void)
{
  uint16_t i,j;
 	uint16_t color;	 
	
	if(frame_flag)
	{
		Lcd_GramScan(2);
		OV7670_RRST_L;
		OV7670_RCK_L;
		OV7670_RCK_H;
		OV7670_RCK_L;
		OV7670_RRST_H;
		OV7670_RCK_H;  
    
		for(i=0;i<9600;i++)
		{
		  for(j=0;j<8;j++)
		  {
			  OV7670_RCK_L;
			  color=GPIOC->IDR&0xFF;
			  OV7670_RCK_H; 
			  color<<=8;  
			  OV7670_RCK_L;
			  color|=GPIOC->IDR&0xFF;
			  OV7670_RCK_H; 
        LCD_WR_Data( color );
		  }
    }       
		frame_flag=1;
	} 
}

void taskbutton(void *p_arg)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);	 
  GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5;		
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
  GPIO_Init(GPIOE, &GPIO_InitStructure);	
	LCD_P8x16Str(1,0,"uCOS-II RUNNING");
	while(1)
	{
		if(GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_2) == 0 && MENU_STATUS == 0 )  
	   {	 
			  sw_delay_ms(10);
        if(GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_2) == 0 )
        {
		      led_on(LED_1); 
					MENU_STATUS=1;
					CLR=0;
		      while(GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_2) == 0 )
          {
            MENU_STATUS=1;
          }       
          led_off(LED_1); 
					MENU_STATUS=1;					
        }
     }
		 
		 if(GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_3) == 0 && MENU_STATUS == 0 )  
	   {	 
			  sw_delay_ms(10);
        if(GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_3) == 0 )
        {
		      led_on(LED_1); 
					MENU_STATUS=2;
					CLR=0;
		      while(GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_3) == 0 )
          {
            MENU_STATUS=2;
          }       
          led_off(LED_1); 
					MENU_STATUS=2;					
        }
     }
		 
		 if(GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_4) == 0 && MENU_STATUS == 0 )  
	   {	 
			  sw_delay_ms(10);
        if(GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_4) == 0 )
        {
		      led_on(LED_1); 
					MENU_STATUS=3;
					CLR=0;
		      while(GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_4) == 0 )
          {
            MENU_STATUS=3;
          }       
          led_off(LED_1); 
					MENU_STATUS=3;					
        }
     }
		 
		 if(GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_5) == 0 && MENU_STATUS == 0 )  
	   {	 
			  sw_delay_ms(10);
        if(GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_5) == 0 )
        {
		      led_on(LED_1); 
					MENU_STATUS=4;
					CLR=0;
		      while(GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_5) == 0 )
          {
            MENU_STATUS=4;
          }       
          led_off(LED_1); 
					MENU_STATUS=4;					
        }
     }
		 
		 if(GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_1) != 0 )  
	   {	 
			  sw_delay_ms(10);
        if(GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_1) != 0 )
        {
					CLR=0;
		      led_on(LED_1); 
					MENU_STATUS=0;
					while(GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_1) != 0 )
          {
						MENU_STATUS=0;
						GPIO_SetBits(GPIOF, GPIO_Pin_12);
          } 
					GPIO_ResetBits(GPIOF,GPIO_Pin_12);
					MENU_STATUS=0;
          led_off(LED_1);             
        }
     }
		OSTimeDly(50);
	}
}

void taskwifi(void *p_arg)
{
	LCD_P8x16Str(1,4,"WiFi: On    ");
	LCD_P8x16Str(1,6,"Cmaera: Off");
	while(1)
	{
		OSTimeDly(50);
		if(Hand("LEDON"))
			{
					USART_ITConfig(USART3, USART_IT_RXNE, DISABLE);
					led_on(LED_3);
					CLR_Buf();		
					USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);				
			}  
			else if(Hand("LEDOFF"))
			{
					USART_ITConfig(USART3, USART_IT_RXNE, DISABLE);
					led_off(LED_3);
					CLR_Buf();
					USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);	
			}
	}
}

void tasktime(void *p_arg)
{
	int ss=0;
	int mm=0;
	int hh=12;
	char pss[2];
	char pmm[2];
	char phh[2];
	pss[0]='0';
	pmm[0]='0';
	phh[0]='0';
	while(1)
	{
		ss++;
		if(ss==60){ss=0;mm++;}
		if(mm==60){ss=0;mm=0;hh++;}
		if(hh==24){ss=0;mm=0;hh=0;}
		if (ss<10)
		{
			pss[0]='0';
			sprintf(&pss[1],"%d",ss);
		}
		else
			sprintf(pss,"%d",ss);
		if (mm<10)
		{
			pmm[0]='0';
			sprintf(&pmm[1],"%d",mm);
		}
		else
			sprintf(pmm,"%d",mm);
		if (hh<10)
		{
			phh[0]='0';
			sprintf(&phh[1],"%d",hh);
		}
		else
			sprintf(phh,"%d",hh);
		LCD_P8x16Str(1,2,"Time:   :  :  ");
		LCD_P8x16Str(48,2,(u8*)phh);
		LCD_P8x16Str(72,2,(u8*)pmm);
		LCD_P8x16Str(96,2,(u8*)pss);
		delay(0x00ffef);
		led_on(LED_4);
		delay(0x00ffef);
		led_off(LED_4);
		OSTimeDly(900);
	}
}


void tasklcd1(void *p_arg)
{
	while(1)
	{
		if(MENU_STATUS==-1)
		{	
			if (CLR!=1)
			{
				LCD_Clear(0, 20, 240, 320, BACKGROUND);
				CLR=1;
			}
			LCD_DispStr(0, 140, (uint8_t *)"----------------------------------------", CYAN);
			LCD_DispStr(0, 150, (uint8_t *)"             TOUCH TO ENTER             ", CYAN);
			LCD_DispStr(0, 160, (uint8_t *)"----------------------------------------", CYAN);
			//LCD_DispChar(60, 60, 'A', BLUE);
			//LCD_DispStr(40, 100, (uint8_t *)"count:", WHITE);
		}
		if(MENU_STATUS==0)
		{	
			if (CAMERA_SIGN == 1)
			{
				Lcd_GramScan(1);
				CAMERA_SIGN = 0;
				LCD_P8x16Str(64,6,"Off");
			}
			if (CLR!=1)
			{
				LCD_Clear(0, 20, 240, 320, BACKGROUND);
				CLR=1;
			}
			LCD_DispStr(0, 0, (uint8_t *)" Welcome! GERRY SYSTEM uC/OS-II BasedOS ", CYAN);
			LCD_DispStr(0, 20, (uint8_t *)"                  MENU                  ", CYAN);
			LCD_DispStr(0, 30, (uint8_t *)"----------------------------------------", CYAN);
			LCD_DispStr(0, 50, (uint8_t *)"1.SENSOR DATA", CYAN);
			LCD_DispStr(0, 70, (uint8_t *)"2.DATA TRANSFER", CYAN);
			LCD_DispStr(0, 90, (uint8_t *)"3.OPEN CAMERA", CYAN);
			LCD_DispStr(0, 110, (uint8_t *)"4.NETWORK DATA", CYAN);
			//LCD_DispChar(60, 60, 'A', BLUE);
			//LCD_DispStr(40, 100, (uint8_t *)"count:", WHITE);
		}
		else if(MENU_STATUS==1)
		{	
			if (CLR!=1)
			{
				LCD_Clear(0, 20, 240, 320, BACKGROUND);
				CLR=1;
			}
			sprintf(strtemp, "%.01f", fTemp);
			for(i=0; i<5; i++)
			{
				strTemp[i]=(uint8_t)strtemp[i];    
			}
			LCD_DispStr(0, 0, (uint8_t *)" Welcome! GERRY SYSTEM uC/OS-II BasedOS ", CYAN);
			LCD_DispStr(0, 20, (uint8_t *)"              SENSOR DATA               ", CYAN);
			LCD_DispStr(0, 30, (uint8_t *)"----------------------------------------", CYAN);
			LCD_DispStr(0, 70, (uint8_t *)"TEMPERATURE-DS18B20:", CYAN);
			LCD_DispStr(125, 70, (uint8_t *)strTemp, YELLOW);
			//LCD_DispStr(40, 100, (uint8_t *)"count:", WHITE);
		}
		else if(MENU_STATUS==2)
		{	
			if (CLR!=1)
			{
				LCD_Clear(0, 20, 240, 320, BACKGROUND);
				CLR=1;
			}
			LCD_DispStr(0, 0, (uint8_t *)" Welcome! GERRY SYSTEM uC/OS-II BasedOS ", CYAN);
			LCD_DispStr(0, 20, (uint8_t *)"             DATA TRANSFER              ", CYAN);
			LCD_DispStr(0, 30, (uint8_t *)"----------------------------------------", CYAN);
			LCD_DispStr(0, 50, (uint8_t *)"RECEIVE LOG:----------------------------", CYAN);
			LCD_DispStr(0, 160, (uint8_t *)"SENDINF LOG:----------------------------", CYAN);
			//LCD_DispChar(60, 60, 'A', BLUE);
			//LCD_DispStr(40, 100, (uint8_t *)"count:", WHITE);
		}
		else if(MENU_STATUS==3)
		{	
			if (CLR!=1)
			{
				LCD_Clear(0, 20, 240, 320, BACKGROUND);
				CLR=1;
			}
			OV7670_tftlcd();
			CAMERA_SIGN = 1;
			LCD_P8x16Str(64,6,"    ");
			LCD_P8x16Str(64,6,"On");
		}
		else if(MENU_STATUS==4)
		{	
			if (CLR!=1)
			{	
				LCD_Clear(0, 20, 240, 320, BACKGROUND);
				CLR=1;
			}
			LCD_DispStr(0, 0, (uint8_t *)" Welcome! GERRY SYSTEM uC/OS-II BasedOS ", CYAN);
			LCD_DispStr(0, 20, (uint8_t *)"              NETWORK DATA              ", CYAN);
			LCD_DispStr(0, 30, (uint8_t *)"----------------------------------------", CYAN);
			LCD_DispStr(0, 50, (uint8_t *)"STATION+AP MODE:192.168.4.1:5000", CYAN);
			//LCD_DispChar(60, 60, 'A', BLUE);
			//LCD_DispStr(40, 100, (uint8_t *)"count:", WHITE);
		}
	}
}

void tasktemp(void *p_arg)
{
	while(1)
	{
		fTemp = DS18B20_Get_Temp();
		sw_delay_ms(850);
		delay(0x04ffef);
		led_on(LED_4);
		delay(0x04ffef);
		led_off(LED_4);
		OSTimeDly(1000);
	}
}

void delay(__IO u32 nCount){

	while(nCount--)
		;

}

int main(void)
{
	//Initialization
	sw_delay_ms(100);
	SPI_GPIO_Init();	
  SPI2_Init();	
  OLED_Init();
	LCD_Init();	
	USART1_Init();
	IIC_GPIO_Init(); 	
	while(DS18B20_Init())
	{
      LCD_P8x16Str(48,0,"CHECK");
      LCD_P8x16Str(0,6,"Sensor ERROR!");
	}
	
	//Screen Test
	OLED_Fill(0xff);
	sw_delay_ms(1000);
	
	//Screen Clean
  OLED_Fill(0x00);
	
	//Start Loading
	LCD_P8x16Str(1,0,"uCOS-II LOADING");
	esp8266_init();	
  USART3_Init();	
	leds_init();
	sw_delay_ms(100);
	
	//Wifi Initialization
	while(!Hand("OK"))
	{		
		  printf("AT\r\n");
		  sw_delay_ms(500);
	}
	
  CLR_Buf();
	LCD_P8x16Str(1,4,"WiFi: On    ");

	while(!(Hand("OK") | Hand("no change")))
	{		
		  printf("AT+CWMODE=3\r\n");
			LCD_P8x16Str(1,2,"AT+CWMODE=3");
		  sw_delay_ms(500);
	}

  if(Hand("OK")) 
  {		
		  printf("AT+RST\r\n");
			LCD_P8x16Str(1,2,"AT+RST");
		  sw_delay_ms(500);
	}
  CLR_Buf();

	while(!(Hand("OK")))
	{		
		  printf("AT+CIPMUX=1\r\n");
			LCD_P8x16Str(1,2,"AT+CIPMUX=1");
		  sw_delay_ms(500);
	}
  CLR_Buf();

	while(!(Hand("OK")))
	{		
		  printf("AT+CIPSERVER=1,5000\r\n");
			LCD_P8x16Str(1,2,"AT+SERVER5000");
		  sw_delay_ms(500);
	}
  CLR_Buf();
	while(!(Hand("OK")))
	{		
		  printf("AT+CIFSR\r\n");
			LCD_P8x16Str(1,2,"AT+CIFSR");
		  sw_delay_ms(500);
	}
  CLR_Buf();
	led_on(LED_3); 
	
	//Screen clear
	OLED_Fill(0x00);
	LCD_Clear(0, 20, 240, 320, BACKGROUND);
	
	while(OV7670_Init())
	{
		sw_delay_ms(20);
	} 
	sw_delay_ms(20);
	OV7670_PA1_Init();
	OV7670_Window_Set(12,176,240,320);
	OV7670_CS_L;
	sw_delay_ms(5);
		
	//Start OS
	OSInit();
	startup_task();
	OSStart();
	//END
}

