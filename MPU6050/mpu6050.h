#ifndef __MPU6050_H
#define __MPU6050_H

#include <stm32f10x.h>
#include <FreeRTOS.h>
#include <task.h>
#include "SYS.h"
#include "mpu6050.h"
//#define 1101000
/*系统调用函数*/
void MPU_6050_LoadInfo (DriverInfoData* PDriverInfo);
#endif
