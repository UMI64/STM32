#include "GUI_Window.h"

/*返回一个被初始化的窗口链表节点*/
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
/*注册一个窗口*/
WDID GUI_WINDOW_CreatWD(WDFormat WD)
{
	WDListFormat * TEMP,* NewNode;
	uint8_t id=1;
	
	if (WDList) //链表已经被创建
	{
		for (TEMP=WDList;TEMP->ID==id++ || TEMP->PNextWD;TEMP=TEMP->PNextWD);//找到链表尾部或ID不连续的节点
		if (TEMP->ID==id) TEMP=TEMP->PFrontWD;//如果是链表不连续的地方就把指针指向前一个节点
		NewNode=GUI_WINDOW_GetInitedWDlistNode();//新申请一个节点
		if (TEMP->PNextWD)//节点后不为空
		{
			NewNode->ID=id;
			NewNode->PNextWD=TEMP->PNextWD;//新节点后指针指向后节点 P><N | P><N<NEW
			NewNode->PNextWD->PFrontWD=NewNode;//后节点前指针指向新节点 P><N<NEW | P>N><NEW
			NewNode->PFrontWD=TEMP;//新节点前指针指向前节点 P>N><NEW | P>N><NEW>P
			NewNode->PFrontWD->PNextWD=NewNode;//前节点后指针指向新节点 P>N><NEW>P | N><NEW><P
			TEMP=NewNode;
		}
		else//节点后为空
		{
			NewNode->ID=id;
			NewNode->PFrontWD=TEMP;
			TEMP->PNextWD=NewNode;
		}
	}
	else //链表没被创建
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
/*消灭一个窗口*/
void GUI_WINDOW_DestroyWD (WDID ID)
{
	
}
/*给某个窗口添加一个活动区域*/
void GUI_WINDOW_AddActiveRegion(WDID ID)
{
	
}
