#include "GUI_2DLib.h"
#include "GUI_MESSdef.h"
/*ÇåÆÁ*/
void GUI_2DLib_Clear (WDID ID,u16 COLOR)
{
	vTaskSuspendAll ();
	GUI_MESS_ADD_A_Message (CLEAN,NULL);
	GUI_MESS_ADD_A_Message (COLOR,NULL);
	xTaskResumeAll ();
}
void GUI_2DLib_MESS_Clear (void)
{
	PHardDrawFUN->DrawRange (0,0,HardInformation->width-1,HardInformation->length-1,GUI_MESS_TakeOut_A_Uint16Data ());
}
/*»­µã*/
void GUI_2DLib_DrawPoint (WDID ID,float x,float y,u16 POINT_COLOR)
{
	vTaskSuspendAll ();
	GUI_MESS_ADD_A_Message (DRAW_POINT,NULL);
	GUI_MESS_ADD_A_Message (NULL,x);
	GUI_MESS_ADD_A_Message (NULL,y);
	GUI_MESS_ADD_A_Message (POINT_COLOR,NULL);
	xTaskResumeAll ();
}
void GUI_2DLib_MESS_DrawPoint (void)
{
	uint16_t x,y,POINT_COLOR;
	x=GUI_MESS_TakeOut_A_FloatData ();
	y=GUI_MESS_TakeOut_A_FloatData ();
	POINT_COLOR=GUI_MESS_TakeOut_A_Uint16Data ();
	PHardDrawFUN->DrawPoint ((uint16_t) x,(uint16_t)y,POINT_COLOR);
}
