#ifndef __DRIVER_H
#define __DRIVER_H

#include "SYS.h"
/*添加驱动头文件*/
#include "mpu6050.h"
/**/
typedef struct DriverFUNFormate
{
	DEV_State (* drv_DEV_OPEN)();
	DEV_State (* drv_DEV_CLOSE)();
	DEV_State (* drv_DEV_CONTROL)(DEV_HANDLE * Handle,uint16_t CMD,void * Buffer,uint32_t Buffersize);
} DriverFUNFormate;

typedef struct DriverREGFormate
{
	DriverFUNFormate FUN;
	struct DriverREGFormate * PNEXT;
} DriverREGFormate;

void SYS_DRI_RegisterDevice (char * DeviceName,DriverREGFormate * DriverREG);
void SYS_DRI_Init (void);
#endif
