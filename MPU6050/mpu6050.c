#include "mpu6050.h"
uint8_t flag;
float fix_gyro_zero[3]={0};
void MPU6050_I2C_ERROR (uint8_t f)
{
	flag=f;
}
uint8_t MPU6050_I2C_ByteWrite(I2CPORT PORT,u8 slaveAddr, u8 pBuffer, u8 writeAddr)
{
	uint8_t MAXerrortime=5;
	vTaskSuspendAll ();
	/**/
	do
	{
		I2CFunc.MYI2C_GenerateSTART (PORT);
		I2CFunc.MYI2C_Send7bitAddress (PORT,slaveAddr,0);//w
		if (I2CFunc.MYI2C_Check (PORT)) {MPU6050_I2C_ERROR(0);I2CFunc.MYI2C_GenerateSTOP (PORT);continue;}
		I2CFunc.MYI2C_Write8bit (PORT,writeAddr);
		if (I2CFunc.MYI2C_Check (PORT)) {MPU6050_I2C_ERROR(1);I2CFunc.MYI2C_GenerateSTOP (PORT);continue;}
		I2CFunc.MYI2C_Write8bit (PORT,pBuffer);
		if (I2CFunc.MYI2C_Check (PORT)) {MPU6050_I2C_ERROR(2);I2CFunc.MYI2C_GenerateSTOP (PORT);continue;}
		I2CFunc.MYI2C_GenerateSTOP (PORT);
		/**/
		xTaskResumeAll ();
		return 0;
	}
	while (MAXerrortime--);
	/**/
	xTaskResumeAll ();
	return 1;
}
uint8_t MPU6050_I2C_BufferRead(I2CPORT PORT,u8 slaveAddr, uint8_t* pBuffer, u8 readAddr,uint8_t readtimes)
{
	uint8_t n=0,MAXerrortime=5;
	vTaskSuspendAll ();
	/**/
	do
	{
		I2CFunc.MYI2C_GenerateSTART (PORT);
		I2CFunc.MYI2C_Send7bitAddress (PORT,slaveAddr,0);//w
		if (I2CFunc.MYI2C_Check (PORT)) {MPU6050_I2C_ERROR(3);I2CFunc.MYI2C_GenerateSTOP (PORT);continue;}
		I2CFunc.MYI2C_Write8bit (PORT,readAddr);
		if (I2CFunc.MYI2C_Check (PORT)) {MPU6050_I2C_ERROR(4);I2CFunc.MYI2C_GenerateSTOP (PORT);continue;}
		I2CFunc.MYI2C_GenerateSTOP (PORT);
		I2CFunc.MYI2C_GenerateSTART (PORT);
		I2CFunc.MYI2C_Send7bitAddress (PORT,slaveAddr,1);//r
		if (I2CFunc.MYI2C_Check (PORT)) {MPU6050_I2C_ERROR(5);I2CFunc.MYI2C_GenerateSTOP (PORT);continue;}
		while (readtimes)
		{	
			pBuffer[n++]=I2CFunc.MYI2C_Read8bit (PORT);
			if (--readtimes==0) {I2CFunc.MYI2C_Nack (PORT);break;};
			I2CFunc.MYI2C_Ack (PORT);
		}
		I2CFunc.MYI2C_GenerateSTOP (PORT);
		/**/
		xTaskResumeAll ();
		return 0;
	}
	while (MAXerrortime--);
	/**/
	xTaskResumeAll ();
	return 1;
}
uint8_t Get_ACCEL (I2CPORT PORT,float* ACCEL)
{
	uint8_t  tmpBuffer[6],i;
	if (MPU6050_I2C_BufferRead(PORT,0xD0,tmpBuffer,MPU6050_RA_ACCEL_XOUT_H,6)) return 1;
	for (i=0;i<3;i++)
	{
		ACCEL[i]=((float)((short)((tmpBuffer[i*2]<<8) | (tmpBuffer[(i*2)+1]))))/16384;
	}
	return 0;
}
uint8_t Get_GYRO (I2CPORT PORT,float* GYRO)
{
	uint8_t tmpBuffer[6],i;  
	if (MPU6050_I2C_BufferRead(PORT,0xD0,tmpBuffer,MPU6050_RA_GYRO_XOUT_H,6)) return 1;
	for (i=0;i<3;i++)
	{
		GYRO[i]=(((float)((short)((tmpBuffer[i*2]<<8) | (tmpBuffer[(i*2)+1]))))/16.4f)-fix_gyro_zero[i];
	}
	if (GYRO[2]<0.3f && GYRO[2]>-0.3f) GYRO[2]=0.0f;
	return 0;
}
uint8_t Get_ACCEL_GYRO (I2CPORT PORT,float * ACCEL_GYRO)
{
	float A[3],G[3];
	if (Get_ACCEL (PORT,A)&&Get_GYRO (PORT,G)) return 1;
	for (uint8_t i=0;i<3;i++)
	{
		ACCEL_GYRO[i]=A[i];
	}
		for (uint8_t i=0;i<3;i++)
	{
		ACCEL_GYRO[i+3]=G[i];
	}
	return 0;
}
/*角加速度归零值*/
void MPU_6050_FixZero(I2CPORT PORT,uint8_t (* TEMP)(I2CPORT PORT,float* temp),float* zerofixed)
{
	uint8_t tmpBuffer[6];
	while(1)
	{
		for (uint16_t j=0;j<100;++j)//采样角加速度100次
		{ 
			MPU6050_I2C_BufferRead(PORT,0xD0,tmpBuffer,MPU6050_RA_GYRO_XOUT_H,6);
			for (uint8_t i=0;i<3;i++)
			{
				zerofixed[i] +=((float)((short)((tmpBuffer[i*2]<<8) | (tmpBuffer[(i*2)+1]))))/16.4f;
			}
		}
		for (uint8_t j=0;j<3;++j)
		{
			zerofixed[j] /= 100;
		}
		if (fabs(zerofixed[0])<3.0f && fabs(zerofixed[1])<5.0f && fabs(zerofixed[2])<1.8f) break;//防止角加速度归零值没有被干扰
	}
}
uint8_t MPU_6050_INIT (I2CPORT PORT)
{
	uint8_t MAXerrorTime=5;
	I2CFunc.MYI2C_Init (PORT);
	do
	{
		if (MPU6050_I2C_ByteWrite(PORT,0xD0,0x00,MPU6050_RA_PWR_MGMT_1)) continue;      // reg107, 唤醒，8M内部时钟源
		if (MPU6050_I2C_ByteWrite(PORT,0xD0,0x07,MPU6050_RA_SMPLRT_DIV)) continue;         //采用频率 1000
		if (MPU6050_I2C_ByteWrite(PORT,0xD0,0x06,MPU6050_RA_CONFIG)) continue;                 
		if (MPU6050_I2C_ByteWrite(PORT,0xD0,0x01,MPU6050_RA_ACCEL_CONFIG)) continue;     //加速度量程 2g
		if (MPU6050_I2C_ByteWrite(PORT,0xD0,0x18,MPU6050_RA_GYRO_CONFIG)) continue;          //角速度量程 2000度/s
		for(uint16_t i=0;i<1000;i++);//延时
		MPU_6050_FixZero (PORT,Get_GYRO,fix_gyro_zero);
		return 0;
	}
	while (MAXerrorTime--);//当最大错误数累计到0时返回错误
	return 1;
}
/*系统链接函数*/
/*****/
DEV_State MPU_6050_OPEN (DEV_HANDLE * Handle)
{
	I2CPORT PORT;
	PORT.GPIOx=(void *)Handle->DriverNeed->Data;
	PORT.sda=Handle->DriverNeed->NextNeed->Data;
	PORT.scl=Handle->DriverNeed->NextNeed->NextNeed->Data;
	if (MPU_6050_INIT (PORT)) return DEV_FAIL;
	else return DEV_SUCCESS;
}
DEV_State MPU_6050_CLOSE ()
{
	return DEV_SUCCESS;
}
/*****/
DEV_State MPU_6050_CONTROL (DEV_HANDLE * Handle,uint16_t CMD,void * Buffer,uint32_t Buffersize)
{
	uint8_t error=0;
	I2CPORT PORT;
	DEV_State state;
	state=SYS_DEV_GetState (Handle);
	if (state==DEV_FAIL || state==DEV_BUSY || state==DEV_CLOSE) return state;
	SYS_DEV_ChangeState (Handle,DEV_BUSY);
	
	PORT.GPIOx=(void *)Handle->DriverNeed->Data;
	PORT.sda=Handle->DriverNeed->NextNeed->Data;
	PORT.scl=Handle->DriverNeed->NextNeed->NextNeed->Data;
	
	switch (CMD)
	{
		case MPU6050_RA_TEMP: 																								;break;
		case MPU6050_RA_GYRO:  						error=+Get_GYRO (PORT,Buffer)				;break;
		case MPU6050_RA_ACCEL: 						error=+Get_ACCEL (PORT,Buffer)			;break;
		case MPU6050_RA_ACCEL_GYRO: 			error=+Get_ACCEL_GYRO (PORT,Buffer)	;break;
		case MPU6050_RA_ACCEL_GYRO_TEMP: 																			;break;
	}
	SYS_DEV_ChangeState (Handle,DEV_OPEN);
	return DEV_SUCCESS;
}
DEV_HANDLE *  RegisterDevice (DeviceNeedFormate * DriverNeed)
{
	DeviceREGFormate DeviceREG;
	DeviceREG.DeviceID=1;
	DeviceREG.DeviceType="senser";
	DeviceREG.FUN.drv_DEV_CLOSE=MPU_6050_CLOSE;
	DeviceREG.FUN.drv_DEV_OPEN=MPU_6050_OPEN;
	DeviceREG.FUN.drv_DEV_CONTROL=MPU_6050_CONTROL;
	DeviceREG.Needs=DriverNeed;
	return SYS_DRI_RegisterDevice (DeviceREG);
}
/*****/
void MPU_6050_REG ()
{
	DriverREGFormate DriverREG;
	static DeviceNeedFormate GPIOPort,SDAGPIO,SCLGPIO;
	
	GPIOPort.NeedName="GPIOPort";
	GPIOPort.Data=NULL;
	GPIOPort.NextNeed=&SDAGPIO;
	
	GPIOPort.NeedName="SDAGPIO";
	GPIOPort.Data=NULL;
	GPIOPort.NextNeed=&SCLGPIO;
	
	GPIOPort.NeedName="SCLGPIO";
	GPIOPort.Data=NULL;
	GPIOPort.NextNeed=NULL;
	
	DriverREG.DriverName="mpu6050";
	DriverREG.Need=&GPIOPort;
	DriverREG.drv_RegisterDevice=(*RegisterDevice);
	SYS_DRI_RegisterDriver (DriverREG);
}
