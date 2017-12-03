#include "GUI_2DLib.h"
void GUI_ActiveClear (WDID ID,u16 COLOR)
{
	PHardDrawFUN->DrawRange (0,0,HardInformation->width-1,HardInformation->length-1,0xffff);
}
void GUI_DrawPoint (WDID ID,float x,float y,u16 POINT_COLOR)
{
	//WDList
	PHardDrawFUN->DrawPoint ((uint16_t) x,(uint16_t)y,POINT_COLOR);
}
