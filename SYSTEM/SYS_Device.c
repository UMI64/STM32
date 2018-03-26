#include "SYS_Driver&Device.h"
#include "SYS_Memory.h"
#include <string.h>

DeviceTypeFormate * DeviceTypeTree=NULL;
/*返回一个空的设备树地址*/
DeviceFormate * SYS_DEV_FindNULLDeviceTree (char * DeviceType)
{
	DeviceTypeFormate * TypeTEMP;
	DeviceFormate * DeviceTEMP;
	if (DeviceTypeTree==NULL) //如果设备类型链表为空
	{
		DeviceTypeTree=SYS_CallMem (sizeof (DeviceTypeFormate),NULL);
		DeviceTypeTree->DeviceTypeName=DeviceType;
		DeviceTypeTree->PDevice=SYS_CallMem (sizeof (DeviceFormate),NULL);
		DeviceTypeTree->PNextType=NULL;
		return DeviceTypeTree->PDevice;
	}
	else						//如果设备类型链表不为空
	{
		for (TypeTEMP=DeviceTypeTree;TypeTEMP->PNextType && strcmp(TypeTEMP->DeviceTypeName,DeviceType);TypeTEMP=TypeTEMP->PNextType);//找到对应的类型或者到空
		if (strcmp(TypeTEMP->DeviceTypeName,DeviceType)!=0) //没有此类型
		{
			TypeTEMP->PNextType=SYS_CallMem (sizeof (DeviceTypeFormate),NULL);
			TypeTEMP=TypeTEMP->PNextType;
			TypeTEMP->DeviceTypeName=DeviceType;
			TypeTEMP->PDevice=SYS_CallMem (sizeof (DeviceFormate),NULL);
			TypeTEMP->PNextType=NULL;
			return TypeTEMP->PDevice;
		}
		else
		{
			if (TypeTEMP->PDevice==NULL)
			{
				TypeTEMP->PDevice=SYS_CallMem (sizeof (DeviceFormate),NULL);
				return TypeTEMP->PDevice;
			}
			for (DeviceTEMP=TypeTEMP->PDevice;DeviceTEMP->PNextDevice;DeviceTEMP=DeviceTEMP->PNextDevice);//找到空
			DeviceTEMP->PNextDevice=SYS_CallMem (sizeof (DeviceFormate),NULL);
			return TypeTEMP->PDevice;
		}
	}
}
/*改变设备状态*/
void SYS_DEV_ChangeState (DEV_HANDLE * Handle,DEV_State State)
{
	DeviceFormate * PDevice;
	PDevice=Handle->DevicePointer;
	PDevice->STATE=State;
}
/*获取设备状态*/
DEV_State SYS_DEV_GetState (DEV_HANDLE * Handle)
{
	return ((DeviceFormate *)Handle->DevicePointer)->STATE;
}
/*创建一个设备*/
DEV_HANDLE * SYS_DEV_CREAT (char * DriverName,DeviceNeedFormate * DeviceNeed)
{
	DeviceNeedFormate * pDeviceNeed=NULL,*PNeedtemp=NULL;
	DriverListFormate * DriverList=NULL,* DRItemp=NULL;
	DriverList=SYS_DRI_ReturnDriverList ();
	for (DRItemp=DriverList;DRItemp->NextDriver && strcmp(DRItemp->DriverName,DriverName);DRItemp=DRItemp->NextDriver);
	if (strcmp(DRItemp->DriverName,DriverName)==0)
	{
		for (;DeviceNeed;DeviceNeed=DeviceNeed->NextNeed)
		{
			if (pDeviceNeed==NULL)	
			{
				pDeviceNeed=SYS_CallMem (sizeof (DeviceNeedFormate),NULL);
				PNeedtemp=pDeviceNeed;
			}
			else 
			{
				PNeedtemp->NextNeed=SYS_CallMem (sizeof (DeviceNeedFormate),NULL);
				PNeedtemp=PNeedtemp->NextNeed;
			}
			PNeedtemp->Data=DeviceNeed->Data;
		}
		return DRItemp->drv_RegisterDevice(pDeviceNeed);
	}
	else
	{
		return NULL;
	}
}
/*打开一个设备*/
DEV_HANDLE * SYS_DEV_OPEN (char * DeviceTypeName,uint8_t DeviceID)
{ 
	DeviceFormate * LineTEMP;
	DeviceTypeFormate * HeadTEMP;
	for (HeadTEMP=DeviceTypeTree;HeadTEMP;HeadTEMP=HeadTEMP->PNextType)//遍历设备的类型
	{
		if (strcmp(HeadTEMP->DeviceTypeName,DeviceTypeName)==0)//判断此类型是不是要找的类型
		{
			for (LineTEMP=HeadTEMP->PDevice;LineTEMP && (LineTEMP->DeviceID==DeviceID || DeviceID==NULL);LineTEMP=LineTEMP->PNextDevice)//遍历设备
			{
				if (LineTEMP->STATE==DEV_CLOSE)//此设备关闭中
				{
					if (LineTEMP->Handle->drv_DEV_OPEN(LineTEMP->Handle)==DEV_SUCCESS)//使用驱动的打开设备函数
					{
						LineTEMP->STATE=DEV_OPEN;//设置设备为打开
						return LineTEMP->Handle;//成功打开则返回设备句柄
					}
				}
				if (LineTEMP->PNextDevice==NULL) return NULL;
			}
		}
		if (HeadTEMP->PNextType==NULL) return NULL;
	}
	return NULL;//没找到此类型
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
