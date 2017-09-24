#ifndef __GUI_H
#define __GUI_H
#include "SYS.h"
#include "GUI_Core.h"
#include <stm32f10x.h>
#include <FreeRTOS.h>
#include <task.h>
/*Ó²¼þ»æÍ¼º¯Êý*/
typedef struct HardDrawfunction
{
    void (* DrawPoint) (u16 x,u16 y,u16 POINT_COLOR);
		void (* DrawRange) (u16 x1,u16 y1,u16 x2,u16 y2,u16 Color);
		void (* ReadColor) (u16 x,u16 y,uint16_t *Color);
}HardDraw;

#endif
