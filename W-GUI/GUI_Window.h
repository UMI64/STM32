#ifndef __GUI_WINDOW_H
#define __GUI_WINDOW_H

#include "GUI.h"
WDListFormat* GUI_WINDOW_GetInitedWDlistNode (void);
WDID GUI_WINDOW_CreatWD(WDFormat WD);
void GUI_WINDOW_DestroyWD (WDID ID);
void GUI_WINDOW_AddActiveRegion(WDID ID);

#endif
