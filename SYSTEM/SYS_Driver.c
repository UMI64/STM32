#include "SYS_Driver&Device.h"
DriverListFormate * DriverList=NULL;
/*返回驱动链表*/
DriverListFormate * SYS_DRI_ReturnDriverList ()
{
	return DriverList;
}
/*注册驱动*/
/*
驱动调用
*/
void SYS_DRI_RegisterDriver (DriverREGFormate DriverREG)
{
	DriverListFormate * TEMP=NULL;
	if (DriverList==NULL)//第一次添加驱动
	{
		DriverList=SYS_CallMem (sizeof (DriverListFormate),NULL);
		DriverList->DriverName=DriverREG.DriverName;
		DriverList->DriverNeed=DriverREG.Need;
		DriverList->drv_RegisterDevice=DriverREG.drv_RegisterDevice;
		DriverList->NextDriver=NULL;
	}
	else
	{
		for (TEMP=DriverList;DriverList->NextDriver;DriverList=DriverList->NextDriver);
		TEMP->NextDriver=SYS_CallMem (sizeof (DriverListFormate),NULL);
		TEMP=TEMP->NextDriver;
		TEMP->DriverName=DriverREG.DriverName;
		TEMP->DriverNeed=DriverREG.Need;
		TEMP->drv_RegisterDevice=DriverREG.drv_RegisterDevice;
		TEMP->NextDriver=NULL;
	}
}
/*注册设备*/
/*
驱动调用
*/
DEV_HANDLE * SYS_DRI_RegisterDevice (DeviceREGFormate DeviceREG)
{
	DeviceFormate * DeviceTEMP;
	
	DeviceTEMP=SYS_DEV_FindNULLDeviceTree (DeviceREG.DeviceType);//得到一个空的设备
	DeviceTEMP->DeviceID=DeviceREG.DeviceID;
	DeviceTEMP->STATE=DEV_CLOSE;
	DeviceTEMP->Handle=SYS_CallMem (sizeof (DEV_HANDLE),NULL);
	DeviceTEMP->Handle->DriverNeed=DeviceREG.Needs;
	DeviceTEMP->Handle->DevicePointer=DeviceTEMP;
	DeviceTEMP->Handle->drv_DEV_OPEN=DeviceREG.FUN.drv_DEV_OPEN;
	DeviceTEMP->Handle->drv_DEV_CONTROL = DeviceREG.FUN.drv_DEV_CONTROL;
	DeviceTEMP->Handle->drv_DEV_CLOSE = DeviceREG.FUN.drv_DEV_CLOSE;
	return DeviceTEMP->Handle;
}
/*添加驱动注册函数*/
void SYS_DRI_Init ()
{
	TFT_ILI9341_REG ();
	MPU_6050_REG ();
}
