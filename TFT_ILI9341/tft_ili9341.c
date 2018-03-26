#include "tft_ili9341.h"
#include <stdlib.h> 
void SPI1_Init ()
{
	GPIO_InitTypeDef GPIO_SPI1_OUT = {GPIO_Pin_5 | GPIO_Pin_7,GPIO_Speed_50MHz,GPIO_Mode_AF_PP};
	GPIO_InitTypeDef GPIO_SPI1_IN = {GPIO_Pin_6,GPIO_Speed_50MHz,GPIO_Mode_IPU};
	GPIO_InitTypeDef GPIO_SPI1_DDX_REST_CS = {GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4,GPIO_Speed_50MHz,GPIO_Mode_Out_PP};
	SPI_InitTypeDef SPI_SPI1={SPI_Direction_2Lines_FullDuplex,SPI_Mode_Master,SPI_DataSize_8b,SPI_CPOL_Low,SPI_CPHA_1Edge,SPI_NSS_Soft,SPI_BaudRatePrescaler_2,SPI_FirstBit_MSB,7};

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	
	GPIO_Init(GPIOA,&GPIO_SPI1_OUT);
	GPIO_Init(GPIOA,&GPIO_SPI1_IN);
	GPIO_Init(GPIOA,&GPIO_SPI1_DDX_REST_CS);
	SPI_Init(SPI1,&SPI_SPI1);
	SPI_I2S_ITConfig(SPI1,SPI_I2S_IT_TXE,DISABLE);
	SPI_I2S_ITConfig(SPI1,SPI_I2S_IT_RXNE,DISABLE);
	SPI_Cmd(SPI1,ENABLE);
}
void SPI_BaudRatePrescaler_datasize(u16 BaudRatePrescaler,u16 datasize)
{
	uint16_t temp;
	while (!(SPI1->SR & SPI_I2S_FLAG_TXE) | (SPI1->SR & SPI_I2S_FLAG_BSY));//可能会在这里卡死
	temp=BaudRatePrescaler | datasize;
	SPI1->CR1 &=0Xf7c7;
	SPI1->CR1 |=temp;
}
void TFT_ILI9341_ReadData_8bit (u8 *Data)
{
	GPIOA->BSRR = GPIO_Pin_3;//DATA
	SPI_BaudRatePrescaler_datasize(SPI_BaudRatePrescaler_8,SPI_DataSize_8b);//8bit读
	while (!(SPI1->SR & SPI_I2S_FLAG_TXE));
	SPI1->DR =0xff;
	while (!(SPI1->SR & SPI_I2S_FLAG_RXNE));
	*Data=SPI1->DR;
}
void TFT_ILI9341_ReadData_16bit (u16 *Data)
{
	GPIOA->BSRR = GPIO_Pin_3;//DATA
	*Data=SPI1->DR;//清除RXEN标志
	while (!(SPI1->SR & SPI_I2S_FLAG_TXE));
	SPI1->DR =0xffff;
	while (!(SPI1->SR & SPI_I2S_FLAG_RXNE));
	*Data=SPI1->DR;
}
void TFT_ILI9341_WriteCmd (u8 Cmd)
{
	GPIOA->BRR = GPIO_Pin_3;//CMD
	while (!(SPI1->SR & SPI_I2S_FLAG_TXE));
	SPI1->DR =Cmd;
	    vTaskDelay(1);           //延时
}
void TFT_ILI9341_WriteData_8bit (u8 Data)
{
	GPIOA->BSRR = GPIO_Pin_3;//DATA
	while (!(SPI1->SR & SPI_I2S_FLAG_TXE));
	SPI1->DR =Data;
	    vTaskDelay(1);           //延时
}
void TFT_ILI9341_WriteData_16bit (u16 Data)
{
	GPIOA->BSRR = GPIO_Pin_3;//DATA
	while (!(SPI1->SR & SPI_I2S_FLAG_TXE));
	SPI1->DR =Data;
		    vTaskDelay(1);           //延时
}
void TFT_ILI9341_Address_set(uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2)
{ 
	SPI_BaudRatePrescaler_datasize(SPI_BaudRatePrescaler_2,SPI_DataSize_8b);//8bit写
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
uint8_t TFT_ILI9341_Init ()
{
	static uint16_t InitColor=0x6666;
	static point_str point;
	static rectangle_str clearn;
	clearn.Xs=0;
	clearn.Ys=0;
	clearn.Xe=WIDTH-1;
	clearn.Ye=LENGTH-1;
	clearn.RECTANGLE_COLOR=InitColor;
	point.X=10;
	point.Y=10;
	point.POINT_COLOR=0;
	SPI1_Init ();
	SPI_BaudRatePrescaler_datasize(SPI_BaudRatePrescaler_2,SPI_DataSize_8b);//8bit写
	GPIO_ResetBits(GPIOA,GPIO_Pin_4);
	GPIO_SetBits(GPIOA,GPIO_Pin_6);//miso
	GPIO_ResetBits(GPIOA,GPIO_Pin_2);
	vTaskDelay(500);//延时20ms
	GPIO_SetBits(GPIOA,GPIO_Pin_2);
	vTaskDelay(50);//延时20ms
	
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
	
		Screen_Drawrectangle(&clearn);
		
		Screen_ReadPointColor (&point);//检测是否初始化成功
	
	if (point.POINT_COLOR==InitColor)	return 0;
	else return 1;
}
void TFT_ILI9341_SoftRest (void)
{
	SPI_BaudRatePrescaler_datasize(SPI_BaudRatePrescaler_2,SPI_DataSize_8b);//8bit写
	TFT_ILI9341_WriteCmd(0x01);
}
void TFT_ILI9341_HardRest (void)
{
	GPIO_SetBits(GPIOA,GPIO_Pin_4);//rest
	GPIO_ResetBits(GPIOA,GPIO_Pin_4);//rest
}
void Screen_DrawPoint(point_str *point)
{
	TFT_ILI9341_Address_set(point->X,point->Y,point->X,point->Y);//设置光标位置
	TFT_ILI9341_WriteCmd(0x2C);//w
	SPI_BaudRatePrescaler_datasize(SPI_BaudRatePrescaler_2,SPI_DataSize_16b);//16bit写
	TFT_ILI9341_WriteData_16bit (point->POINT_COLOR);
}
void Screen_Drawrectangle (rectangle_str *rectangle)
{
	unsigned int i,j;
	TFT_ILI9341_Address_set(rectangle->Xs,rectangle->Ys,rectangle->Xe,rectangle->Ye);
	TFT_ILI9341_WriteCmd(0x2C);//w
	SPI_BaudRatePrescaler_datasize(SPI_BaudRatePrescaler_2,SPI_DataSize_16b);
	GPIOA->BSRR = GPIO_Pin_3;//DATA
	for(i=rectangle->Ys;i<=rectangle->Ye;++i)
	{
		for (j=rectangle->Xs;j<=rectangle->Xe;++j)
		{
			while (!(SPI1->SR & SPI_I2S_FLAG_TXE) | (SPI1->SR & SPI_I2S_FLAG_BSY));
				SPI1->DR =rectangle->RECTANGLE_COLOR;			 
		}
	}
}
void Screen_ReadPointColor (point_str *point)
{
	uint16_t r=0,g=0,b=0,trans_1=0,trans_2=0;
	TFT_ILI9341_Address_set(point->X,point->Y,point->X,point->Y);
	TFT_ILI9341_WriteCmd (0x2e);//r
	SPI_BaudRatePrescaler_datasize(SPI_BaudRatePrescaler_8,SPI_DataSize_16b);//16bit读
	TFT_ILI9341_ReadData_16bit (&trans_1);
	TFT_ILI9341_ReadData_16bit (&trans_2);
	
				GPIO_SetBits(GPIOA,GPIO_Pin_4);//rest
				GPIO_ResetBits(GPIOA,GPIO_Pin_4);//rest
	
	r=(trans_1<<8)&0xf800;
	g=(trans_2>>5) & 0x07e0;
	b=(trans_2>>3) & 0x001f;
	point->POINT_COLOR= r | g | b;
}
DEV_State TFT_ILI9341_OPEN ()
{
	if (TFT_ILI9341_Init ()) return DEV_FAIL;
	else return DEV_SUCCESS;
}
DEV_State TFT_ILI9341_CLOSE ()
{
	return DEV_SUCCESS;
}
DEV_State TFT_ILI9341_CONTROL (DEV_HANDLE * Handle,uint16_t CMD,void * Buffer,uint32_t Buffersize)
{
	DEV_State state;
	state=SYS_DEV_GetState (Handle);
	if (state==DEV_FAIL || state==DEV_BUSY || state==DEV_CLOSE) return state;
	SYS_DEV_ChangeState (Handle,DEV_BUSY);
	switch (CMD)
	{
		case ILI9341_DRAW_POINT: 					Screen_DrawPoint((point_str *)Buffer)			;break;
		case ILI9341_DRAW_RECTANGLE:  		Screen_Drawrectangle ((rectangle_str *)Buffer)				;break;
		case ILI9341_READ_POINT: 					Screen_ReadPointColor ((point_str *)Buffer)	;break;
	}
	SYS_DEV_ChangeState (Handle,DEV_OPEN);
	return DEV_SUCCESS;
}
DEV_HANDLE * TFT_ILI9341_REG_DEV (DeviceNeedFormate * DriverNeed)
{
	DeviceREGFormate DeviceREG;
	DeviceREG.DeviceID=1;
	DeviceREG.DeviceType="screen";
	DeviceREG.FUN.drv_DEV_CLOSE=TFT_ILI9341_CLOSE;
	DeviceREG.FUN.drv_DEV_OPEN=TFT_ILI9341_OPEN;
	DeviceREG.FUN.drv_DEV_CONTROL=TFT_ILI9341_CONTROL;
	DeviceREG.Needs=DriverNeed;
	return SYS_DRI_RegisterDevice (DeviceREG);
}
void TFT_ILI9341_REG ()
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
	
	DriverREG.DriverName="ili9341";
	DriverREG.Need=&GPIOPort;
	DriverREG.drv_RegisterDevice=( * TFT_ILI9341_REG_DEV);
	SYS_DRI_RegisterDriver (DriverREG);
}
