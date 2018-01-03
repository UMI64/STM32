#include "mpu6050.h"
uint8_t flag;
float fix_gyro_zero[4]={-0.715f,-4.457f,-1.1961f,0.0f};
void MPU6050_I2C_ERROR (uint8_t f)
{
	flag=f;
}
void MPU6050_I2C_ByteWrite(I2CPORT PORT,u8 slaveAddr, u8 pBuffer, u8 writeAddr)
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
void MPU6050_I2C_BufferRead(I2CPORT PORT,u8 slaveAddr, uint8_t* pBuffer, u8 readAddr,uint8_t readtimes)
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

void Get_ACCEL (I2CPORT PORT,float* ACCEL)
{
	uint8_t  tmpBuffer[6],i;
	MPU6050_I2C_BufferRead(PORT,0xD0,tmpBuffer,MPU6050_RA_ACCEL_XOUT_H,6);
	for (i=0;i<3;i++)
	{
		ACCEL[i]=((float)((short)((tmpBuffer[i*2]<<8) | (tmpBuffer[(i*2)+1]))))/16384;
	}
}
void Get_GYRO (I2CPORT PORT,float* GYRO)
{
	uint8_t tmpBuffer[6],i;  
	MPU6050_I2C_BufferRead(PORT,0xD0,tmpBuffer,MPU6050_RA_GYRO_XOUT_H,6);
	for (i=0;i<3;i++)
	{
		GYRO[i]=(((float)((short)((tmpBuffer[i*2]<<8) | (tmpBuffer[(i*2)+1]))))/16.4f)-fix_gyro_zero[i];
	}
	if (fix_gyro_zero[3]>0.1f && GYRO[2]<0.3f && GYRO[2]>-0.3f) GYRO[2]=0.0f;
}
void Get_ACCEL_GYRO (I2CPORT PORT,float * ACCEL_GYRO)
{
	float A[3],G[3];
	Get_ACCEL (PORT,A);
	Get_GYRO (PORT,G);
	for (uint8_t i=0;i<3;i++)
	{
		ACCEL_GYRO[i]=A[i];
	}
		for (uint8_t i=0;i<3;i++)
	{
		ACCEL_GYRO[i+3]=G[i];
	}
	
}

void MPU_6050_FixZero(I2CPORT PORT,void (* TEMP)(I2CPORT PORT,float* temp),float* zerofixed)
{
	float temp[3];
	for (uint16_t i=0;i<2000;++i)
	{
		TEMP(PORT,temp);
		for (uint8_t j=0;j<3;++j)
		{
			zerofixed[j] += temp[j];
		}
	}
	zerofixed[3]=1.0f;
}
uint8_t MPU_6050_INIT (I2CPORT PORT)
{
	float temp[3]={0};
	I2CFunc.MYI2C_Init (PORT);
	MPU6050_I2C_ByteWrite(PORT,0xD0,0x00,MPU6050_RA_PWR_MGMT_1);      // reg107, 唤醒，8M内部时钟源
	MPU6050_I2C_ByteWrite(PORT,0xD0,0x07,MPU6050_RA_SMPLRT_DIV);         //采用频率 1000
	MPU6050_I2C_ByteWrite(PORT,0xD0,0x06,MPU6050_RA_CONFIG);                 
	MPU6050_I2C_ByteWrite(PORT,0xD0,0x01,MPU6050_RA_ACCEL_CONFIG);     //加速度量程 2g
	MPU6050_I2C_ByteWrite(PORT,0xD0,0x18,MPU6050_RA_GYRO_CONFIG);          //角速度量程 500度/s
	Get_ACCEL (PORT,temp);
	for (uint8_t i=0;temp[i]>0.1f || temp[i]<-0.1f;++i) 
	{
		for(uint16_t i=0;i<1000;i++);
		MPU_6050_FixZero (PORT,Get_GYRO,fix_gyro_zero);
		return 1;
	}
	return 0;
}
/*系统链接函数*/
/*****/
I2CPORT SetPort0()
{
	I2CPORT PORT0;
	PORT0.sda=GPIO_Pin_7;
	PORT0.scl=GPIO_Pin_6;
	PORT0.GPIOx=GPIOB;
	return PORT0;
}
DEV_State MPU_6050_OPEN_0 ()
{
	if (MPU_6050_INIT (SetPort0())==1) return DEV_SUCCESS;
	else return DEV_FAIL;
}
DEV_State MPU_6050_CLOSE_0 ()
{
	return DEV_SUCCESS;
}
/*****/
I2CPORT SetPort1()
{
	I2CPORT PORT1;
	PORT1.sda=GPIO_Pin_9;
	PORT1.scl=GPIO_Pin_8;
	PORT1.GPIOx=GPIOB;
	return PORT1;
}
DEV_State MPU_6050_OPEN_1 ()
{
	if (MPU_6050_INIT (SetPort1())==1) return DEV_SUCCESS;
	else return DEV_FAIL;
}
DEV_State MPU_6050_CLOSE_1 ()
{
	return DEV_SUCCESS;
}
/*****/
DEV_State MPU_6050_CONTROL_ALL (DEV_HANDLE * Handle,uint16_t CMD,void * Buffer,uint32_t Buffersize)
{
	I2CPORT PORT;
	SYS_DEV_ChangeState (Handle,DEV_BUSY);
	if (Handle->drv_DEV_OPEN==MPU_6050_OPEN_0) PORT=SetPort0 ();
	else PORT=SetPort1 ();
	switch (CMD)
	{
		case MPU6050_RA_TEMP: ;break;
		case MPU6050_RA_GYRO:  Get_GYRO (PORT,Buffer);break;
		case MPU6050_RA_ACCEL: Get_ACCEL (PORT,Buffer);break;
		case MPU6050_RA_ACCEL_GYRO: Get_ACCEL_GYRO (PORT,Buffer);break;
		case MPU6050_RA_ACCEL_GYRO_TEMP: ;break;
	}
	SYS_DEV_ChangeState (Handle,DEV_OPEN);
	return DEV_SUCCESS;
}
/*****/
void MPU_6050_REG ()
{
	char * DriveName;
	DriverREGFormate mpu6050REG_0,mpu6050REG_1;
	
	mpu6050REG_0.FUN.drv_DEV_OPEN=MPU_6050_OPEN_0;
	mpu6050REG_0.FUN.drv_DEV_CLOSE=MPU_6050_CLOSE_0;
	mpu6050REG_0.FUN.drv_DEV_CONTROL=MPU_6050_CONTROL_ALL;
	mpu6050REG_0.PNEXT=&mpu6050REG_1;
	
	mpu6050REG_1.FUN.drv_DEV_OPEN=MPU_6050_OPEN_1;
	mpu6050REG_1.FUN.drv_DEV_CLOSE=MPU_6050_CLOSE_1;
	mpu6050REG_1.FUN.drv_DEV_CONTROL=MPU_6050_CONTROL_ALL;
	mpu6050REG_1.PNEXT=NULL;
	DriveName="mpu6050";
	SYS_DRI_RegisterDevice (DriveName,&mpu6050REG_0);
}
