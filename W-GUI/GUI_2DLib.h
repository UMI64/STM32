#ifndef __GUI_2DLIB_H
#define __GUI_2DLIB_H

#include "GUI.h"
/*�ⲿ����*/
void GUI_2DLib_Clear (WDID ID,u16 COLOR);
void GUI_2DLib_DrawPoint (WDID ID,float x,float y,u16 POINT_COLOR);
/*�ڲ���Ϣ���ӵ���*/
void GUI_2DLib_MESS_Clear (void);
void GUI_2DLib_MESS_DrawPoint (void);

#endif
