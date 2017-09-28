#include "mpu6050.h"
static GyroscopeData Gyroscopedata;
void I2C1_Init ()
{
	I2C_InitTypeDef I2C1_InitData;
	I2C1_InitData.I2C_ClockSpeed=400000;
	I2C1_InitData.I2C_Mode=I2C_Mode_I2C;
	I2C1_InitData.I2C_Ack=I2C_Ack_Enable;
	I2C1_InitData.I2C_AcknowledgedAddress=I2C_AcknowledgedAddress_7bit;
	I2C1_InitData.I2C_DutyCycle=I2C_DutyCycle_2;
	I2C1_InitData.I2C_OwnAddress1=0x00;
	I2C_Init (I2C1,&I2C1_InitData);
	I2C_Cmd(I2C1, ENABLE);
}
void RCC_I2C1_Init ()
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1 , ENABLE);
}
void RCC_GPIO_I2C1_Init ()
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
}
void GPIO_I2C1_Init ()
{
	GPIO_InitTypeDef GPIO_InitData;
	GPIO_InitData.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitData.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitData.GPIO_Mode = GPIO_Mode_AF_OD;
	GPIO_Init(GPIOB, &GPIO_InitData);
}
void MPU6050_I2C_ByteWrite(u8 slaveAddr, u8 pBuffer, u8 writeAddr)
{
  /* Send START condition */
  I2C_GenerateSTART(I2C1, ENABLE);          //发送开始信号
  /* Test on EV5 and clear it */
  while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));

  /* Send MPU6050 address for write */
  I2C_Send7bitAddress(I2C1, slaveAddr, I2C_Direction_Transmitter);          // 发送 MPU6050 地址、状态（写）
  /* Test on EV6 and clear it */
  while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

  /* Send the MPU6050's internal address to write to */
  I2C_SendData(I2C1, writeAddr);                   //发送 MPU6050内部某个待写寄存器地址
  /* Test on EV8 and clear it */
  while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

  /* Send the byte to be written */
  I2C_SendData(I2C1, pBuffer);                     //发送要写入的内容
  /* Test on EV8 and clear it */
  while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

  /* Send STOP condition */
  I2C_GenerateSTOP(I2C1, ENABLE);          //发送结束信号
}
void MPU6050_I2C_BufferRead(u8 slaveAddr, u8* pBuffer, u8 readAddr, u16 NumByteToRead)
{
  /* While the bus is busy */  
  while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY));

  /* Send START condition */
  I2C_GenerateSTART(I2C1, ENABLE);
  /* Test on EV5 and clear it */
  while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));

  /* Send MPU6050 address for write */
  I2C_Send7bitAddress(I2C1, slaveAddr, I2C_Direction_Transmitter); 
  /* Test on EV6 and clear it */
  while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

  /* Clear EV6 by setting again the PE bit */
  I2C_Cmd(I2C1, ENABLE);
  /* Send the MPU6050's internal address to write to */
  I2C_SendData(I2C1, readAddr);
  /* Test on EV8 and clear it */
  while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

/* Send STRAT condition a second time */
  I2C_GenerateSTART(I2C1, ENABLE);
  /* Test on EV5 and clear it */
  while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));

  /* Send MPU6050 address for read */
  I2C_Send7bitAddress(I2C1, slaveAddr, I2C_Direction_Receiver);
  /* Test on EV6 and clear it */
  while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));

  /* While there is data to be read */
  while(NumByteToRead)
  {
    if(NumByteToRead == 1)
    {
      /* Disable Acknowledgement */
      I2C_AcknowledgeConfig(I2C1, DISABLE);

      /* Send STOP Condition */
      I2C_GenerateSTOP(I2C1, ENABLE);
    }

    /* Test on EV7 and clear it */
    if(I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED))
    {
      /* Read a byte from the MPU6050 */
      *pBuffer = I2C_ReceiveData(I2C1);

      /* Point to the next location where the byte read will be saved */
      pBuffer++;

      /* Decrement the read bytes counter */
      NumByteToRead--;
    }
  }

  /* Enable Acknowledgement to be ready for another reception */
  I2C_AcknowledgeConfig(I2C1, ENABLE);
}
void MPU_6050_INIT ()
{
	RCC_I2C1_Init ();
	RCC_GPIO_I2C1_Init ();
	GPIO_I2C1_Init ();
	I2C1_Init ();
//	MPU6050_I2C_ByteWrite(0xd0,0x00,MPU6050_RA_PWR_MGMT_1);      // reg107, 唤醒，8M内部时钟源
//	MPU6050_I2C_ByteWrite(0xd0,0x07,MPU6050_RA_SMPLRT_DIV);         //采用频率 1000
//	MPU6050_I2C_ByteWrite(0xd0,0x06,MPU6050_RA_CONFIG);                 
//	MPU6050_I2C_ByteWrite(0xd0,0x01,MPU6050_RA_ACCEL_CONFIG);     //加速度量程 2g
//	MPU6050_I2C_ByteWrite(0xd0,0x18,MPU6050_RA_GYRO_CONFIG);          //角速度量程 2000度/s
}
void MPU6050_GetRawAccelGyro(s16* AccelGyro) 
{
    u8 tmpBuffer[14],i; 
    MPU6050_I2C_BufferRead(0xd0, tmpBuffer, MPU6050_RA_ACCEL_XOUT_H, 14); 
    /* Get acceleration */
    for(i=0; i<3; i++) 
      AccelGyro[i]=((s16)((u16)tmpBuffer[2*i] << 8) + tmpBuffer[2*i+1]);
   /* Get Angular rate */
    for(i=4; i<7; i++)
      AccelGyro[i-1]=((s16)((u16)tmpBuffer[2*i] << 8) + tmpBuffer[2*i+1]);        

}
/*系统接口函数*/
TESTPASS MPU_6050_TEST ()
{
	return PASS;
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
	PDriverInfo->Init=MPU_6050_INIT;
	PDriverInfo->Test=MPU_6050_TEST;
	PDriverInfo->PInfo=&Gyroscopedata;
	PDriverInfo->DriverID=50;
}
