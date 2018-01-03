#ifndef __SYS_H
#define __SYS_H
/*C#*/
#include <stdlib.h>
/*stm32*/
#include <stm32f10x.h>
/*设定头文件*/
#include <SYS_Config.h>

/*公共结构体*/
typedef enum {NotReady,GetReady} ReadyState; 

/*Driver*//*Device*/
/*状态枚举类型*/
typedef enum
{
	DEV_FAIL=0,
	DEV_SUCCESS=1,
	DEV_OPEN=2,
	DEV_CLOSE=3,
	DEV_BUSY=4
} DEV_State;
/*设备句柄*/
typedef struct DEV_HANDLE
{
	void * DevicePointer;
	DEV_State (* drv_DEV_OPEN)();
	DEV_State (* drv_DEV_CLOSE)();
	DEV_State (* drv_DEV_CONTROL)(struct DEV_HANDLE * Handle,uint16_t CMD,void * Buffer,uint32_t Buffersize);
} DEV_HANDLE;
/**/

/*System*/
#include "SYS.h"
#include "SYS_CORE.h"
#include "SYS_Memory.h"
#include "SYS_Device.h"
#include "SYS_Driver.h"
#include "SYS_I2C.h"
/*GUI*/
#include "GUI_Core.h"

#endif
