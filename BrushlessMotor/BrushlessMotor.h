#ifndef __BRUSHLESSMOTOR_H
#define __BRUSHLESSMOTOR_H

#include <stm32f10x.h>
#include <FreeRTOS.h>
#include <task.h>
#include "SYS.h"
#include "BrushlessMotor.h"

/*ϵͳ���ú���*/
void BrushlessMotor_LoadInfo (DriverInfoData* PDriverInfo);
#endif
