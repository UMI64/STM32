#include <stm32f10x.h>
#include <FreeRTOS.h>
#include <task.h>
#include "SYS.h"
#include "GUI.h"

void test ()
{
	void * b;
	void * c;
	void * d;
	void * e;
	void * f;
	while (1)
	{
		SYS_Init();
		GUI_Init ();
		b=SYS_CallMem (200,SYS_MENID);
		c=SYS_CallMem (200,SYS_MENID);
		d=SYS_CallMem (200,SYS_MENID);
		e=SYS_CallMem (1000,SYS_MENID);
		f=SYS_CallMem (800,SYS_MENID);
		SYS_FreeMem (d);
		SYS_FreeMem (b);
		SYS_FreeMem (c);
		SYS_FreeMem (f);
		SYS_FreeMem (e);
	}
}
int main ()
{
	xTaskCreate(test,"test",50,NULL,1,NULL);
	vTaskStartScheduler();
}
