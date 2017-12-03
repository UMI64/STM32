#include "GUI_Core.h"
HardInformationFormat * HardInformation;
HardDraw * PHardDrawFUN;
WDListFormat * WDList=NULL;
	
uc8 GUI_LoadFunction ()
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
void GUI_WaitFunction ()
{
	while (GUI_LoadFunction ()==0);
	//xTaskCreate(GUI,"test",50,NULL,1,NULL);//启动GUI核心进程
	vTaskDelete(NULL);
}
void GUI_Init ()
{
	SYS_CallHEAP (GUI_MEMSIZE,GUI_MENID);//分配GUI内存
	PHardDrawFUN=SYS_CallMem (sizeof (HardDraw),GUI_MENID);//
	HardInformation=SYS_CallMem (sizeof (HardInformationFormat),GUI_MENID);//
	xTaskCreate(GUI_WaitFunction,"test",50,NULL,1,NULL);//启动GUI功能加载进程
}
void GUI ()
{

	while (1)
	{
		GUI_Refresh (WDList);
		PHardDrawFUN->DrawRange (20,20,60,60,0x6789);
	}
}
/*返回一个被初始化的窗口链表节点*/
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
/*注册一个窗口*/
WDID GUI_CreatWD(WDFormat WD)
{
	WDListFormat * TEMP,* NewNode;
	uint8_t id=1;
	
	if (WDList) //链表已经被创建
	{
		for (TEMP=WDList;TEMP->ID==id++ || TEMP->PNextWD;TEMP=TEMP->PNextWD);//找到链表尾部或ID不连续的节点
		if (TEMP->ID==id) TEMP=TEMP->PFrontWD;//如果是链表不连续的地方就把指针指向前一个节点
		NewNode=GUI_GetInitedWDlistNode();//新申请一个节点
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
/*消灭一个窗口*/
void GUI_DestroyWD (WDID ID)
{
	
}
/*给某个窗口添加一个活动区域*/
void GUI_AddActiveRegion(WDID ID)
{
	
}

