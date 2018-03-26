#include <stm32f10x.h>
#include <FreeRTOS.h>
#include <task.h>
#include "SYS.h"
#include <stable.h>

void Init ()
{
	SYS_Init();
	Stable_Init();
	vTaskDelete(NULL);
}
int main ()

{
	xTaskCreate(Init,"Init",150,NULL,4,NULL);
	vTaskStartScheduler();
}
