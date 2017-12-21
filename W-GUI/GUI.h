#ifndef __GUI_H
#define __GUI_H
#include <stm32f10x.h>
#include <FreeRTOS.h>
#include <task.h>
typedef uint8_t WDID;
/*硬件绘图函数*/
typedef struct HardDrawfunction
{
    void (* DrawPoint) (u16 x,u16 y,u16 POINT_COLOR);
		void (* DrawRange) (u16 x1,u16 y1,u16 x2,u16 y2,u16 Color);
		void (* ReadColor) (u16 x,u16 y,uint16_t *Color);
}HardDraw;
/*硬件信息*/
typedef struct HardInformationFormat
{
	u16 id;
	u16 width;
	u16 length;
	u8  color_type;
}HardInformationFormat;
/*GUI窗口信息格式*/
typedef struct WDFormat
{
	float Length;
	float Width;
	float XPosition;
	float YPosition;
}WDFormat;
/*GUI窗口信息链表格式*/
typedef struct WDListFormat
{
	struct WDListFormat * PFrontWD;
	struct WDListFormat * PNextWD;
	WDID ID;
	float Length;
	float Width;
	float XPosition;
	float YPosition;
}WDListFormat;
/*消息链表格式*/
typedef struct MESSListFormat
{
	struct MESSListFormat * PNext;
	uint16_t Data;
	float fData;
}MESSListFormat;
/*GUI内存ID*/
extern uint8_t GUIMENID;
/**/
extern HardInformationFormat * HardInformation;
extern HardDraw * PHardDrawFUN;
extern WDListFormat * WDList;
#include "SYS.h"
#include "GUI_Core.h"
#include "GUI_Window.h"
#include "GUI_Message.h"
#include "GUI_2DLib.h"
#endif
