#include "stm32f10x.h"


#define HIGH  1
#define LOW   0

#define DS18B20_CLK     RCC_APB2Periph_GPIOB
#define DS18B20_PIN     GPIO_Pin_1                  
#define DS18B20_PORT		GPIOB 

//带参宏，可以像内联函数一样使用,输出高电平或低电平
#define DS18B20_DATA_OUT(a)	if (a)	\
					GPIO_SetBits(GPIOB,GPIO_Pin_1);\
					else		\
					GPIO_ResetBits(GPIOB,GPIO_Pin_1)
 //读取引脚的电平
#define  DS18B20_DATA_IN()	   GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_1)

extern volatile uint8_t ReadTempFlag;
          
uint8_t DS18B20_Init(void);
float DS18B20_Get_Temp(void);
