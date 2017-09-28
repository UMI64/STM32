#include <stm32f10x.h>
#include <FreeRTOS.h>
#include <task.h>
#include "SYS.h"
#include "GUI.h"
#include <stable.h>

void Init ()
{
	void * a;
	void * b;
	void * c;
	void * d;
	void * e;
	void * f;
	SYS_Init();
	GUI_Init ();
	Stable_Init ();
	a=SYS_CallMem (200,SYS_MENID);
	b=SYS_CallMem (200,SYS_MENID);
	c=SYS_CallMem (200,SYS_MENID);
	d=SYS_CallMem (200,SYS_MENID);
	e=SYS_CallMem (1000,SYS_MENID);
	f=SYS_CallMem (800,SYS_MENID);
	SYS_FreeMem (a);
	SYS_FreeMem (d);
	SYS_FreeMem (b);
	SYS_FreeMem (c);
	SYS_FreeMem (f);
	SYS_FreeMem (e);
	vTaskDelete(NULL);
}
int main ()
{
	xTaskCreate(Init,"Init",50,NULL,4,NULL);
	vTaskStartScheduler();
}
