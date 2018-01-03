#include "GUI_Message.h"
#include "GUI_MESSdef.h"
MESSListFormat * MessageBox;
uint16_t MessageNum=0;
void GUI_MESS_MessageInit ()
{
	MessageBox=NULL;
}
/*添加一个消息*/
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
	if (MessageBox==NULL)//消息盒子为空
	{
		MessageBox=GUI_MESS_GetInitedMESSNode ();
		TEMP=MessageBox;
	}
	else 
	{
		for (TEMP=MessageBox;TEMP->PNext;TEMP=TEMP->PNext);//找到最后一个
		TEMP->PNext=GUI_MESS_GetInitedMESSNode ();
		TEMP=TEMP->PNext;
	}
	TEMP->Data=Data;
	TEMP->fData=fData;
	return 1;
}
float GUI_MESS_TakeOut_A_FloatData ()
{	
	void * MessageAddr=MessageBox;//存储将要释放的消息地址
	if (MessageBox==NULL) while (1);
	float fdata=MessageBox->fData;
	MessageBox=MessageBox->PNext;	
	SYS_FreeMem (MessageAddr);//释放消息所占的内存空间
	return fdata;//返回一个数据
}
uint16_t GUI_MESS_TakeOut_A_Uint16Data ()
{
	void * MessageAddr=MessageBox;//存储将要释放的消息地址
	if (MessageBox==NULL) while (1);
	uint16_t data=MessageBox->Data;
	MessageBox=MessageBox->PNext;	
	SYS_FreeMem (MessageAddr);//释放消息所占的内存空间
	return data;//返回一个数据
}
void GUI_MESS_Handle_A_Message ()
{
	void * MessageAddr;
	uint16_t CMD;
	if (MessageBox==NULL) return ;//消息盒子为空
	vTaskSuspendAll ();
	MessageAddr=MessageBox;//存储将要释放的消息地址
	CMD=MessageBox->Data;
	MessageBox=MessageBox->PNext;//指向下一个消息
	SYS_FreeMem (MessageAddr);//释放消息所占的内存空间
	switch (CMD)
	{
		case CLEAN: 			GUI_2DLib_MESS_Clear ();				break;
		case DRAW_POINT: 	GUI_2DLib_MESS_DrawPoint ();		break;
		case DRAW_LINE:   GUI_2DLib_MESS_DrawLine (); 		break;
	}
	xTaskResumeAll ();
}
