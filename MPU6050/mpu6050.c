#include "mpu6050.h"
static GyroscopeData Gyroscopedata;
I2CPORT PORT;
uint8_t flag;
void MPU6050_I2C_ERROR (uint8_t f)
{
	flag=f;
	while (1);
}
void MPU6050_I2C_ByteWrite(u8 slaveAddr, u8 pBuffer, u8 writeAddr)
{
	vTaskSuspendAll ();
	
	I2CFunc.MYI2C_GenerateSTART (PORT);
	I2CFunc.MYI2C_Send7bitAddress (PORT,slaveAddr,0);//w
	if (I2CFunc.MYI2C_Check (PORT)) MPU6050_I2C_ERROR(0);
	I2CFunc.MYI2C_Write8bit (PORT,writeAddr);
	if (I2CFunc.MYI2C_Check (PORT)) MPU6050_I2C_ERROR(1);
	I2CFunc.MYI2C_Write8bit (PORT,pBuffer);
	if (I2CFunc.MYI2C_Check (PORT)) MPU6050_I2C_ERROR(2);
	I2CFunc.MYI2C_GenerateSTOP (PORT);
	
	xTaskResumeAll ();
}
void MPU6050_I2C_BufferRead(u8 slaveAddr, uint8_t* pBuffer, u8 readAddr,uint8_t readtimes)
{
	uint8_t n=0;
	vTaskSuspendAll ();
	
	I2CFunc.MYI2C_GenerateSTART (PORT);
	I2CFunc.MYI2C_Send7bitAddress (PORT,slaveAddr,0);//w
	if (I2CFunc.MYI2C_Check (PORT)) MPU6050_I2C_ERROR(3);
	I2CFunc.MYI2C_Write8bit (PORT,readAddr);
	if (I2CFunc.MYI2C_Check (PORT)) MPU6050_I2C_ERROR(4);
	I2CFunc.MYI2C_GenerateSTOP (PORT);
	I2CFunc.MYI2C_GenerateSTART (PORT);
	I2CFunc.MYI2C_Send7bitAddress (PORT,slaveAddr,1);//r
	if (I2CFunc.MYI2C_Check (PORT)) MPU6050_I2C_ERROR(5);
	while (readtimes)
	{
		pBuffer[n++]=I2CFunc.MYI2C_Read8bit (PORT);
		if (--readtimes==0) {I2CFunc.MYI2C_Nack (PORT);break;};
		I2CFunc.MYI2C_Ack (PORT);
	}
	I2CFunc.MYI2C_GenerateSTOP (PORT);

	xTaskResumeAll ();
}
void MPU_6050_INIT ()
{
	PORT.sda=GPIO_Pin_7;
	PORT.scl=GPIO_Pin_6;
	PORT.GPIOx=GPIOB;
	I2CFunc.MYI2C_Init (PORT);
	MPU6050_I2C_ByteWrite(0xD0,0x00,MPU6050_RA_PWR_MGMT_1);      // reg107, 唤醒，8M内部时钟源
	MPU6050_I2C_ByteWrite(0xD0,0x07,MPU6050_RA_SMPLRT_DIV);         //采用频率 1000
	MPU6050_I2C_ByteWrite(0xD0,0x06,MPU6050_RA_CONFIG);                 
	MPU6050_I2C_ByteWrite(0xD0,0x01,MPU6050_RA_ACCEL_CONFIG);     //加速度量程 2g
	MPU6050_I2C_ByteWrite(0xD0,0x08,MPU6050_RA_GYRO_CONFIG);          //角速度量程 500度/s
}
/*系统接口函数*/
TESTPASS MPU_6050_TEST ()
{
	return PASS;
}
void Get_ACCEL (float* ACCEL)
{
	uint8_t  tmpBuffer[6],i;
	MPU6050_I2C_BufferRead(0xD0,tmpBuffer,MPU6050_RA_ACCEL_XOUT_H,6);
	for (i=0;i<3;i++)
	{
		ACCEL[i]=((float)((short)((tmpBuffer[i*2]<<8) | (tmpBuffer[(i*2)+1]))))/16384;
	}
}
void Get_GYRO (float* GYRO)
{
	uint8_t tmpBuffer[6],i;  
	MPU6050_I2C_BufferRead(0xD0,tmpBuffer,MPU6050_RA_GYRO_XOUT_H,6);
	for (i=0;i<3;i++)
	{
		GYRO[i]=((float)((short)((tmpBuffer[i*2]<<8) | (tmpBuffer[(i*2)+1]))))/65.5;
	}
}
void MPU_6050_LoadInfo (DriverInfoData* PDriverInfo)
{
	Gyroscopedata.Get_ACCEL=Get_ACCEL;
	Gyroscopedata.Get_GYRO=Get_GYRO;
	PDriverInfo->Init=MPU_6050_INIT;
	PDriverInfo->Test=MPU_6050_TEST;
	PDriverInfo->PInfo=&Gyroscopedata;
	PDriverInfo->DriverID=50;
}
