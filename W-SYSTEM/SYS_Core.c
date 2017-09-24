#include "SYS_CORE.h"

void SYS_Init(void)
{
	SYS_MemoryInit ();
	SYS_DriverInit();

}
