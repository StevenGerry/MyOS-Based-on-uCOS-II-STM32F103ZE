#include "stm32f10x.h"

#define  IIC_SDA_0     GPIO_ResetBits(GPIOB, GPIO_Pin_10) 
#define  IIC_SDA_1     GPIO_SetBits(GPIOB, GPIO_Pin_10) 
#define  IIC_SCL_0     GPIO_ResetBits(GPIOB, GPIO_Pin_11) 
#define  IIC_SCL_1     GPIO_SetBits(GPIOB, GPIO_Pin_11) 

#define  IIC_SDA_READ  GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_10)	

void IIC_GPIO_Init(void);  
FunctionalState IIC_Start(void);
void IIC_Stop(void);
void IIC_Ack(void);
void IIC_NAck(void);
FunctionalState IIC_Wait_Ack(void);
void IIC_Send_Byte(uint8_t Data);
uint8_t IIC_Read_Byte(uint8_t ack);

