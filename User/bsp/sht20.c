#include "stm32f10x.h"
#include "sht20.h"
#include "user_usart.h"
#include "user_iic.h"	
#include "delay.h"

 float temperatureC;
 float humidityRH;
 
 uint8_t sndata1[8];
 uint8_t sndata2[6];
 uint32_t SN1; 
 uint32_t SN2; 

 /**************************************************************************************
 * ��  �� : ��SHT20дһ�ֽ�����
 * ��  �� : SendByte: ��д������        WriteAddress: ��д���ַ
 * ����ֵ :   1---�ɹ�д��,     0---ʧ��
 **************************************************************************************/
FunctionalState SHT2X_IIC_WriteByte(uint8_t WriteAddress,uint8_t SendByte)
{		
    if(!IIC_Start())   return DISABLE;
    IIC_Send_Byte(0x80); 
    if(!IIC_Wait_Ack()){IIC_Stop(); return DISABLE;}
    IIC_Send_Byte(WriteAddress);                // ���õ���ʼ��ַ 
    IIC_Wait_Ack();	
    IIC_Send_Byte(SendByte);
    IIC_Wait_Ack();   
    IIC_Stop(); 
    return ENABLE;
}	

 /**************************************************************************************
 * ��  �� : ��SHT20��ȡһ������
 * ��  �� : ReadAddress: ��������ַ    length: ����������    pBuffer: ��Ŷ�������
 * ����ֵ :   1---�ɹ���ȡ,     0---ʧ��
 **************************************************************************************/
FunctionalState SHT2X_IIC_ReadByte( uint8_t ReadAddress, uint16_t length  ,uint8_t* pBuffer)
{		
	  if(!IIC_Start())return DISABLE;
    IIC_Send_Byte(0x80);              // ���ø���ʼ��ַ+������ַ 
    if(!IIC_Wait_Ack())
		{IIC_Stop(); return DISABLE;}
    IIC_Send_Byte(ReadAddress);       // ���õ���ʼ��ַ      
    IIC_Wait_Ack();
    IIC_Start();
    IIC_Send_Byte(0x81);
    IIC_Wait_Ack();
    while(length)
    {
			*pBuffer = IIC_Read_Byte(0x00);
			if(length == 1)
			IIC_NAck();
			else IIC_Ack(); 
			pBuffer++;
			length--;
    }
    IIC_Stop();
    return ENABLE;
} 

 /**************************************************************************************
 * ��  �� : SHT20У�麯��
 * ��  �� : data[]: �ȴ����������   startByte: ��ʼ����ı��
            number: �������         checksum: ���յ���CRC����
 * ����ֵ :   1---�ɹ����,     0---ʧ��
 **************************************************************************************/
FunctionalState SHT2x_CheckCrc(uint8_t data[],uint8_t startBytes,uint8_t number, uint8_t checksum)
{
	uint8_t bit=0;
	uint8_t crc = 0;	
  uint8_t byteCtr;
  //calculates 8-Bit checksum with given polynomial
  for (byteCtr = startBytes; byteCtr < startBytes+number; ++byteCtr)
  { crc ^= (data[byteCtr]);
    for (bit = 8; bit > 0; --bit)
    { if (crc & 0x80) crc = (crc << 1) ^ 0x131;
      else crc = (crc << 1);
    }
  }
  if (crc ==checksum) 
	return ENABLE;
  else
	return DISABLE;
}

 /**************************************************************************************
 * ��  �� : SHT20���û��Ĵ�������
 * ��  �� : ��
 * ����ֵ :  ���ض�ȡ���Ĳ���ֵ
 **************************************************************************************/
uint8_t SHT2x_ReadUserRegister(void)
{
	uint8_t data[1]={0};
  SHT2X_IIC_ReadByte( USER_REG_R, 1 ,data);
  return data[0];	
}

 /**************************************************************************************
 * ��  �� : SHT20д�û��Ĵ�������
 * ��  �� : userdata: ��д��Ĳ���
 * ����ֵ :   1---�ɹ�д��,     0---ʧ��
 **************************************************************************************/
FunctionalState SHT2x_WriteUserRegister(uint8_t userdata)
{
	SHT2X_IIC_WriteByte(USER_REG_W ,userdata);
	if(userdata==SHT2x_ReadUserRegister())
	return ENABLE;
	else 
	return DISABLE;
}

 /**************************************************************************************
 * ��  �� : �����¶�
 * ��  �� : ��
 * ����ֵ :   1---�ɹ�������¶�ֵ,     0---ʧ��
 **************************************************************************************/
FunctionalState SHT2x_Calc_T(void)
{
	 uint8_t length=0;
	 uint8_t Tdata[3]={0};
	 if(!IIC_Start())return DISABLE;
    IIC_Send_Byte(I2C_ADR_W); 
    if(!IIC_Wait_Ack())
		{IIC_Stop(); return DISABLE;}
    IIC_Send_Byte(TRIG_T_MEASUREMENT_POLL);      
    IIC_Wait_Ack();
		sw_delay_us(20);
	  IIC_Stop();
		do
		{
		IIC_Start();
    IIC_Send_Byte(I2C_ADR_R);
		}
		while(!IIC_Wait_Ack());
		for(length=0;length<=3;length++)
		{
			Tdata[length]=IIC_Read_Byte(0x00);
			IIC_Ack(); 		
		};
		IIC_NAck();		
		IIC_Stop();		
		if(((Tdata[0]+Tdata[1]+Tdata[2])>0)&&SHT2x_CheckCrc(Tdata,0,2,Tdata[2]))	
    temperatureC= (-46.85 + (175.72/65536 )*((float)((((uint16_t)Tdata[0]<<8)+(uint16_t)Tdata[1])&0xfffc)));
		else
		return DISABLE;
	  return ENABLE;
}

 /**************************************************************************************
 * ��  �� : ����ʪ��
 * ��  �� : ��
 * ����ֵ :   1---�ɹ������ʪ��ֵ,     0---ʧ��
 **************************************************************************************/
FunctionalState SHT2x_Calc_RH(void)
{	
   uint8_t length=0;
	 uint8_t RHdata[3]={0};
	 if(!IIC_Start())return DISABLE;
    IIC_Send_Byte(I2C_ADR_W); 
    if(!IIC_Wait_Ack())
		{IIC_Stop(); return DISABLE;}
    IIC_Send_Byte(TRIG_RH_MEASUREMENT_POLL);      
    IIC_Wait_Ack();
		sw_delay_us(20);
	  IIC_Stop();
		do
		{
		IIC_Start();
    IIC_Send_Byte(I2C_ADR_R);
		}
		while(!IIC_Wait_Ack());
		for(length=0;length<=3;length++)
		{
			RHdata[length]=IIC_Read_Byte(0x00);
			IIC_Ack(); 		
		};
		IIC_NAck();		
		IIC_Stop();
		if(((RHdata[0]+RHdata[1]+RHdata[2])>0)&&SHT2x_CheckCrc(RHdata,0,2,RHdata[2]))		
	  humidityRH = -6.0 + 125.0/65536 * ((float)((((uint16_t)RHdata[0]<<8)+(uint16_t)RHdata[1])&0xfff0)); 
		else
		return DISABLE;
	  return ENABLE;
}

 /**************************************************************************************
 * ��  �� : SHT20�����λ
 * ��  �� : ��
 * ����ֵ :   1---�ɹ���λ,     0---ʧ��
 **************************************************************************************/
FunctionalState SHT2x_SoftReset(void)
{
	 if(!IIC_Start())return DISABLE;
    IIC_Send_Byte(0x80); 
    if(!IIC_Wait_Ack()){IIC_Stop(); return DISABLE;}
    IIC_Send_Byte(SOFT_RESET);       
    IIC_Wait_Ack();	  
    IIC_Stop(); 
  	sw_delay_ms(1500);
		return ENABLE;	
}

 /**************************************************************************************
 * ��  �� : SHT20���кŻ�ȡ
 * ��  �� : pBuffer1[]: ָ��1   pBuffer2: ָ��2
 * ����ֵ :   1---�ɹ����,     0---ʧ��
 **************************************************************************************/
FunctionalState SHT2x_GetSerialNumber(uint8_t *pBuffer1,uint8_t *pBuffer2)
{   
	  uint8_t length=8;
	  if(!IIC_Start())return DISABLE;
    IIC_Send_Byte(0x80); 
    if(!IIC_Wait_Ack()){IIC_Stop(); return DISABLE;}
    IIC_Send_Byte(0xfa);       
    IIC_Wait_Ack();	
    IIC_Send_Byte(0x0f);
    IIC_Wait_Ack();   
		if(!IIC_Start())return DISABLE;
    IIC_Send_Byte(0x81); 
    if(!IIC_Wait_Ack()){IIC_Stop(); return DISABLE;}
    while(length)
    {
      *pBuffer1 = IIC_Read_Byte(0x00);
      if(length == 1)
				IIC_NAck();
      else IIC_Ack(); 
      pBuffer1++;
      length--;
    }
     IIC_Stop();
		length=6;
	  if(!IIC_Start())return DISABLE;
    IIC_Send_Byte(0x80); 
    if(!IIC_Wait_Ack()){IIC_Stop(); return DISABLE;}
    IIC_Send_Byte(0xfc);     
    IIC_Wait_Ack();	
    IIC_Send_Byte(0xc9);
    IIC_Wait_Ack();   
		if(!IIC_Start())return DISABLE;
    IIC_Send_Byte(0x81); 
    if(!IIC_Wait_Ack()){IIC_Stop(); return DISABLE;}
    while(length)
    {
      *pBuffer2 = IIC_Read_Byte(0x00);
      if(length == 1)
				IIC_NAck();
      else IIC_Ack(); 
      pBuffer2++;
      length--;
    }
     IIC_Stop();	
    return ENABLE;
}

 /**************************************************************************************
 * ��  �� : SHT20��ʼ��
 * ��  �� : ��
 * ����ֵ :  ��
 **************************************************************************************/
void SHT2X_Init(void)
{
		SHT2x_GetSerialNumber(sndata1,sndata2);
		if((sndata1[0]+sndata1[1]+sndata1[3]+sndata1[4]+sndata1[5]+sndata1[6]+sndata1[7])>0)
		{		
		if(
		SHT2x_CheckCrc(sndata1,0,1,sndata1[1])&&
		SHT2x_CheckCrc(sndata1,2,1,sndata1[3])&&
		SHT2x_CheckCrc(sndata1,4,1,sndata1[5])&&
		SHT2x_CheckCrc(sndata1,6,1,sndata1[7])&&
		SHT2x_CheckCrc(sndata2,0,2,sndata2[2])&&
		SHT2x_CheckCrc(sndata2,3,2,sndata2[5])
		)
		{
		printf("SHT2X CRC ok\r\n");
		SN1=((sndata2[3]<<24)+(sndata2[4]<<16)+(sndata1[0]<<8)+sndata1[2]);
		SN2=((sndata1[4]<<24)+(sndata1[6]<<16)+(sndata2[0]<<8)+sndata2[1]);
		printf("SHT2X SN:0x%x%x\r\n",SN1,SN2);
		}
		else
		printf("SHT2X CRC error\r\n");	
		}
		else
		{
		SHT2x_GetSerialNumber(sndata1,sndata2);
		printf("SHT2Xͨ�Ŵ���\r\n");
		}

		if(SHT2x_WriteUserRegister(0x3a))
		printf("���óɹ�\r\n");
		else 
		printf("���ô���\r\n");
}

 /**************************************************************************************
 * ��  �� : SHT20��ʪ�ȴ��ڷ��ͺ���
 * ��  �� : ��
 * ����ֵ : ��
 **************************************************************************************/
void SHT2X_TEST(void)
{
	  //�¶Ȳ���OK��ͨ������1�ϴ�
		if(SHT2x_Calc_T())
		{ 
			 printf("�¶ȣ�%f\r\n",temperatureC);
		}
    //ʪ�Ȳ���OK��ͨ������1�ϴ�
		if(SHT2x_Calc_RH())
		{
			printf("ʪ�ȣ�%f%%\r\n",humidityRH);
		}
}

float SHT2X_RE(char input)
{
	  //�¶Ȳ���OK��ͨ������1�ϴ�
		if(SHT2x_Calc_T()&&input=='T')
		{ 
			 printf("�¶ȣ�%f\r\n",temperatureC);
			return temperatureC;
		}
    //ʪ�Ȳ���OK��ͨ������1�ϴ�
		if(SHT2x_Calc_RH()&&input=='H')
		{
			printf("ʪ�ȣ�%f%%\r\n",humidityRH);
			return humidityRH;
		}
}
