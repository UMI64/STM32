#include "SYS_Driver&Device.h"
#include "SYS_Memory.h"
#include <string.h>

DeviceTypeFormate * DeviceTypeTree=NULL;
/*����һ���յ��豸����ַ*/
DeviceFormate * SYS_DEV_FindNULLDeviceTree (char * DeviceType)
{
	DeviceTypeFormate * TypeTEMP;
	DeviceFormate * DeviceTEMP;
	if (DeviceTypeTree==NULL) //����豸��������Ϊ��
	{
		DeviceTypeTree=SYS_CallMem (sizeof (DeviceTypeFormate),NULL);
		DeviceTypeTree->DeviceTypeName=DeviceType;
		DeviceTypeTree->PDevice=SYS_CallMem (sizeof (DeviceFormate),NULL);
		DeviceTypeTree->PNextType=NULL;
		return DeviceTypeTree->PDevice;
	}
	else						//����豸��������Ϊ��
	{
		for (TypeTEMP=DeviceTypeTree;TypeTEMP->PNextType && strcmp(TypeTEMP->DeviceTypeName,DeviceType);TypeTEMP=TypeTEMP->PNextType);//�ҵ���Ӧ�����ͻ��ߵ���
		if (strcmp(TypeTEMP->DeviceTypeName,DeviceType)!=0) //û�д�����
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
			for (DeviceTEMP=TypeTEMP->PDevice;DeviceTEMP->PNextDevice;DeviceTEMP=DeviceTEMP->PNextDevice);//�ҵ���
			DeviceTEMP->PNextDevice=SYS_CallMem (sizeof (DeviceFormate),NULL);
			return TypeTEMP->PDevice;
		}
	}
}
/*�ı��豸״̬*/
void SYS_DEV_ChangeState (DEV_HANDLE * Handle,DEV_State State)
{
	DeviceFormate * PDevice;
	PDevice=Handle->DevicePointer;
	PDevice->STATE=State;
}
/*��ȡ�豸״̬*/
DEV_State SYS_DEV_GetState (DEV_HANDLE * Handle)
{
	return ((DeviceFormate *)Handle->DevicePointer)->STATE;
}
/*����һ���豸*/
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
/*��һ���豸*/
DEV_HANDLE * SYS_DEV_OPEN (char * DeviceTypeName,uint8_t DeviceID)
{ 
	DeviceFormate * LineTEMP;
	DeviceTypeFormate * HeadTEMP;
	for (HeadTEMP=DeviceTypeTree;HeadTEMP;HeadTEMP=HeadTEMP->PNextType)//�����豸������
	{
		if (strcmp(HeadTEMP->DeviceTypeName,DeviceTypeName)==0)//�жϴ������ǲ���Ҫ�ҵ�����
		{
			for (LineTEMP=HeadTEMP->PDevice;LineTEMP && (LineTEMP->DeviceID==DeviceID || DeviceID==NULL);LineTEMP=LineTEMP->PNextDevice)//�����豸
			{
				if (LineTEMP->STATE==DEV_CLOSE)//���豸�ر���
				{
					if (LineTEMP->Handle->drv_DEV_OPEN(LineTEMP->Handle)==DEV_SUCCESS)//ʹ�������Ĵ��豸����
					{
						LineTEMP->STATE=DEV_OPEN;//�����豸Ϊ��
						return LineTEMP->Handle;//�ɹ����򷵻��豸���
					}
				}
				if (LineTEMP->PNextDevice==NULL) return NULL;
			}
		}
		if (HeadTEMP->PNextType==NULL) return NULL;
	}
	return NULL;//û�ҵ�������
}
/*�ر�һ���豸*/
DEV_State SYS_DEV_CLOSE (DEV_HANDLE * Handle)
{
	DEV_State State;
	State=Handle->drv_DEV_CLOSE ();//ʹ�ùر��豸����
	if (State==SUCCESS) SYS_DEV_ChangeState(Handle->DevicePointer,DEV_CLOSE);//�����豸Ϊ�ر�
	return State;
}
/*��������*/
DEV_State SYS_DEV_CONTROL (DEV_HANDLE * Handle,uint16_t CMD,void * Buffer,uint32_t Buffersize)
{
	return Handle->drv_DEV_CONTROL(Handle,CMD,Buffer,Buffersize);//ʹ���豸���ƺ���
}
