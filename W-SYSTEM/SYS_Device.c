#include "SYS_Device.h"
/*Driver*/
#include "tft_ili9341.h"

DriverTreeData* PDriverTree=NULL;
DriverTreeData* PDriverPassTree=NULL;
//DriverInfoData
DriverInfoData* AddToDriverTree()
{
	DriverTreeData* _PDriverTree;
	_PDriverTree=PDriverTree;
	if (_PDriverTree) 
	{
		for (;_PDriverTree->PNext;_PDriverTree=_PDriverTree->PNext);
		_PDriverTree->PNext=SYS_CallMem (sizeof (DriverTreeData),SYS_MENID);
		_PDriverTree->PNext->PFront=_PDriverTree;
		_PDriverTree=_PDriverTree->PNext;
		_PDriverTree->PNext=NULL;
	}
	else PDriverTree=SYS_CallMem (sizeof (DriverTreeData),SYS_MENID);
	return &PDriverTree->DriverInfo;
}
void SYS_AddtoDriverTree()
{
	/*加入驱动加载函数*/
	TFT_ILI9341_LoadInfo (AddToDriverTree());
}
void SYS_AddtoDriverPassTree()
{
	DriverTreeData* _PDriverTree;
	DriverTreeData* _PDriverPassTree;
	for (_PDriverTree=PDriverTree;_PDriverTree;_PDriverTree=_PDriverTree->PNext)//读取驱动列表并对每一个驱动进行测试，通过则加入通过驱动列表
	{
		if (_PDriverTree->DriverInfo.Test ()==PASS)
		{
			_PDriverPassTree=PDriverPassTree;
			if (_PDriverPassTree==NULL)
			{
				PDriverPassTree=SYS_CallMem (sizeof (DriverTreeData),SYS_MENID);
				PDriverPassTree->DriverInfo=PDriverTree->DriverInfo;
				PDriverPassTree->PFront=NULL;
				PDriverPassTree->PNext=NULL;
			}
			else
			{
				for (;_PDriverPassTree->PNext;_PDriverPassTree=_PDriverPassTree->PNext);
				_PDriverPassTree->PNext=SYS_CallMem (sizeof (DriverTreeData),SYS_MENID);
				_PDriverPassTree->PNext->PFront=_PDriverPassTree;
				_PDriverPassTree=_PDriverPassTree->PNext;
				_PDriverPassTree->PNext=NULL;
				_PDriverPassTree->DriverInfo=_PDriverTree->DriverInfo;
			}
		}
	}
}
void SYS_DriverInit()
{
	SYS_AddtoDriverTree();
	SYS_AddtoDriverPassTree();
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
//	/*添加驱动*/
//	SYS_ili9341_Load (AddDriverInfo (&DriverTree));
//}
//void DeviceFound ()
//{
//	
//}
