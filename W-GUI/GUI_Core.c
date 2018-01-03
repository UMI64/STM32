#include "GUI_Core.h"
/*�豸��Ϣ�ṹ�����*/
HardInformationFormat * HardInformation;
/*�豸���ܺ����ṹ�����*/
HardDraw * PHardDrawFUN;
/*������Ϣ�ṹ��(����)����*/
WDListFormat * WDList=NULL;
/*GUI�ڴ�ID����*/
uint8_t GUIMENID;

uc8 GUI_CORE_LoadFunction ()
{
	DriverTreeData* PTEMP;
	LCDData* PLCDdata;
	for (PTEMP=PDriverPassTree;PTEMP->DriverInfo.DriverID!=22 && PTEMP->PNext!=NULL;PTEMP=PTEMP->PNext);//Ѱ���Ѽ��ص���ʾ����
	if (PTEMP->DriverInfo.DriverID==22)//����з��ϵ������Ͱѻ�����ͼ������������
	{/*������ȷ����*/
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
	{/*û�з�������*/
		return 0;
	}
}
void GUI_CORE_WaitFunction ()
{
	while (GUI_CORE_LoadFunction ()==0);
	xTaskCreate(GUI_CORE,"test",100,NULL,1,NULL);//����GUI���Ľ���
	vTaskDelete(NULL);
}
void GUI_CORE_Init ()
{
	GUIMENID=SYS_CallHEAP (7096);//����GUI�ڴ�
	PHardDrawFUN=SYS_CallMem (sizeof (HardDraw),GUIMENID);//
	HardInformation=SYS_CallMem (sizeof (HardInformationFormat),GUIMENID);//
	GUI_MESS_MessageInit ();//��Ϣ��ʼ��
	xTaskCreate(GUI_CORE_WaitFunction,"test",50,NULL,1,NULL);//����GUI���ܼ��ؽ���
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


