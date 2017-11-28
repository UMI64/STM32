#ifndef __SYS_H
#define __SYS_H
/*C#*/
#include <stdlib.h>
/*stm32*/
#include <stm32f10x.h>

/*公共结构体*/
typedef enum {NotReady,GetReady} ReadyState; 



/*System*/
#include "SYS.h"
#include "SYS_CORE.h"
#include "SYS_Device.h"
#include "SYS_Memory.h"
#include "SYS_I2C.h"

#endif
