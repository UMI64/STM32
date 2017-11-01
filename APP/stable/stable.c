#include "stable.h"
GyroscopeData* GetGyroscopeData;
BrushlessMotorData* BrushlessMotordata;
void Stable_LoadFunction ()
{
	DriverTreeData* PTEMP;
	GyroscopeData* PGyroscopeData;
	BrushlessMotorData* PBrushlessMotordata;
	for (PTEMP=PDriverPassTree;PTEMP->DriverInfo.DriverID!=50 && PTEMP->PNext!=NULL;PTEMP=PTEMP->PNext);//Ѱ���Ѽ�������
	if (PTEMP->DriverInfo.DriverID==50)//����з��ϵ������ͰѺ�����������
	{/*������ȷ*/
		GetGyroscopeData=SYS_CallMem (sizeof (GyroscopeData),STABLE_MENID);
		PGyroscopeData=PTEMP->DriverInfo.PInfo;
		GetGyroscopeData->Get_X_Angular_Acceleration=PGyroscopeData->Get_X_Angular_Acceleration;
		GetGyroscopeData->Get_Y_Angular_Acceleration=PGyroscopeData->Get_Y_Angular_Acceleration;
		GetGyroscopeData->Get_Z_Angular_Acceleration=PGyroscopeData->Get_Z_Angular_Acceleration;
		GetGyroscopeData->Get_X_Acceleration=PGyroscopeData->Get_X_Acceleration;
		GetGyroscopeData->Get_Y_Acceleration=PGyroscopeData->Get_Y_Acceleration;
		GetGyroscopeData->Get_Z_Acceleration=PGyroscopeData->Get_Z_Acceleration;
	}
	else
	{/*��������*/
		
	}
	for (PTEMP=PDriverPassTree;PTEMP->DriverInfo.DriverID!=51 && PTEMP->PNext!=NULL;PTEMP=PTEMP->PNext);//Ѱ���Ѽ�������
	if (PTEMP->DriverInfo.DriverID==51)//����з��ϵ������ͰѺ�����������
	{/*������ȷ*/
		BrushlessMotordata=SYS_CallMem (sizeof (BrushlessMotorData),STABLE_MENID);
		PBrushlessMotordata=PTEMP->DriverInfo.PInfo;
		BrushlessMotordata->TurnPower=PBrushlessMotordata->TurnPower;
	}
	else
	{/*��������*/
		
	}
}
void stable ()
{
	while (1)
	{
		GetGyroscopeData->Get_X_Angular_Acceleration ();
		BrushlessMotordata->TurnPower (100,20,1);
		vTaskDelay (100);
	}
}
void Stable_Init ()
{
	SYS_CallHEAP (500,STABLE_MENID);
	Stable_LoadFunction ();
	xTaskCreate(stable,"test",50,NULL,1,NULL);
}
