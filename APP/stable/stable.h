#ifndef __STABLE_H
#define __STABLE_H

#include <stm32f10x.h>
#include <FreeRTOS.h>
#include <task.h>
#include "SYS.h"
#include "stable.h"



/*内部调用*/
void stable (void);
void Stable_Init (void);
void IMUupdate(float * ABuff,float * GBuff);

#endif
