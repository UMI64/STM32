#include "GUI_Message.h"
#include "GUI_MESSdef.h"
MESSListFormat * MessageBox;
uint16_t MessageNum=0;
void GUI_MESS_MessageInit ()
{
	MessageBox=NULL;
}
/*���һ����Ϣ*/
MESSListFormat * GUI_MESS_GetInitedMESSNode ()
{
	uint16_t size=0;
	MESSListFormat * TEMP;
	size=SYS_HeapFreeSize (1);
	TEMP=SYS_CallMem (sizeof (MESSListFormat),GUIMENID);
	if (TEMP==NULL)
		while (1);
	TEMP->Data=NULL;
	TEMP->fData=NULL;
	TEMP->PNext=NULL;
	return TEMP;
}
uint8_t GUI_MESS_ADD_A_Message (uint16_t Data,float fData)
{
	MESSListFormat * TEMP;
	if (MessageNum>=MAXMESS) 
		return 0;
	if (MessageBox==NULL)//��Ϣ����Ϊ��
	{
		MessageBox=GUI_MESS_GetInitedMESSNode ();
		TEMP=MessageBox;
	}
	else 
	{
		for (TEMP=MessageBox;TEMP->PNext;TEMP=TEMP->PNext);//�ҵ����һ��
		TEMP->PNext=GUI_MESS_GetInitedMESSNode ();
		TEMP=TEMP->PNext;
	}
	TEMP->Data=Data;
	TEMP->fData=fData;
	return 1;
}
float GUI_MESS_TakeOut_A_FloatData ()
{	
	void * MessageAddr=MessageBox;//�洢��Ҫ�ͷŵ���Ϣ��ַ
	if (MessageBox==NULL) while (1);
	float fdata=MessageBox->fData;
	MessageBox=MessageBox->PNext;	
	SYS_FreeMem (MessageAddr);//�ͷ���Ϣ��ռ���ڴ�ռ�
	return fdata;//����һ������
}
uint16_t GUI_MESS_TakeOut_A_Uint16Data ()
{
	void * MessageAddr=MessageBox;//�洢��Ҫ�ͷŵ���Ϣ��ַ
	if (MessageBox==NULL) while (1);
	uint16_t data=MessageBox->Data;
	MessageBox=MessageBox->PNext;	
	SYS_FreeMem (MessageAddr);//�ͷ���Ϣ��ռ���ڴ�ռ�
	return data;//����һ������
}
void GUI_MESS_Handle_A_Message ()
{
	void * MessageAddr;
	uint16_t CMD;
	if (MessageBox==NULL) return ;//��Ϣ����Ϊ��
	vTaskSuspendAll ();
	MessageAddr=MessageBox;//�洢��Ҫ�ͷŵ���Ϣ��ַ
	CMD=MessageBox->Data;
	MessageBox=MessageBox->PNext;//ָ����һ����Ϣ
	SYS_FreeMem (MessageAddr);//�ͷ���Ϣ��ռ���ڴ�ռ�
	switch (CMD)
	{
		case CLEAN: 			GUI_2DLib_MESS_Clear ();				break;
		case DRAW_POINT: 	GUI_2DLib_MESS_DrawPoint ();		break;
		case DRAW_LINE:   GUI_2DLib_MESS_DrawLine (); 		break;
	}
	xTaskResumeAll ();
}
