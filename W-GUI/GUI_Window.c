#include "GUI_Window.h"

/*����һ������ʼ���Ĵ�������ڵ�*/
WDListFormat* GUI_WINDOW_GetInitedWDlistNode ()
{
	WDListFormat* PWDListNode;
	PWDListNode=SYS_CallMem (sizeof (WDListFormat),GUIMENID);//
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
WDID GUI_WINDOW_CreatWD(WDFormat WD)
{
	WDListFormat * TEMP,* NewNode;
	uint8_t id=1;
	
	if (WDList) //�����Ѿ�������
	{
		for (TEMP=WDList;TEMP->ID==id++ || TEMP->PNextWD;TEMP=TEMP->PNextWD);//�ҵ�����β����ID�������Ľڵ�
		if (TEMP->ID==id) TEMP=TEMP->PFrontWD;//��������������ĵط��Ͱ�ָ��ָ��ǰһ���ڵ�
		NewNode=GUI_WINDOW_GetInitedWDlistNode();//������һ���ڵ�
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
		TEMP=GUI_WINDOW_GetInitedWDlistNode();
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
void GUI_WINDOW_DestroyWD (WDID ID)
{
	
}
/*��ĳ���������һ�������*/
void GUI_WINDOW_AddActiveRegion(WDID ID)
{
	
}
