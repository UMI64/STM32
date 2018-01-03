#include "SYS_Device.h"
/*Driver*/
#include "tft_ili9341.h"
#include "BrushlessMotor.h"

DriverTreeData* PDriverTree=NULL, * PDriverPassTree=NULL;
//DriverInfoData
DriverInfoData* AddToDriverTree()
{
	DriverTreeData* _PDriverTree;
	_PDriverTree=PDriverTree;
	if (_PDriverTree) 
	{//建立下一个节点
		for (;_PDriverTree->PNext;_PDriverTree=_PDriverTree->PNext);
		_PDriverTree->PNext=SYS_CallMem (sizeof (DriverTreeData),NULL);
		_PDriverTree->PNext->PFront=_PDriverTree;
		_PDriverTree=_PDriverTree->PNext;
		_PDriverTree->PNext=NULL;
	}
	else
	{		
		PDriverTree=SYS_CallMem (sizeof (DriverTreeData),NULL);
		_PDriverTree=PDriverTree;
	}
	return &_PDriverTree->DriverInfo;
}
void SYS_AddtoDriverTree()
{
	/*加入驱动加载函数***************************************/
	TFT_ILI9341_LoadInfo (AddToDriverTree());
	BrushlessMotor_LoadInfo (AddToDriverTree());
}
void SYS_DeleteDriverfromPDriverPassTree (DriverTreeData * P)
{
	if (P->PFront==NULL && P->PNext!=NULL) {PDriverPassTree=P->PNext; P->PNext->PFront=NULL;}
	else if (P->PNext==NULL && P->PFront!=NULL) P->PFront->PNext=NULL;
	else if (P->PFront==NULL && P->PNext==NULL ) PDriverPassTree=NULL;
	else 
	{
		P->PFront->PNext=P->PNext;
		P->PNext->PFront=P->PFront;
	}
}
void SYS_DeleteDriverfromPDriverTree (DriverTreeData * P)
{
	if (P->PFront==NULL && P->PNext!=NULL) {PDriverTree=P->PNext; P->PNext->PFront=NULL;}
	else if (P->PNext==NULL && P->PFront!=NULL) P->PFront->PNext=NULL;
	else if (P->PFront==NULL && P->PNext==NULL ) PDriverTree=NULL;
	else 
	{
		P->PFront->PNext=P->PNext;
		P->PNext->PFront=P->PFront;
	}
}
void SYS_ADDtoDriverPASS (DriverTreeData* P)
{
	DriverTreeData* _PDriverPassTree=PDriverPassTree;
	SYS_DeleteDriverfromPDriverTree (P);
	if (PDriverPassTree==NULL)
	{
		PDriverPassTree=P;
		PDriverPassTree->PFront=NULL;
		PDriverPassTree->PNext=NULL;
	}
	else
	{
		for (;_PDriverPassTree->PNext;_PDriverPassTree=_PDriverPassTree->PNext);
		_PDriverPassTree->PNext=P;
		_PDriverPassTree->PNext->PFront=_PDriverPassTree;
		_PDriverPassTree=_PDriverPassTree->PNext;
		_PDriverPassTree->PNext=NULL;
	}
}
void SYS_ADDtoDriver (DriverTreeData* P)
{
	DriverTreeData* _PDriverTree=PDriverTree;
	SYS_DeleteDriverfromPDriverPassTree (P);
	if (PDriverTree==NULL)
	{
		PDriverTree=P;
		PDriverTree->PFront=NULL;
		PDriverTree->PNext=NULL;
	}
	else
	{
		for (;_PDriverTree->PNext;_PDriverTree=_PDriverTree->PNext);
		_PDriverTree->PNext=P;
		_PDriverTree->PNext->PFront=_PDriverTree;
		_PDriverTree=_PDriverTree->PNext;
		_PDriverTree->PNext=NULL;
	}
}
void SYS_AddtoDriverPassTree()
{
	DriverTreeData* _PDriverTree;
	DriverTreeData* TEMP;
	for (_PDriverTree=PDriverTree;_PDriverTree;_PDriverTree=TEMP)//读取驱动列表并对每一个驱动进行测试，通过则加入通过驱动列表
	{
		TEMP=_PDriverTree->PNext;
		_PDriverTree->DriverInfo.Init ();//初始化
		if (_PDriverTree->DriverInfo.Test ()==PASS)//进行测试
		{
			SYS_ADDtoDriverPASS (_PDriverTree);
		}
	}
}
void SYS_DriverManage ()
{
	DriverTreeData* _PDriverTree;
	DriverTreeData* _PDriverPassTree;
	while (1)
	{
		for (_PDriverPassTree=PDriverPassTree;_PDriverPassTree;_PDriverPassTree=_PDriverPassTree->PNext)
		{
			if (_PDriverPassTree->DriverInfo.Test ()==NOTPASS)
			{
				_PDriverPassTree->DriverInfo.Init ();//初始化
				if (_PDriverPassTree->DriverInfo.Test ()==NOTPASS)//如果还是没成功就从已加载驱动列表中删掉
				{
					SYS_ADDtoDriver (_PDriverPassTree);
				}
			}
		}
		for (_PDriverTree=PDriverTree;_PDriverTree;_PDriverTree=_PDriverTree->PNext)
		{
			if (_PDriverTree->DriverInfo.Test ()==NOTPASS)
			{
				_PDriverTree->DriverInfo.Init ();//初始化
				if (_PDriverTree->DriverInfo.Test ()==PASS)
				{
					SYS_ADDtoDriverPASS (_PDriverTree);
				}
			}
		}
		int i;
		int *q=&i;
		*q=uxTaskGetStackHighWaterMark(NULL);
		vTaskDelay (500);
	}
}
void SYS_DriverInit()
{
	SYS_AddtoDriverTree();
	SYS_AddtoDriverPassTree();
	xTaskCreate(SYS_DriverManage,"Manage",30,NULL,1,NULL);
}


/***********************/




DeviceTreeHeadFormate * DeviceTree=NULL;

DeviceTreeHeadFormate * SYS_DEV_FindNULLTreeHead ()
{
	DeviceTreeHeadFormate * TEMP;
	if (DeviceTree==NULL) 
	{
		DeviceTree=SYS_CallMem (sizeof (DeviceTreeHeadFormate),NULL);
		DeviceTree->DeviceName=NULL;
		DeviceTree->PDeviceLine=NULL;
		DeviceTree->PNextHead=NULL;
		return DeviceTree;
	}
	else
	{
		for (TEMP=DeviceTree;TEMP->PNextHead;TEMP=TEMP->PNextHead);
		TEMP->PNextHead=SYS_CallMem (sizeof (DeviceTreeHeadFormate),NULL);
		TEMP=TEMP->PNextHead;
		TEMP->DeviceName=NULL;
		TEMP->PDeviceLine=NULL;
		TEMP->PNextHead=NULL;
		return TEMP;
	}
}
void SYS_DEV_ChangeState (DEV_HANDLE * Handle,DEV_State State)
{
	DeviceLineFormate * PDevice;
	PDevice=Handle->DevicePointer;
	PDevice->STATE=State;
}
/*打开一个设备*/
DEV_HANDLE * SYS_DEV_OPEN (char * DeviceName)
{
	DeviceLineFormate * LineTEMP;
	DeviceTreeHeadFormate * HeadTEMP;
	for (HeadTEMP=DeviceTree;HeadTEMP;HeadTEMP=HeadTEMP->PNextHead)//遍历设备的头
	{
		if (*HeadTEMP->DeviceName==*DeviceName)//判断此头是不是要找的头
		{
			for (LineTEMP=HeadTEMP->PDeviceLine;LineTEMP;LineTEMP=LineTEMP->PNextDevice)//遍历设备
			{
				if (LineTEMP->STATE==DEV_CLOSE)//此设备关闭中
				{
					if (LineTEMP->Handle->drv_DEV_OPEN()==DEV_SUCCESS)//使用驱动的打开设备函数
					{
						LineTEMP->STATE=DEV_OPEN;//设置设备为打开
						return LineTEMP->Handle;//成功打开则返回设备句柄
					}
				}
				if (LineTEMP->PNextDevice==NULL) return NULL;
			}
		}
		if (HeadTEMP->PNextHead==NULL) return NULL;
	}
	return NULL;//没找到头
}
/*关闭一个设备*/
DEV_State SYS_DEV_CLOSE (DEV_HANDLE * Handle)
{
	DEV_State State;
	State=Handle->drv_DEV_CLOSE ();//使用关闭设备函数
	if (State==SUCCESS) SYS_DEV_ChangeState(Handle->DevicePointer,DEV_CLOSE);//设置设备为关闭
	return State;
}
/*控制驱动*/
DEV_State SYS_DEV_CONTROL (DEV_HANDLE * Handle,uint16_t CMD,void * Buffer,uint32_t Buffersize)
{
	return Handle->drv_DEV_CONTROL(Handle,CMD,Buffer,Buffersize);//使用设备控制函数
}
