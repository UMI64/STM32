#include "GUI_Core.h"
HardInformationFormat * HardInformation;
HardDraw * PHardDrawFUN;
WDListFormat * WDList=NULL;
	
uc8 GUI_LoadFunction ()
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
void GUI_WaitFunction ()
{
	while (GUI_LoadFunction ()==0);
	//xTaskCreate(GUI,"test",50,NULL,1,NULL);//����GUI���Ľ���
	vTaskDelete(NULL);
}
void GUI_Init ()
{
	SYS_CallHEAP (GUI_MEMSIZE,GUI_MENID);//����GUI�ڴ�
	PHardDrawFUN=SYS_CallMem (sizeof (HardDraw),GUI_MENID);//
	HardInformation=SYS_CallMem (sizeof (HardInformationFormat),GUI_MENID);//
	xTaskCreate(GUI_WaitFunction,"test",50,NULL,1,NULL);//����GUI���ܼ��ؽ���
}
void GUI ()
{

	while (1)
	{
		GUI_Refresh (WDList);
		PHardDrawFUN->DrawRange (20,20,60,60,0x6789);
	}
}
/*����һ������ʼ���Ĵ�������ڵ�*/
WDListFormat* GUI_GetInitedWDlistNode ()
{
	WDListFormat* PWDListNode;
	PWDListNode=SYS_CallMem (sizeof (WDListFormat),GUI_MENID);//
	PWDListNode->PFrontWD=NULL;
	PWDListNode->PNextWD=NULL;
	PWDListNode->ID=NULL;
	PWDListNode->Width=NULL;
	PWDListNode->Length=NULL;
	PWDListNode->XPosition=NULL;
	PWDListNode->YPosition=NULL;
	return PWDListNode;
}
/*ע��һ������*/
WDID GUI_CreatWD(WDFormat WD)
{
	WDListFormat * TEMP,* NewNode;
	uint8_t id=1;
	
	if (WDList) //�����Ѿ�������
	{
		for (TEMP=WDList;TEMP->ID==id++ || TEMP->PNextWD;TEMP=TEMP->PNextWD);//�ҵ�����β����ID�������Ľڵ�
		if (TEMP->ID==id) TEMP=TEMP->PFrontWD;//��������������ĵط��Ͱ�ָ��ָ��ǰһ���ڵ�
		NewNode=GUI_GetInitedWDlistNode();//������һ���ڵ�
		if (TEMP->PNextWD)//�ڵ��Ϊ��
		{
			NewNode->ID=id;
			NewNode->PNextWD=TEMP->PNextWD;//�½ڵ��ָ��ָ���ڵ� P><N | P><N<NEW
			NewNode->PNextWD->PFrontWD=NewNode;//��ڵ�ǰָ��ָ���½ڵ� P><N<NEW | P>N><NEW
			NewNode->PFrontWD=TEMP;//�½ڵ�ǰָ��ָ��ǰ�ڵ� P>N><NEW | P>N><NEW>P
			NewNode->PFrontWD->PNextWD=NewNode;//ǰ�ڵ��ָ��ָ���½ڵ� P>N><NEW>P | N><NEW><P
			TEMP=NewNode;
		}
		else//�ڵ��Ϊ��
		{
			NewNode->ID=id;
			NewNode->PFrontWD=TEMP;
			TEMP->PNextWD=NewNode;
		}
	}
	else //����û������
	{	
		TEMP=GUI_GetInitedWDlistNode();
		TEMP->ID=1;
		WDList=TEMP;
	}
	TEMP->Width=WD.Width;
	TEMP->Length=WD.Length;
	TEMP->XPosition=WD.XPosition;
	TEMP->YPosition=WD.YPosition;
	return id;
}
/*����һ������*/
void GUI_DestroyWD (WDID ID)
{
	
}
/*��ĳ���������һ�������*/
void GUI_AddActiveRegion(WDID ID)
{
	
}

