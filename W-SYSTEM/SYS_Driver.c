#include "SYS_Driver.h"

void SYS_DRI_RegisterDevice (char * DeviceName,DriverREGFormate * DriverREG)
{
	DeviceTreeHeadFormate * HeadTEMP;
	DriverREGFormate * REGTEMP;
	DeviceLineFormate * LineTEMP;
	/*�õ�һ���յ���������ͷ*/
	HeadTEMP=SYS_DEV_FindNULLTreeHead ();
	HeadTEMP->DeviceName=DeviceName;
	for (REGTEMP=DriverREG;REGTEMP;REGTEMP=REGTEMP->PNEXT)
	{
		if (HeadTEMP->PDeviceLine==NULL) //���û�е�һ����
		{
			HeadTEMP->PDeviceLine=SYS_CallMem (sizeof (DeviceLineFormate),NULL);
			LineTEMP=HeadTEMP->PDeviceLine;
			LineTEMP->PNextDevice=NULL;
		}
		else if (LineTEMP->PNextDevice==NULL)//��һ����������
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
/*�������ע�ắ��*/
void SYS_DRI_Init ()
{
	MPU_6050_REG ();
}
