#include "GUI_Core.h"
/*设备信息结构体变量*/
HardInformationFormat * HardInformation;
/*设备功能函数结构体变量*/
HardDraw * PHardDrawFUN;
/*窗口信息结构体(链表)变量*/
WDListFormat * WDList=NULL;
/*GUI内存ID变量*/
uint8_t GUIMENID;

uc8 GUI_CORE_LoadFunction ()
{
	DriverTreeData* PTEMP;
	LCDData* PLCDdata;
	for (PTEMP=PDriverPassTree;PTEMP->DriverInfo.DriverID!=22 && PTEMP->PNext!=NULL;PTEMP=PTEMP->PNext);//寻找已加载的显示驱动
	if (PTEMP->DriverInfo.DriverID==22)//如果有符合的驱动就把基本绘图函数链接起来
	{/*发现正确驱动*/
		PLCDdata=PTEMP->DriverInfo.PInfo;
		HardInformation->color_type=PLCDdata->color_type;
		HardInformation->id=PLCDdata->id;
		HardInformation->length=PLCDdata->length;
		HardInformation->width=PLCDdata->width;
		PHardDrawFUN->DrawPoint=PLCDdata->DrawPoint;
		PHardDrawFUN->DrawRange=PLCDdata->DrawRange;
		PHardDrawFUN->ReadColor=PLCDdata->ReadColor;
		return 1;
	}
	else
	{/*没有发现驱动*/
		return 0;
	}
}
void GUI_CORE_WaitFunction ()
{
	while (GUI_CORE_LoadFunction ()==0);
	xTaskCreate(GUI_CORE,"test",100,NULL,1,NULL);//启动GUI核心进程
	vTaskDelete(NULL);
}
void GUI_CORE_Init ()
{
	GUIMENID=SYS_CallHEAP (7096);//分配GUI内存
	PHardDrawFUN=SYS_CallMem (sizeof (HardDraw),GUIMENID);//
	HardInformation=SYS_CallMem (sizeof (HardInformationFormat),GUIMENID);//
	GUI_MESS_MessageInit ();//消息初始化
	xTaskCreate(GUI_CORE_WaitFunction,"test",50,NULL,1,NULL);//启动GUI功能加载进程
}
void GUI_CORE ()
{
	while (1)
	{
		int i=0;
		int *u=&i;
		*u=uxTaskGetStackHighWaterMark(NULL);
		GUI_MESS_Handle_A_Message ();
	}
}


