#ifndef __BRUSHLESSMOTOR_H
#define __BRUSHLESSMOTOR_H

#include <stm32f10x.h>
#include <FreeRTOS.h>
#include <task.h>
#include "SYS.h"
#include "BrushlessMotor.h"

/*系统调用函数*/
void BrushlessMotor_LoadInfo (DriverInfoData* PDriverInfo);
#endif
