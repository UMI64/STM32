#include "GUI.h"
#include "GUI_Core.h"

HardDraw* PHardDrawFUN=NULL;

void GUI_LoadFunction ()
{
	DriverTreeData* PTEMP;
	LCDData* PLCDdata;
	PHardDrawFUN=SYS_CallMem (sizeof (HardDraw),GUI_MENID);
	for (PTEMP=PDriverPassTree;PTEMP->DriverInfo.DriverID!=22 && PTEMP->PNext!=NULL;PTEMP=PTEMP->PNext);//寻找已加载的显示驱动
	if (PTEMP->DriverInfo.DriverID==22)//如果有符合的驱动就把基本绘图函数链接起来
	{/*驱动正确*/
		PLCDdata=PTEMP->DriverInfo.PInfo;
		PHardDrawFUN->DrawPoint=PLCDdata->DrawPoint;
		PHardDrawFUN->DrawRange=PLCDdata->DrawRange;
		PHardDrawFUN->ReadColor=PLCDdata->ReadColor;
	}
	else
	{/*驱动错误*/
		
	}
}
void GUI_Init ()
{
	SYS_CallHEAP (GUI_MEMSIZE,GUI_MENID);
	GUI_LoadFunction ();
	xTaskCreate(GUI,"test",50,NULL,1,NULL);
}
u8 GUI_Find_TestHardDrawFUNChange ()
{
	DriverTreeData* PTEMP;
	LCDData* PLCDdata;
	for (PTEMP=PDriverPassTree;PTEMP->DriverInfo.DriverID!=22 && PTEMP->PNext!=NULL;PTEMP=PTEMP->PNext);//寻找已加载的显示驱动
	if (PTEMP->DriverInfo.DriverID==22) 
	{
		PLCDdata=PTEMP->DriverInfo.PInfo;
		PHardDrawFUN->DrawPoint=PLCDdata->DrawPoint;
		PHardDrawFUN->DrawRange=PLCDdata->DrawRange;
		PHardDrawFUN->ReadColor=PLCDdata->ReadColor;
		return 1;//如果有符合的驱动返回1
	}
	else
	{		
		
		return 0;
	}
};
void GUI ()
{

	while (1)
	{
		if (GUI_Find_TestHardDrawFUNChange ())
		{
			PHardDrawFUN->DrawRange (20,20,60,60,0x6789);
		}
	}
}
