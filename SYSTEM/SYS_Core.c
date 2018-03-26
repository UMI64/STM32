#include "SYS_Memory.h"
#include "SYS_Driver&Device.h"

void SYS_Init(void)
{
	SYS_MemoryInit ();
	SYS_DRI_Init();
}
