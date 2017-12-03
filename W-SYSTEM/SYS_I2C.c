#include "stm32f10x.h"
#include "SYS_I2C.h"
I2CFUNC I2CFunc={MYI2C_Init,MYI2C_GenerateSTART,MYI2C_GenerateSTOP,MYI2C_Ack,MYI2C_Nack,MYI2C_Check,MYI2C_Send7bitAddress,MYI2C_Write8bit,MYI2C_Read8bit};
void MYI2C_Init (I2CPORT PORT)
{
	GPIO_InitTypeDef GPIO_InitData;
	GPIO_InitData.GPIO_Pin = PORT.scl | PORT.sda;
	GPIO_InitData.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitData.GPIO_Mode = GPIO_Mode_Out_OD;
	
		switch ((uint32_t) PORT.GPIOx)
	{
		case (uint32_t)GPIOA : 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);break;
		case (uint32_t)GPIOB :	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);break;
		case (uint32_t)GPIOC : 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);break;
		case (uint32_t)GPIOD : 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);break;
		case (uint32_t)GPIOE :	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);break;
		case (uint32_t)GPIOF :	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOF, ENABLE);break;
		case (uint32_t)GPIOG :	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOG, ENABLE);
	}
	
	GPIO_Init(PORT.GPIOx, &GPIO_InitData);
}
void MYI2C_GenerateSTART(I2CPORT PORT)
{
	GPIO_SetBits(PORT.GPIOx,PORT.scl);//1
	GPIO_SetBits(PORT.GPIOx,PORT.sda);//1
	//
	GPIO_ResetBits(PORT.GPIOx,PORT.sda);//0
}
void MYI2C_GenerateSTOP(I2CPORT PORT)
{
	GPIO_ResetBits(PORT.GPIOx,PORT.scl);//0
	GPIO_ResetBits(PORT.GPIOx,PORT.sda);//0
	//
	GPIO_SetBits(PORT.GPIOx,PORT.scl);//1
	GPIO_ResetBits(PORT.GPIOx,PORT.scl);//0
}
void MYI2C_Ack (I2CPORT PORT)
{
	GPIO_ResetBits(PORT.GPIOx,PORT.scl);//0
	//
	GPIO_ResetBits(PORT.GPIOx,PORT.sda);//0
	//
	GPIO_SetBits(PORT.GPIOx,PORT.scl);//1
	//
	GPIO_ResetBits(PORT.GPIOx,PORT.scl);//0
	GPIO_SetBits(PORT.GPIOx,PORT.sda);//1
}
void MYI2C_Nack(I2CPORT PORT)
{
	GPIO_ResetBits(PORT.GPIOx,PORT.scl);//0
	//
	GPIO_SetBits(PORT.GPIOx,PORT.sda);//1
	//
	GPIO_SetBits(PORT.GPIOx,PORT.scl);//1
	//
	GPIO_ResetBits(PORT.GPIOx,PORT.scl);//0
}
uint8_t MYI2C_Check (I2CPORT PORT)
{
	uint16_t c=0;
	GPIO_ResetBits(PORT.GPIOx,PORT.scl);//0
	//
	GPIO_SetBits(PORT.GPIOx,PORT.sda);//1
	//
	GPIO_SetBits(PORT.GPIOx,PORT.scl);//1
	//
	while ((PORT.GPIOx->IDR & PORT.sda) != (uint32_t)Bit_RESET)
	{
		c++;
		if (c==2000)
		{
			return 1;
		}
	}
	GPIO_ResetBits(PORT.GPIOx,PORT.scl);//0
	return 0;
}
void MYI2C_Send7bitAddress(I2CPORT PORT,uint8_t slaveAddr,uint8_t rw)
{
	uint8_t i,data=slaveAddr | rw;
	for (i=0;i<8;i++)
		{
			GPIO_ResetBits(PORT.GPIOx,PORT.scl);//0
			//
			if (data&0x80)
			{
				GPIO_SetBits(PORT.GPIOx,PORT.sda);//1
			}
			else
			{
				GPIO_ResetBits(PORT.GPIOx,PORT.sda);//0
			}
			//
			GPIO_SetBits(PORT.GPIOx,PORT.scl);//1
			//
			GPIO_ResetBits(PORT.GPIOx,PORT.scl);//0
			data<<=1;
		}
}
void MYI2C_Write8bit(I2CPORT PORT,uint8_t data)
{
	uint8_t i;
	for (i=0;i<8;i++)
		{
			GPIO_ResetBits(PORT.GPIOx,PORT.scl);//0
			//
			if (data&0x80)
			{
				GPIO_SetBits(PORT.GPIOx,PORT.sda);//1
			}
			else
			{
				GPIO_ResetBits(PORT.GPIOx,PORT.sda);//0
			}
			//
			GPIO_SetBits(PORT.GPIOx,PORT.scl);//1
			//
			GPIO_ResetBits(PORT.GPIOx,PORT.scl);//0
			data<<=1;
		}
}
uint8_t MYI2C_Read8bit(I2CPORT PORT)
{
	uint8_t data=0,i=0;
	while (i<8)
	{
		GPIO_ResetBits(PORT.GPIOx,PORT.scl);//0
		//
		GPIO_SetBits(PORT.GPIOx,PORT.scl);//1
		//
		data=data | ((PORT.GPIOx->IDR & PORT.sda) != (uint32_t)Bit_RESET);
		GPIO_ResetBits(PORT.GPIOx,PORT.scl);//0
		if (i==7) return data;
		i++;
		data<<=1;
	}
	return 0;
}
