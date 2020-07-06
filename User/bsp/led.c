#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "led.h"
#include "delay.h"

void leds_init(void)
{
   GPIO_InitTypeDef  GPIO_InitStructure;
   RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOG, ENABLE);	 
   GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_6;		
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
   GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
   GPIO_Init(GPIOG, &GPIO_InitStructure);	
	
	
   GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_8;	
   GPIO_Init(GPIOA, &GPIO_InitStructure);	
	
   GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_8 | GPIO_Pin_9;	
   GPIO_Init(GPIOB, &GPIO_InitStructure);	

	 leds_off();					
}

void leds_on(void)
{
   GPIO_ResetBits(GPIOG,GPIO_Pin_6);
	 GPIO_ResetBits(GPIOA,GPIO_Pin_8);
	 GPIO_ResetBits(GPIOB,GPIO_Pin_8|GPIO_Pin_9);
}


void leds_off(void)
{
   GPIO_SetBits(GPIOG,GPIO_Pin_6);
	 GPIO_SetBits(GPIOA,GPIO_Pin_8);
	 GPIO_SetBits(GPIOB,GPIO_Pin_8|GPIO_Pin_9);
}


void led_on(uint32_t led_idx)
{
	 uint16_t pin_num = (uint16_t)(led_idx&0xFFFF);
	 if(led_idx == LED_1)GPIOG->BRR = pin_num;
	 if(led_idx == LED_2)GPIOA->BRR = pin_num;
	 if((led_idx == LED_3) || (led_idx == LED_4))
	 {
		  GPIOB->BRR = pin_num;
	 }
}


void led_off(uint32_t led_idx)
{
	 uint16_t pin_num = (uint16_t)(led_idx&0xFFFF);
	 if(led_idx == LED_1)GPIOG->BSRR = pin_num;
	 if(led_idx == LED_2)GPIOA->BSRR = pin_num;
	 if((led_idx == LED_3) || (led_idx == LED_4))
	 {
		   GPIOB->BSRR = pin_num;
	 }
}


void led_toggle(uint32_t led_idx)
{

	 uint16_t pin_num = (uint16_t)(led_idx&0xFFFF);

	 if(led_idx == LED_1)
	 {
		 GPIOG->ODR ^= pin_num;
	 }
	 if(led_idx == LED_2)
	 {
		 GPIOA->ODR ^= pin_num;
	 }
	 if((led_idx == LED_3) || (led_idx == LED_4))
	 {
		 GPIOB->ODR ^= pin_num;
	 }
}


