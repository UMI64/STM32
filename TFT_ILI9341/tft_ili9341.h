#ifndef __TFT_ILI9341_H
#define __TFT_ILI9341_H
#include "SYS.h"
#include "tft_ili9341.h"

#define ID 0x0000
#define WIDTH (u16)240
#define LENGTH (u16)320
#define COLOR_TYPE (u8)16
/*指令*/
#define ILI9341_DRAW_POINT 0x00
#define ILI9341_DRAW_RECTANGLE 0x01
#define ILI9341_READ_POINT 0x02
/*数据结构*/
typedef struct point_str
{
	uint16_t X;
	uint16_t Y;
	uint16_t POINT_COLOR;
}point_str;
typedef struct rectangle_str
{
	uint16_t Xs;
	uint16_t Ys;
	uint16_t Xe;
	uint16_t Ye;
	uint16_t RECTANGLE_COLOR;
}rectangle_str;
/*硬件基本功能*/
void TFT_ILI9341_WriteCmd (u8 Cmd);
void TFT_ILI9341_WriteData_8bit (u8 Data);
void TFT_ILI9341_WriteData_16bit (u16 Data);
void TFT_ILI9341_WriteDataContinue (u32 DataNum,u32 SENDBUFF);
void TFT_ILI9341_ReadData_8bit (uint8_t *Data);
void TFT_ILI9341_ReadData_16bit (u16 *Data);
void TFT_ILI9341_Address_set(uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2);
/*硬件高级功能*/
uint8_t TFT_ILI9341_Init (void);
void TFT_ILI9341_SoftRest (void);
void TFT_ILI9341_HardRest (void);
/*硬件绘图功能*/
void Screen_DrawPoint(point_str * point);
void Screen_Drawrectangle (rectangle_str * rectangle);
void Screen_ReadPointColor (point_str * point);

/*系统调用函数*/
void TFT_ILI9341_REG (void);

void SPI1_Init (void);
#endif
