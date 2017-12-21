#include <stm32f10x.h>
#include <FreeRTOS.h>
#include <task.h>
#include "SYS.h"
#include "GUI.h"
#include <stable.h>

void tesst ()
{
	uint8_t MENID;
	uint32_t i=0;
	MENID=SYS_CallHEAP (1024);
	while (1) 
	{
		SYS_CallMem (50,MENID);
		++i;
		while (i>1000);
	}
}
void Init ()
{
	SYS_Init();
	//xTaskCreate(tesst,"tesst",50,NULL,4,NULL);
	GUI_CORE_Init();
	Stable_Init();
	vTaskDelete(NULL);
}
int main ()
{
	xTaskCreate(Init,"Init",50,NULL,4,NULL);
	vTaskStartScheduler();
}
