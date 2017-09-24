#ifndef __TFT_ILI9341_H
#define __TFT_ILI9341_H
#include <stm32f10x.h>
#include <FreeRTOS.h>
#include <task.h>
#include "SYS.h"
#include "tft_ili9341.h"

#define ID 0x0000
#define WIDTH (u16)240
#define LENGTH (u16)320
#define TESTCOLOR (u16)0xfcfc
#define COLOR_TYPE (u8)16
/*系统调用函数*/
void TFT_ILI9341_LoadInfo (DriverInfoData* PDriverInfo);
/*硬件基本功能*/
void TFT_ILI9341_WriteCmd (u8 Cmd);
void TFT_ILI9341_WriteData_8bit (u8 Data);
void TFT_ILI9341_WriteData_16bit (u16 Data);
void TFT_ILI9341_WriteDataContinue (u32 DataNum,u32 SENDBUFF);
void TFT_ILI9341_ReadData_8bit (uint8_t *Data);
void TFT_ILI9341_ReadData_16bit (u16 *Data);
void TFT_ILI9341_Address_set(uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2);
/*硬件高级功能*/
void TFT_ILI9341_Init (void);
void TFT_ILI9341_SoftRest (void);
void TFT_ILI9341_HardRest (void);
/*硬件绘图功能*/
void Screen_ReadColor (u16 x,u16 y,uint16_t *Color);
void Screen_Clear     (u16 x1,u16 y1,u16 x2,u16 y2,u16 Color);
void Screen_DrawPoint (u16 x,u16 y,u16 POINT_COLOR);

#endif
