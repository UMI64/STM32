#ifndef __GUI_2DLIB_H
#define __GUI_2DLIB_H

#include "GUI.h"
/*�ⲿ����*/
void GUI_2DLib_Clear (WDID ID,u16 COLOR);
void GUI_2DLib_DrawPoint (WDID ID,float x,float y,u16 POINT_COLOR);
void GUI_2DLib_DrawLine (WDID ID,float x0,float y0,float x1,float y1,u16 POINT_COLOR);
/*�ڲ���Ϣ���ӵ���*/
void GUI_2DLib_MESS_Clear (void);
void GUI_2DLib_MESS_DrawPoint (void);
void GUI_2DLib_MESS_DrawLine (void);


#endif
