#include "tft_ili9341.h"
#include <stdlib.h> 
static LCDData LCDdata;
void SPI1_Init ()
{
	GPIO_InitTypeDef GPIO_SPI1_OUT = {GPIO_Pin_5 | GPIO_Pin_7,GPIO_Speed_50MHz,GPIO_Mode_AF_PP};
	GPIO_InitTypeDef GPIO_SPI1_IN = {GPIO_Pin_6,GPIO_Speed_50MHz,GPIO_Mode_IPU};
	GPIO_InitTypeDef GPIO_SPI1_DDX_REST_CS = {GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4,GPIO_Speed_50MHz,GPIO_Mode_Out_PP};
	SPI_InitTypeDef SPI_SPI1={SPI_Direction_2Lines_FullDuplex,SPI_Mode_Master,SPI_DataSize_16b,SPI_CPOL_Low,SPI_CPHA_1Edge,SPI_NSS_Soft,SPI_BaudRatePrescaler_2,SPI_FirstBit_MSB,7};
	NVIC_InitTypeDef SPI1_NVIC={SPI1_IRQn,1,1,ENABLE};
	NVIC_InitTypeDef DMA_SPI1TX_NVIC={DMA1_Channel3_IRQn,1,1,DISABLE};

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE);
	
	GPIO_Init(GPIOA,&GPIO_SPI1_OUT);
	GPIO_Init(GPIOA,&GPIO_SPI1_IN);
	GPIO_Init(GPIOA,&GPIO_SPI1_DDX_REST_CS);
	SPI_Init(SPI1,&SPI_SPI1);
	SPI_I2S_ITConfig(SPI1,SPI_I2S_IT_TXE,DISABLE);
	SPI_I2S_ITConfig(SPI1,SPI_I2S_IT_RXNE,DISABLE);
	SPI_Cmd(SPI1,ENABLE);
	SPI_I2S_DMACmd(SPI1,SPI_I2S_DMAReq_Tx,DISABLE);
	DMA_Cmd(DMA1_Channel3,DISABLE);
	DMA_ITConfig(DMA1_Channel3,DMA_IT_TC,DISABLE);
	NVIC_Init(&SPI1_NVIC);
	NVIC_Init(&DMA_SPI1TX_NVIC);
}
void DMA_TX_Init (u32 DataNum,u32 SENDBUFF)
{
	DMA_InitTypeDef SPI1_TX_DMA;
	SPI1_TX_DMA.DMA_PeripheralBaseAddr=(uint32_t) &(SPI1->DR);
	SPI1_TX_DMA.DMA_MemoryBaseAddr=(uint32_t) SENDBUFF;
	SPI1_TX_DMA.DMA_DIR=DMA_DIR_PeripheralDST;
	SPI1_TX_DMA.DMA_BufferSize=DataNum;
	SPI1_TX_DMA.DMA_PeripheralInc=DMA_PeripheralInc_Disable;
	SPI1_TX_DMA.DMA_MemoryInc=DMA_MemoryInc_Enable;
	SPI1_TX_DMA.DMA_PeripheralDataSize=DMA_PeripheralDataSize_HalfWord;
	SPI1_TX_DMA.DMA_MemoryDataSize=DMA_MemoryDataSize_HalfWord;
	SPI1_TX_DMA.DMA_Mode=DMA_Mode_Normal;
	SPI1_TX_DMA.DMA_Priority=DMA_Priority_VeryHigh;
	SPI1_TX_DMA.DMA_M2M=DMA_M2M_Disable;
	DMA_DeInit(DMA1_Channel3);
	DMA_Init(DMA1_Channel3,&SPI1_TX_DMA);
	DMA_ITConfig(DMA1_Channel3,DMA_IT_TC,ENABLE);
	SPI_I2S_DMACmd(SPI1,SPI_I2S_DMAReq_Tx,ENABLE);
	DMA_Cmd(DMA1_Channel3,ENABLE);
}
void SPI_BaudRatePrescaler_datasize(u16 BaudRatePrescaler,u16 datasize)
{
	u16 temp;
	while (!(SPI1->SR & SPI_I2S_FLAG_TXE) | (SPI1->SR & SPI_I2S_FLAG_BSY));//可能会在这里卡死
	temp=BaudRatePrescaler | datasize;
	SPI1->CR1 &=0Xf7c7;
	SPI1->CR1 |=temp;
}
void TFT_ILI9341_ReadData_8bit (u8 *Data)
{
	GPIO_SetBits(GPIOA,GPIO_Pin_3);//DATA
	SPI_BaudRatePrescaler_datasize(SPI_BaudRatePrescaler_8,SPI_DataSize_8b);//8bit读
	while (!(SPI1->SR & SPI_I2S_FLAG_TXE));
	SPI1->DR =0xff;
	while (!(SPI1->SR & SPI_I2S_FLAG_RXNE));
	*Data=SPI1->DR;
}
void TFT_ILI9341_ReadData_16bit (u16 *Data)
{
	GPIO_SetBits(GPIOA,GPIO_Pin_3);//DATA
	SPI_BaudRatePrescaler_datasize(SPI_BaudRatePrescaler_8,SPI_DataSize_16b);//16bit读
	*Data=SPI1->DR;//清除RXEN标志
	while (!(SPI1->SR & SPI_I2S_FLAG_TXE));
	SPI1->DR =0xffff;
	while (!(SPI1->SR & SPI_I2S_FLAG_RXNE));
	*Data=SPI1->DR;
}
void TFT_ILI9341_WriteCmd (u8 Cmd)
{
	GPIO_ResetBits(GPIOA,GPIO_Pin_3);//CMD
	SPI_BaudRatePrescaler_datasize(SPI_BaudRatePrescaler_2,SPI_DataSize_8b);//8bit写
	while (!(SPI1->SR & SPI_I2S_FLAG_TXE));
	SPI1->DR =Cmd;
}
void TFT_ILI9341_WriteData_8bit (u8 Data)
{
	GPIO_SetBits(GPIOA,GPIO_Pin_3);//DATA
	SPI_BaudRatePrescaler_datasize(SPI_BaudRatePrescaler_2,SPI_DataSize_8b);//8bit写
	while (!(SPI1->SR & SPI_I2S_FLAG_TXE));
	SPI1->DR =Data;
}
void TFT_ILI9341_WriteData_16bit (u16 Data)
{
	GPIO_SetBits(GPIOA,GPIO_Pin_3);//DATA
	SPI_BaudRatePrescaler_datasize(SPI_BaudRatePrescaler_2,SPI_DataSize_16b);//16bit写
	while (!(SPI1->SR & SPI_I2S_FLAG_TXE));
	SPI1->DR =Data;
}
void TFT_ILI9341_WriteDataContinue (u32 DataNum,u32 SENDBUFF)
{
	GPIO_ResetBits(GPIOA,GPIO_Pin_3);//CMD
	SPI_BaudRatePrescaler_datasize(SPI_BaudRatePrescaler_2,SPI_DataSize_8b);//8bit写
	while (!(SPI1->SR & SPI_I2S_FLAG_TXE));
	SPI1->DR = 0x2c;
	SPI_BaudRatePrescaler_datasize(SPI_BaudRatePrescaler_2,SPI_DataSize_16b);//16bit写
	GPIO_SetBits(GPIOA,GPIO_Pin_3);//DATA
	DMA_TX_Init (DataNum,SENDBUFF);
}
void TFT_ILI9341_Address_set(uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2)
{ 
	TFT_ILI9341_WriteCmd(0x2a);
	TFT_ILI9341_WriteData_8bit(x1>>8);
	TFT_ILI9341_WriteData_8bit(x1);
	TFT_ILI9341_WriteData_8bit(x2>>8);
	TFT_ILI9341_WriteData_8bit(x2);
  
	TFT_ILI9341_WriteCmd(0x2b);
	TFT_ILI9341_WriteData_8bit(y1>>8);
	TFT_ILI9341_WriteData_8bit(y1);
	TFT_ILI9341_WriteData_8bit(y2>>8);
	TFT_ILI9341_WriteData_8bit(y2);					 						 
}
void TFT_ILI9341_SoftRest (void)
{
	TFT_ILI9341_WriteCmd(0x01);
}
void TFT_ILI9341_HardRest (void)
{
	GPIO_SetBits(GPIOA,GPIO_Pin_4);//rest
	GPIO_ResetBits(GPIOA,GPIO_Pin_4);//rest
}
void TFT_ILI9341_Init ()
{
	SPI1_Init ();
	GPIO_ResetBits(GPIOA,GPIO_Pin_4);
	GPIO_SetBits(GPIOA,GPIO_Pin_6);//miso
	GPIO_ResetBits(GPIOA,GPIO_Pin_2);
	vTaskDelay(20);//延时20ms
	GPIO_SetBits(GPIOA,GPIO_Pin_2);
	vTaskDelay(20);//延时20ms
TFT_ILI9341_WriteCmd(0xCB);
    TFT_ILI9341_WriteData_8bit(0x39);
    TFT_ILI9341_WriteData_8bit(0x2C);
    TFT_ILI9341_WriteData_8bit(0x00);
    TFT_ILI9341_WriteData_8bit(0x34);
    TFT_ILI9341_WriteData_8bit(0x02);

TFT_ILI9341_WriteCmd(0xCF);  
    TFT_ILI9341_WriteData_8bit(0x00); 
    TFT_ILI9341_WriteData_8bit(0XC1); 
    TFT_ILI9341_WriteData_8bit(0X30); 
 	
TFT_ILI9341_WriteCmd(0xE8);  
    TFT_ILI9341_WriteData_8bit(0x85); 
    TFT_ILI9341_WriteData_8bit(0x00); 
    TFT_ILI9341_WriteData_8bit(0x78); 
 	
TFT_ILI9341_WriteCmd(0xEA);  
    TFT_ILI9341_WriteData_8bit(0x00); 
    TFT_ILI9341_WriteData_8bit(0x00); 
 	
TFT_ILI9341_WriteCmd(0xED);  
    TFT_ILI9341_WriteData_8bit(0x64); 
    TFT_ILI9341_WriteData_8bit(0x03); 
    TFT_ILI9341_WriteData_8bit(0X12); 
    TFT_ILI9341_WriteData_8bit(0X81); 

TFT_ILI9341_WriteCmd(0xF7);  
    TFT_ILI9341_WriteData_8bit(0x20); 
   
TFT_ILI9341_WriteCmd(0xC0);    //Power control 
    TFT_ILI9341_WriteData_8bit(0x23);   //VRH[5:0] 
   
TFT_ILI9341_WriteCmd(0xC1);    //Power control 
    TFT_ILI9341_WriteData_8bit(0x10);   //SAP[2:0];BT[3:0] 
 
TFT_ILI9341_WriteCmd(0xC5);    //VCM control 
    TFT_ILI9341_WriteData_8bit(0x3e); //对比度调节
    TFT_ILI9341_WriteData_8bit(0x28); 
 
TFT_ILI9341_WriteCmd(0xC7);    //VCM control2 
    TFT_ILI9341_WriteData_8bit(0x86);  //--
 
TFT_ILI9341_WriteCmd(0x36);    // Memory Access Control 
    TFT_ILI9341_WriteData_8bit(0x08); //	   //48 68竖屏//28 E8 横屏

TFT_ILI9341_WriteCmd(0x3A);    
    TFT_ILI9341_WriteData_8bit(0x55); 

TFT_ILI9341_WriteCmd(0xB1);    
    TFT_ILI9341_WriteData_8bit(0x00);  
    TFT_ILI9341_WriteData_8bit(0x18); 
 
TFT_ILI9341_WriteCmd(0xB6);    // Display Function Control 
    TFT_ILI9341_WriteData_8bit(0x08); 
    TFT_ILI9341_WriteData_8bit(0x82);
    TFT_ILI9341_WriteData_8bit(0x27);  
 	
TFT_ILI9341_WriteCmd(0xF2);    // 3Gamma Function Disable 
    TFT_ILI9341_WriteData_8bit(0x00); 
 	
TFT_ILI9341_WriteCmd(0x26);    //Gamma curve selected 
    TFT_ILI9341_WriteData_8bit(0x01); 
 
TFT_ILI9341_WriteCmd(0xE0);    //Set Gamma 
    TFT_ILI9341_WriteData_8bit(0x0F); 
    TFT_ILI9341_WriteData_8bit(0x31); 
    TFT_ILI9341_WriteData_8bit(0x2B); 
    TFT_ILI9341_WriteData_8bit(0x0C); 
    TFT_ILI9341_WriteData_8bit(0x0E); 
    TFT_ILI9341_WriteData_8bit(0x08); 
    TFT_ILI9341_WriteData_8bit(0x4E); 
    TFT_ILI9341_WriteData_8bit(0xF1); 
    TFT_ILI9341_WriteData_8bit(0x37); 
    TFT_ILI9341_WriteData_8bit(0x07); 
    TFT_ILI9341_WriteData_8bit(0x10); 
    TFT_ILI9341_WriteData_8bit(0x03); 
    TFT_ILI9341_WriteData_8bit(0x0E); 
    TFT_ILI9341_WriteData_8bit(0x09); 
    TFT_ILI9341_WriteData_8bit(0x00); 

TFT_ILI9341_WriteCmd(0XE1);    //Set Gamma 
    TFT_ILI9341_WriteData_8bit(0x00); 
    TFT_ILI9341_WriteData_8bit(0x0E); 
    TFT_ILI9341_WriteData_8bit(0x14); 
    TFT_ILI9341_WriteData_8bit(0x03); 
    TFT_ILI9341_WriteData_8bit(0x11); 
    TFT_ILI9341_WriteData_8bit(0x07); 
    TFT_ILI9341_WriteData_8bit(0x31); 
    TFT_ILI9341_WriteData_8bit(0xC1); 
    TFT_ILI9341_WriteData_8bit(0x48); 
    TFT_ILI9341_WriteData_8bit(0x08); 
    TFT_ILI9341_WriteData_8bit(0x0F); 
    TFT_ILI9341_WriteData_8bit(0x0C); 
    TFT_ILI9341_WriteData_8bit(0x31); 
    TFT_ILI9341_WriteData_8bit(0x36); 
    TFT_ILI9341_WriteData_8bit(0x0F); 
		
TFT_ILI9341_WriteCmd(0x2A);
    TFT_ILI9341_WriteData_8bit(0x00);
    TFT_ILI9341_WriteData_8bit(0x00);
    TFT_ILI9341_WriteData_8bit(0x00);
    TFT_ILI9341_WriteData_8bit(0xEF);
  
TFT_ILI9341_WriteCmd(0x2B);
    TFT_ILI9341_WriteData_8bit(0x00);
    TFT_ILI9341_WriteData_8bit(0x00);
    TFT_ILI9341_WriteData_8bit(0x01);
    TFT_ILI9341_WriteData_8bit(0x3F);
	
TFT_ILI9341_WriteCmd(0x11);    //退出睡眠

    vTaskDelay(120);           //延时120ms
			
TFT_ILI9341_WriteCmd(0x29);    //开显示
	
		Screen_Clear(0,0,WIDTH-1,LENGTH-1,0xffff);
}



/*系统接口函数*/
/*驱动加载函数和硬件测试函数*/
TESTPASS TFT_ILI9341_TEST()
{
	uint16_t TESTCOLOR,testcolor,tempcolor;
	static uint8_t x=0;
	vTaskSuspendAll ();
	
	if (x) x=0;
	else x=1;
	TESTCOLOR=x | 0xfcfc;
	
	xTaskResumeAll ();
	Screen_ReadColor (10,10,&tempcolor);
	Screen_DrawPoint(10,10,TESTCOLOR);
	Screen_ReadColor (10,10,&testcolor);
	Screen_DrawPoint(10,10,tempcolor);
	if (testcolor==TESTCOLOR) return PASS;
	else return NOTPASS;
}
void TFT_ILI9341_LoadInfo (DriverInfoData* PDriverInfo)
{
	LCDdata.DrawPoint=Screen_DrawPoint;
	LCDdata.DrawRange=Screen_Clear;
	LCDdata.ReadColor=Screen_ReadColor;
	PDriverInfo->Init=TFT_ILI9341_Init;
	PDriverInfo->Test=TFT_ILI9341_TEST;
	PDriverInfo->PInfo=&LCDdata;
	PDriverInfo->DriverID=22;
}
/*基本绘图函数*/
void Screen_Clear     (u16 x1,u16 y1,u16 x2,u16 y2,u16 Color)
{
	unsigned int i,j;
	vTaskSuspendAll ();
	TFT_ILI9341_Address_set(x1,y1,x2,y2);
	TFT_ILI9341_WriteCmd(0x2C);//w
	SPI_BaudRatePrescaler_datasize(SPI_BaudRatePrescaler_2,SPI_DataSize_16b);
	GPIO_SetBits(GPIOA,GPIO_Pin_3);//DATA
	for(i=y1;i<=y2;i++)
	{
		for (j=x1;j<=x2;j++)
		{
			while (!(SPI1->SR & SPI_I2S_FLAG_TXE) | (SPI1->SR & SPI_I2S_FLAG_BSY));
				SPI1->DR =Color;			 
		}
	}
	xTaskResumeAll ();
}
void Screen_DrawPoint(u16 x,u16 y,u16 POINT_COLOR)
{
	vTaskSuspendAll ();
	TFT_ILI9341_Address_set(x,y,x,y);//设置光标位置
	TFT_ILI9341_WriteCmd(0x2C);//w
	TFT_ILI9341_WriteData_16bit (POINT_COLOR);
	xTaskResumeAll ();
}
void Screen_ReadColor (u16 x,u16 y,uint16_t *Color)
{
	uint16_t r=0,g=0,b=0,trans_1=0,trans_2=0;
	vTaskSuspendAll ();
	TFT_ILI9341_Address_set(x,y,x,y);
	TFT_ILI9341_WriteCmd (0x2e);//r
	TFT_ILI9341_ReadData_16bit (&trans_1);
	TFT_ILI9341_ReadData_16bit (&trans_2);
	
				GPIO_SetBits(GPIOA,GPIO_Pin_4);//rest
				GPIO_ResetBits(GPIOA,GPIO_Pin_4);//rest
	
	r=(trans_1<<8)&0xf800;
	g=(trans_2>>5) & 0x07e0;
	b=(trans_2>>3) & 0x001f;
	*Color= r | g | b;
	xTaskResumeAll ();
}
