#include "SYS_Device.h"
/*Driver*/
#include "tft_ili9341.h"
#include "mpu6050.h"
#include "BrushlessMotor.h"

DriverTreeData* PDriverTree=NULL, * PDriverPassTree=NULL;
//DriverInfoData
DriverInfoData* AddToDriverTree()
{
	DriverTreeData* _PDriverTree;
	_PDriverTree=PDriverTree;
	if (_PDriverTree) 
	{//������һ���ڵ�
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
	/*�����������غ���***************************************/
	TFT_ILI9341_LoadInfo (AddToDriverTree());
	MPU_6050_LoadInfo (AddToDriverTree());
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
	for (_PDriverTree=PDriverTree;_PDriverTree;_PDriverTree=TEMP)//��ȡ�����б���ÿһ���������в��ԣ�ͨ�������ͨ�������б�
	{
		TEMP=_PDriverTree->PNext;
		_PDriverTree->DriverInfo.Init ();//��ʼ��
		if (_PDriverTree->DriverInfo.Test ()==PASS)//���в���
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
				_PDriverPassTree->DriverInfo.Init ();//��ʼ��
				if (_PDriverPassTree->DriverInfo.Test ()==NOTPASS)//�������û�ɹ��ʹ��Ѽ��������б���ɾ��
				{
					SYS_ADDtoDriver (_PDriverPassTree);
				}
			}
		}
		for (_PDriverTree=PDriverTree;_PDriverTree;_PDriverTree=_PDriverTree->PNext)
		{
			if (_PDriverTree->DriverInfo.Test ()==NOTPASS)
			{
				_PDriverTree->DriverInfo.Init ();//��ʼ��
				if (_PDriverTree->DriverInfo.Test ()==PASS)
				{
					SYS_ADDtoDriverPASS (_PDriverTree);
				}
			}
		}
		vTaskDelay (500);
	}
}
void SYS_DriverInit()
{
	SYS_AddtoDriverTree();
	SYS_AddtoDriverPassTree();
	xTaskCreate(SYS_DriverManage,"Manage",40,NULL,1,NULL);
}






















//DriverTreeData* AddDriverInfo (DriverTreeData* PDriverTree)
//{
//	DriverTreeData* PTEMP;
//	while ((*PDriverTree).PDriverInfo != NULL)
//	{
//		if ((*PDriverTree).PNext==NULL) 
//		{
//			(*PDriverTree).PNext=SYS_CallMem(sizeof (DriverTreeData),SYS_MENID);
//			PTEMP=PDriverTree;
//			PDriverTree=(*PDriverTree).PNext;
//			(*PDriverTree).PFront=PTEMP;
//			
//		}
//		else
//		{
//			PDriverTree=(*PDriverTree).PNext;
//		}
//	}
//	return PDriverTree;
//}
//void SYS_Devicd_DriverLoad ()
//{
//	/*�������*/
//	SYS_ili9341_Load (AddDriverInfo (&DriverTree));
//}
//void DeviceFound ()
//{
//	
//}
