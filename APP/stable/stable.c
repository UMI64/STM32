#include "stable.h"
#include "ahrs.h"
/**/
GyroscopeData* GetGyroscopeData;
BrushlessMotorData* BrushlessMotordata;
/*设备句柄*/
DEV_HANDLE * AGhandle,* BLhandle;
/*内存ID*/
uint8_t MENID;
/*加速度角速度变量*/
float AGBuff[6]={0};
AHRS_EulerAngleTypeDef RPY={0};
AHRS_QuaternionTypeDef Q;
/**/
void Stable_LoadFunction ()
{
	DriverTreeData* PTEMP;
	BrushlessMotorData* PBrushlessMotordata;
	for (PTEMP=PDriverPassTree;PTEMP->DriverInfo.DriverID!=51 && PTEMP->PNext!=NULL;PTEMP=PTEMP->PNext);//寻找已加载驱动
	if (PTEMP->DriverInfo.DriverID==51)//如果有符合的驱动就把函数链接起来
	{/*驱动正确*/
		BrushlessMotordata=SYS_CallMem (sizeof (BrushlessMotorData),MENID);
		PBrushlessMotordata=PTEMP->DriverInfo.PInfo;
		BrushlessMotordata->TurnPower=PBrushlessMotordata->TurnPower;
	}
	else
	{/*驱动错误*/
		
	}
}
void Stable_OpenDevices ()
{
	char* DeviceName;
	DeviceName="mpu6050";
	while (1)
	{
		AGhandle=SYS_DEV_OPEN (DeviceName);
		if (AGhandle) 
		{
			xTaskCreate(stable,"test",300,NULL,1,NULL);
			vTaskDelete(NULL);
			break;
		}
	}
}
void turny (float turn,float * p)
{
	float D,r2;
	D=atan2(p[2],p[0]);
	r2=p[2]*p[2]+p[0]*p[0];
	D+=turn*0.0174533f;
	r2=sqrtf(r2);
	p[2]=r2*sin(D);
	p[0]=r2*cos(D);
}
void turnx (float turn,float * p)
{
	float D,r2;
	D=atan2(p[2],p[1]);
	r2=p[2]*p[2]+p[1]*p[1];
	D+=turn*0.0174533f;
	r2=sqrtf(r2);
	p[2]=r2*sin(D);
	p[1]=r2*cos(D);
}
void turnz (float turn,float * p)
{
	float D,r2;
	D=atan2(p[0],p[1]);
	r2=p[0]*p[0]+p[1]*p[1];
	D+=turn*0.0174533f;
	r2=sqrtf(r2);
	p[0]=r2*sin(D);
	p[1]=r2*cos(D);
}
void D3D(WDID id,float Yaw,float Pitch,float Roll)
{
	static float p1[3],p2[3],p3[3],p4[3],p5[3];
	float static temp1[2]={0},temp2[2]={0},temp3[2]={0},temp4[2]={0},temp5[2]={0};
	
	GUI_2DLib_DrawLine (id,temp1[0]+120,temp1[1]+160,temp2[0]+120,temp2[1]+160,0xffff);
	GUI_2DLib_DrawLine (id,temp2[0]+120,temp2[1]+160,temp3[0]+120,temp3[1]+160,0xffff);
	GUI_2DLib_DrawLine (id,temp2[0]+120,temp2[1]+160,temp5[0]+120,temp5[1]+160,0xffff);
	GUI_2DLib_DrawLine (id,temp2[0]+120,temp2[1]+160,temp4[0]+120,temp4[1]+160,0xffff);
	
	p1[0]=0;p1[1]=-40;p1[2]=130;
	p2[0]=0;p2[1]=0;p2[2]=150;
	p3[0]=0;p3[1]=40;p3[2]=130;
	p4[0]=0;p4[1]=0;p4[2]=0;
	p5[0]=-40;p5[1]=0;p5[2]=130;
	turny (Pitch,p1);turny (Pitch,p2);turny (Pitch,p3);turny (Pitch,p4);turny (Pitch,p5);
	turnx (Roll,p1);turnx (Roll,p2);turnx (Roll,p3);turnx (Roll,p4);turnx (Roll,p5);
	turnz (Yaw,p1);turnz (Yaw,p2);turnz (Yaw,p3);turnz (Yaw,p4);turnz (Yaw,p5);
	temp1[0]=p1[1];temp1[1]=p1[2];
	temp2[0]=p2[1];temp2[1]=p2[2];
	temp3[0]=p3[1];temp3[1]=p3[2];
	temp4[0]=p4[1];temp4[1]=p4[2];
	temp5[0]=p5[1];temp5[1]=p5[2];
	vTaskSuspendAll ();
	GUI_2DLib_DrawLine (id,temp1[0]+120,temp1[1]+160,temp2[0]+120,temp2[1]+160,0x7800);
	GUI_2DLib_DrawLine (id,temp2[0]+120,temp2[1]+160,temp3[0]+120,temp3[1]+160,0x7800);
	GUI_2DLib_DrawLine (id,temp2[0]+120,temp2[1]+160,temp5[0]+120,temp5[1]+160,0x7800);
	GUI_2DLib_DrawLine (id,temp2[0]+120,temp2[1]+160,temp4[0]+120,temp4[1]+160,0x7800);
	xTaskResumeAll ();
}
void turn (float p[3],float x,float y,float z,float O)
{
	float TurnMatrix[3][3]={(0)};
	float px,py,pz;
	px= p[0];
	py= p[1];
	pz= p[2];

  TurnMatrix[0][0] = cosf(O) + x * x * (1 - cosf(O));
  TurnMatrix[0][1] = x * y * (1 - cosf(O)) + z * sinf(O);
  TurnMatrix[0][2] = x * z * (1 - cosf(O)) - y * sinf(O);

  TurnMatrix[1][0] = x * y * (1 - cosf(O)) - z * sinf(O);
  TurnMatrix[1][1] = cosf(O) + y * y * (1 - cosf(O));
  TurnMatrix[1][2] = z * y * (1 - cosf(O)) + x * sinf(O);

  TurnMatrix[2][0] = x * z * (1 - cosf(O)) + y * sinf(O);
  TurnMatrix[2][1] = y * z * (1 - cosf(O)) - x * sinf(O);
	TurnMatrix[2][2] = cosf(O) + z * z * (1 - cosf(O));
	
	p[0]=px * TurnMatrix[0][0] + py * TurnMatrix[0][1] + pz* TurnMatrix[0][2];
	p[1]=px * TurnMatrix[1][0] + py * TurnMatrix[1][1] + pz * TurnMatrix[1][2];
	p[2]=px * TurnMatrix[2][0] + py * TurnMatrix[2][1] + pz * TurnMatrix[2][2];
}
void D3D2(WDID id,AHRS_QuaternionTypeDef Q)
{
	static float p1[3],p2[3],p3[3],p4[3],p5[3];
	float static temp1[2]={0},temp2[2]={0},temp3[2]={0},temp4[2]={0},temp5[2]={0};
	float O,nx,ny,nz;
	float temp;
	
	/**/
	p1[0]=10;
	p1[1]=0;
	p1[2]=0;
	turn (p1,0,0,1,1.5707963f);
	GUI_2DLib_DrawLine (id,p1[1]+120,p1[2]+160,120,160,0x7800);
	/**/
	
	GUI_2DLib_DrawLine (id,temp1[0]+120,temp1[1]+160,temp2[0]+120,temp2[1]+160,0xffff);
	GUI_2DLib_DrawLine (id,temp2[0]+120,temp2[1]+160,temp3[0]+120,temp3[1]+160,0xffff);
	GUI_2DLib_DrawLine (id,temp2[0]+120,temp2[1]+160,temp5[0]+120,temp5[1]+160,0xffff);
	GUI_2DLib_DrawLine (id,temp2[0]+120,temp2[1]+160,temp4[0]+120,temp4[1]+160,0xffff);
	GUI_2DLib_DrawLine (id,temp1[0]+120,temp1[1]+160,temp3[0]+120,temp3[1]+160,0xffff);
	GUI_2DLib_DrawLine (id,temp3[0]+120,temp3[1]+160,temp5[0]+120,temp5[1]+160,0xffff);
	GUI_2DLib_DrawLine (id,temp5[0]+120,temp5[1]+160,temp1[0]+120,temp1[1]+160,0xffff);
	
	p1[0]=0;p1[1]=-20;p1[2]=60;
	p2[0]=0;p2[1]=0;p2[2]=100;
	p3[0]=0;p3[1]=20;p3[2]=60;
	p4[0]=0;p4[1]=0;p4[2]=0;
	p5[0]=-20;p5[1]=0;p5[2]=60;
	O=2.0f * acos(Q.q0);// * 57.2957f;
	temp=sin(O/2.0f);
	nx=Q.q1 / temp;
	ny=Q.q2 / temp;
	nz=Q.q3 / temp;
	turn (p1,nx,ny,nz,O);
	turn (p2,nx,ny,nz,O);
	turn (p3,nx,ny,nz,O);
	turn (p4,nx,ny,nz,O);
	turn (p5,nx,ny,nz,O);
	
//	GUI_2DLib_DrawPoint (id,temp1[0]+120,temp1[1]+160,0xffff);
//	GUI_2DLib_DrawPoint (id,temp2[0]+120,temp2[1]+160,0xffff);
//	GUI_2DLib_DrawPoint (id,temp3[0]+120,temp3[1]+160,0xffff);
//	GUI_2DLib_DrawPoint (id,temp5[0]+120,temp5[1]+160,0xffff);
	
	temp1[0]=p1[1];temp1[1]=p1[2];
	temp2[0]=p2[1];temp2[1]=p2[2];
	temp3[0]=p3[1];temp3[1]=p3[2];
	temp4[0]=p4[1];temp4[1]=p4[2];
	temp5[0]=p5[1];temp5[1]=p5[2];
	
	vTaskSuspendAll ();
//	GUI_2DLib_DrawPoint (id,temp1[0]+120,temp1[1]+160,0x7800);
//	GUI_2DLib_DrawPoint (id,temp2[0]+120,temp2[1]+160,0x7800);
//	GUI_2DLib_DrawPoint (id,temp3[0]+120,temp3[1]+160,0x7800);
//	GUI_2DLib_DrawPoint (id,temp5[0]+120,temp5[1]+160,0x7800);
	GUI_2DLib_DrawLine (id,temp1[0]+120,temp1[1]+160,temp2[0]+120,temp2[1]+160,0x7800);
	GUI_2DLib_DrawLine (id,temp2[0]+120,temp2[1]+160,temp3[0]+120,temp3[1]+160,0x7800);
	GUI_2DLib_DrawLine (id,temp2[0]+120,temp2[1]+160,temp5[0]+120,temp5[1]+160,0x7800);
	GUI_2DLib_DrawLine (id,temp2[0]+120,temp2[1]+160,temp4[0]+120,temp4[1]+160,0x7800);
	GUI_2DLib_DrawLine (id,temp1[0]+120,temp1[1]+160,temp3[0]+120,temp3[1]+160,0x7800);
	GUI_2DLib_DrawLine (id,temp3[0]+120,temp3[1]+160,temp5[0]+120,temp5[1]+160,0x7800);
	GUI_2DLib_DrawLine (id,temp5[0]+120,temp5[1]+160,temp1[0]+120,temp1[1]+160,0x7800);
	xTaskResumeAll ();
}
void drawpic(WDID id,float Yaw,float Pitch,float Roll,AHRS_QuaternionTypeDef Q)
{
//	static float px=0,py;
//	if (px>=240){GUI_2DLib_Clear (id,0xffff);px=0;}
//	else px+=0.1;
//	py=50+Roll*0.28f;
//	GUI_2DLib_DrawPoint (id,px,py,0x7800);//roll红
//	py=150+Pitch*0.28f;
//	GUI_2DLib_DrawPoint (id,px,py,0x07E0);//Pitch绿
//	py=250+Yaw*0.28f;
//	GUI_2DLib_DrawPoint (id,px,py,0x001F);//Yaw蓝
//	D3D (id,RPY.Yaw,RPY.Pitch,RPY.Roll);
	D3D2(id,Q);
}
void stable ()
{
	int i=0;
	WDFormat WD;
	WDID id;
	WD.Length=320;
	WD.Width=240;
	WD.XPosition=0;
	WD.YPosition=0;
	id=GUI_WINDOW_CreatWD(WD);
	while (1)
	{
		int *q=&i;
		*q=uxTaskGetStackHighWaterMark(NULL);
		SYS_DEV_CONTROL (AGhandle,MPU6050_RA_ACCEL_GYRO,AGBuff,sizeof (AGBuff));//得到xyz加速度//得到xyz角加速度
//		RPY=IMUupdate2(AGBuff);
		Q=IMUupdate3(AGBuff);
		drawpic(id,RPY.Yaw,RPY.Pitch,RPY.Roll,Q);
		BrushlessMotordata->TurnPower (100,(uint8_t)(RPY.Roll*0.6),1);
		vTaskDelay (5);
	}
}
void Stable_Init ()
{
	MENID=SYS_CallHEAP (500);
	Stable_LoadFunction ();
	xTaskCreate(Stable_OpenDevices,"test",30,NULL,1,NULL);
}
