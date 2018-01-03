#include "SYS_Driver.h"

void SYS_DRI_RegisterDevice (char * DeviceName,DriverREGFormate * DriverREG)
{
	DeviceTreeHeadFormate * HeadTEMP;
	DriverREGFormate * REGTEMP;
	DeviceLineFormate * LineTEMP;
	/*得到一个空的驱动链表头*/
	HeadTEMP=SYS_DEV_FindNULLTreeHead ();
	HeadTEMP->DeviceName=DeviceName;
	for (REGTEMP=DriverREG;REGTEMP;REGTEMP=REGTEMP->PNEXT)
	{
		if (HeadTEMP->PDeviceLine==NULL) //如果没有第一个链
		{
			HeadTEMP->PDeviceLine=SYS_CallMem (sizeof (DeviceLineFormate),NULL);
			LineTEMP=HeadTEMP->PDeviceLine;
			LineTEMP->PNextDevice=NULL;
		}
		else if (LineTEMP->PNextDevice==NULL)//下一个链不存在
		{
			LineTEMP->PNextDevice=SYS_CallMem (sizeof (DeviceLineFormate),NULL);
			LineTEMP=LineTEMP->PNextDevice;
			LineTEMP->PNextDevice=NULL;
		}
		LineTEMP->STATE=DEV_CLOSE;
		LineTEMP->Handle=SYS_CallMem (sizeof (DEV_HANDLE),NULL);
		LineTEMP->Handle->DevicePointer=LineTEMP;
		LineTEMP->Handle->drv_DEV_OPEN= REGTEMP->FUN.drv_DEV_OPEN;
		LineTEMP->Handle->drv_DEV_CONTROL = REGTEMP->FUN.drv_DEV_CONTROL;
		LineTEMP->Handle->drv_DEV_CLOSE = REGTEMP->FUN.drv_DEV_CLOSE;
	}
}
/*添加驱动注册函数*/
void SYS_DRI_Init ()
{
	MPU_6050_REG ();
}
