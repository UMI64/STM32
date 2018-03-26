#ifndef __I2C_H
#define __I2C_H
#include "stm32f10x.h"
typedef struct I2CPORT
{
	uint16_t scl;
	uint16_t sda;
	GPIO_TypeDef * GPIOx;
}I2CPORT;

void MYI2C_Init (I2CPORT PORT);
void MYI2C_GenerateSTART(I2CPORT PORT);
void MYI2C_GenerateSTOP(I2CPORT PORT);
void MYI2C_Ack (I2CPORT PORT);
void MYI2C_Nack(I2CPORT PORT);
uint8_t MYI2C_Check (I2CPORT PORT);
void MYI2C_Send7bitAddress(I2CPORT PORT,uint8_t slaveAddr,uint8_t rw);
void MYI2C_Write8bit(I2CPORT PORT,uint8_t data);
uint8_t MYI2C_Read8bit(I2CPORT PORT);

typedef struct I2CFUNC
{
	void (* MYI2C_Init)(I2CPORT PORT);
	void (* MYI2C_GenerateSTART)(I2CPORT PORT);
	void (* MYI2C_GenerateSTOP)(I2CPORT PORT);
	void (* MYI2C_Ack)(I2CPORT PORT);
	void (* MYI2C_Nack)(I2CPORT PORT);
	uint8_t (* MYI2C_Check)(I2CPORT PORT);
	void (* MYI2C_Send7bitAddress)(I2CPORT PORT,uint8_t slaveAddr,uint8_t rw);
	void (* MYI2C_Write8bit)(I2CPORT PORT,uint8_t data);
	uint8_t (* MYI2C_Read8bit)(I2CPORT PORT);
}I2CFUNC;

extern I2CFUNC I2CFunc;
#endif
