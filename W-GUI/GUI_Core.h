#ifndef __GUI_CORE_H
#define __GUI_CORE_H

#include "GUI.h"

/**/
void GUI_Init (void);
WDListFormat* GUI_GetInitedWDlistNode (void);
WDID GUI_CreatWD(WDFormat WD);
void GUI (void);

#endif
