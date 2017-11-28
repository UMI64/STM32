#include "stable.h"
GyroscopeData* GetGyroscopeData;
BrushlessMotorData* BrushlessMotordata;
float ABuff[3]={0},GBuff[3]={0};
void Stable_LoadFunction ()
{
	DriverTreeData* PTEMP;
	GyroscopeData* PGyroscopeData;
	BrushlessMotorData* PBrushlessMotordata;
	for (PTEMP=PDriverPassTree;PTEMP->DriverInfo.DriverID!=50 && PTEMP->PNext!=NULL;PTEMP=PTEMP->PNext);//寻找已加载驱动
	if (PTEMP->DriverInfo.DriverID==50)//如果有符合的驱动就把函数链接起来
	{/*驱动正确*/
		GetGyroscopeData=SYS_CallMem (sizeof (GyroscopeData),STABLE_MENID);
		PGyroscopeData=PTEMP->DriverInfo.PInfo;
		GetGyroscopeData->Get_ACCEL=PGyroscopeData->Get_ACCEL;
		GetGyroscopeData->Get_GYRO=PGyroscopeData->Get_GYRO;
	}
	else
	{/*驱动错误*/
		
	}
	for (PTEMP=PDriverPassTree;PTEMP->DriverInfo.DriverID!=51 && PTEMP->PNext!=NULL;PTEMP=PTEMP->PNext);//寻找已加载驱动
	if (PTEMP->DriverInfo.DriverID==51)//如果有符合的驱动就把函数链接起来
	{/*驱动正确*/
		BrushlessMotordata=SYS_CallMem (sizeof (BrushlessMotorData),STABLE_MENID);
		PBrushlessMotordata=PTEMP->DriverInfo.PInfo;
		BrushlessMotordata->TurnPower=PBrushlessMotordata->TurnPower;
	}
	else
	{/*驱动错误*/
		
	}
}
void stable ()
{
	while (1)
	{
		GetGyroscopeData->Get_ACCEL (ABuff);
		GetGyroscopeData->Get_GYRO (GBuff);
		//BrushlessMotordata->TurnPower (100,80,1);
		//vTaskDelay (10);
	}
}
void Stable_Init ()
{
	SYS_CallHEAP (500,STABLE_MENID);
	Stable_LoadFunction ();
	xTaskCreate(stable,"test",100,NULL,1,NULL);
}
