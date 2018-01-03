#include "GUI_2DLib.h"
#include "math.h"
#include "GUI_MESSdef.h"
/*ÇåÆÁ*/
void GUI_2DLib_Clear (WDID ID,u16 COLOR)
{
	vTaskSuspendAll ();
	if (GUI_MESS_ADD_A_Message (CLEAN,NULL)==NULL) { xTaskResumeAll ();return;}
	if (GUI_MESS_ADD_A_Message (COLOR,NULL)==NULL) { xTaskResumeAll ();return;}
	xTaskResumeAll ();
	++MessageNum;
}
void GUI_2DLib_MESS_Clear (void)
{
	uint16_t Color;
	Color=GUI_MESS_TakeOut_A_Uint16Data ();
	PHardDrawFUN->DrawRange (0,0,HardInformation->width-1,HardInformation->length-1,Color);
	--MessageNum;
}
/*»­µã*/
void GUI_2DLib_DrawPoint (WDID ID,float x,float y,u16 POINT_COLOR)
{
	vTaskSuspendAll ();
	if (GUI_MESS_ADD_A_Message (DRAW_POINT,NULL)==NULL) { xTaskResumeAll ();return;}
	if (GUI_MESS_ADD_A_Message (NULL,x)==NULL) { xTaskResumeAll ();return;}
	if (GUI_MESS_ADD_A_Message (NULL,y)==NULL) { xTaskResumeAll ();return;}
	if (GUI_MESS_ADD_A_Message (POINT_COLOR,NULL)==NULL) { xTaskResumeAll ();return;}
	xTaskResumeAll ();
	++MessageNum;
}
void GUI_2DLib_MESS_DrawPoint (void)
{
	uint16_t x,y,POINT_COLOR;
	x=GUI_MESS_TakeOut_A_FloatData ();
	y=GUI_MESS_TakeOut_A_FloatData ();
	POINT_COLOR=GUI_MESS_TakeOut_A_Uint16Data ();
	PHardDrawFUN->DrawPoint ((uint16_t) x,(uint16_t)y,POINT_COLOR);
	--MessageNum;
}
/*»­Ïß*/
void drawline (float x0,float x1,float y0,float y1,uint16_t POINT_COLOR)
{ 
	float x,y,dx,dy,k,m; 
	dx = x1-x0;dy=y1-y0;
	if (fabs(dx)<1 && fabs(dy)<1)
	{
		PHardDrawFUN->DrawPoint (x0,y0,POINT_COLOR);
		return ;
	}
	if (fabs(dx)<1 || fabs(dy)>fabs(dx))
	{
		k=dx/dy; 
		if (y0>y1)
		{
			m=y0;
			y=y1;
			x=x1;
		}
		else
		{
			m=y1;
			y=y0;
			x=x0;
		}
		for (;y< m;y++)
		{
			PHardDrawFUN->DrawPoint (x,y,POINT_COLOR);
			x=x+k;
		}
	}
	else
	{
		k=dy/dx;
		if (x0>x1)
		{
			m=x0;
			x=x1;
			y=y1;
		}
		else
		{
			m=x1;
			x=x0;
			y=y0;
		}
		for (;x<m;x++)
		{
			PHardDrawFUN->DrawPoint (x,y,POINT_COLOR);
			y=y+k;
		}
	}
}
void GUI_2DLib_DrawLine (WDID ID,float x0,float y0,float x1,float y1,u16 POINT_COLOR)
{
	vTaskSuspendAll ();
	if (GUI_MESS_ADD_A_Message (DRAW_LINE,NULL)==NULL) { xTaskResumeAll ();return;}
	if (GUI_MESS_ADD_A_Message (NULL,x0)==NULL) { xTaskResumeAll ();return;}
	if (GUI_MESS_ADD_A_Message (NULL,y0)==NULL) { xTaskResumeAll ();return;}
	if (GUI_MESS_ADD_A_Message (NULL,x1)==NULL) { xTaskResumeAll ();return;}
	if (GUI_MESS_ADD_A_Message (NULL,y1)==NULL) { xTaskResumeAll ();return;}
	if (GUI_MESS_ADD_A_Message (POINT_COLOR,NULL)==NULL) { xTaskResumeAll ();return;}
	xTaskResumeAll ();
	++MessageNum;
}
void GUI_2DLib_MESS_DrawLine (void)
{
	uint16_t x0,y0,x1,y1,POINT_COLOR;
	x0=GUI_MESS_TakeOut_A_FloatData ();
	y0=GUI_MESS_TakeOut_A_FloatData ();
	x1=GUI_MESS_TakeOut_A_FloatData ();
	y1=GUI_MESS_TakeOut_A_FloatData ();
	POINT_COLOR=GUI_MESS_TakeOut_A_Uint16Data ();
	drawline (x0,x1,y0,y1,POINT_COLOR);
	--MessageNum;
}

