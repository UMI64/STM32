#include "mpu6050.h"
static GyroscopeData Gyroscopedata;
void I2C1_Init ()
{
	I2C_InitTypeDef I2C1_InitData;
	I2C1_InitData.I2C_ClockSpeed=400000;
	I2C1_InitData.I2C_Mode=I2C_Mode_I2C;
	I2C1_InitData.I2C_Ack=I2C_Ack_Enable;
	I2C1_InitData.I2C_AcknowledgedAddress=I2C_AcknowledgedAddress_7bit;
	I2C1_InitData.I2C_DutyCycle=I2C_DutyCycle_16_9;
	I2C1_InitData.I2C_OwnAddress1=0x00;
	I2C_Init (I2C1,&I2C1_InitData);
}
void MPU_6050_INIT ()
{
	
}

/*系统接口函数*/
TESTPASS MPU_6050_TEST ()
{
	return NOTPASS;
}
u16 Get_X_Angular_Acceleration ()
{
	return 0;
}
u16 Get_Y_Angular_Acceleration ()
{
	return 0;
}
u16 Get_Z_Angular_Acceleration ()
{
	return 0;
}
u16 Get_X_Acceleration ()
{
	return 0;
}
u16 Get_Y_Acceleration ()
{
	return 0;
}
u16 Get_Z_Acceleration ()
{
	return 0;
}
void MPU_6050_LoadInfo (DriverInfoData* PDriverInfo)
{
	Gyroscopedata.Get_X_Angular_Acceleration=Get_X_Angular_Acceleration;
	Gyroscopedata.Get_Y_Angular_Acceleration=Get_Y_Angular_Acceleration;
	Gyroscopedata.Get_Z_Angular_Acceleration=Get_Z_Angular_Acceleration;
	Gyroscopedata.Get_X_Acceleration=Get_X_Acceleration;
	Gyroscopedata.Get_Y_Acceleration=Get_Y_Acceleration;
	Gyroscopedata.Get_Z_Acceleration=Get_Z_Acceleration;
	PDriverInfo->Test=MPU_6050_TEST;
	PDriverInfo->PInfo=&Gyroscopedata;
	PDriverInfo->DriverID=50;
}
