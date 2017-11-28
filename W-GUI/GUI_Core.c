#include "GUI.h"
#include "GUI_Core.h"

HardDraw* PHardDrawFUN=NULL;

void GUI_LoadFunction ()
{
	DriverTreeData* PTEMP;
	LCDData* PLCDdata;
	PHardDrawFUN=SYS_CallMem (sizeof (HardDraw),GUI_MENID);
	for (PTEMP=PDriverPassTree;PTEMP->DriverInfo.DriverID!=22 && PTEMP->PNext!=NULL;PTEMP=PTEMP->PNext);//Ѱ���Ѽ��ص���ʾ����
	if (PTEMP->DriverInfo.DriverID==22)//����з��ϵ������Ͱѻ�����ͼ������������
	{/*������ȷ*/
		PLCDdata=PTEMP->DriverInfo.PInfo;
		PHardDrawFUN->DrawPoint=PLCDdata->DrawPoint;
		PHardDrawFUN->DrawRange=PLCDdata->DrawRange;
		PHardDrawFUN->ReadColor=PLCDdata->ReadColor;
	}
	else
	{/*��������*/
		
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
	for (PTEMP=PDriverPassTree;PTEMP->DriverInfo.DriverID!=22 && PTEMP->PNext!=NULL;PTEMP=PTEMP->PNext);//Ѱ���Ѽ��ص���ʾ����
	if (PTEMP->DriverInfo.DriverID==22) 
	{
		PLCDdata=PTEMP->DriverInfo.PInfo;
		PHardDrawFUN->DrawPoint=PLCDdata->DrawPoint;
		PHardDrawFUN->DrawRange=PLCDdata->DrawRange;
		PHardDrawFUN->ReadColor=PLCDdata->ReadColor;
		return 1;//����з��ϵ���������1
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
