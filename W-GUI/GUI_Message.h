#ifndef __GUI_MESSAGE_H
#define __GUI_MESSAGE_H

#include "GUI.h"

void GUI_MESS_MessageInit (void);
void GUI_MESS_ADD_A_Message (uint16_t Cmd,float Data);
float GUI_MESS_TakeOut_A_FloatData (void);
uint16_t GUI_MESS_TakeOut_A_Uint16Data (void);
void GUI_MESS_Handle_A_Message (void);

#endif