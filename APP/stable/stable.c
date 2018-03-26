#include "stable.h"
#include "ahrs.h"
/*设备句柄*/
DEV_HANDLE * AGhandle,* BLhandle,* SChandle;
/*内存ID*/
uint8_t MENID;
/*加速度角速度变量*/
float AGBuff[6]={0};
AHRS_EulerAngleTypeDef RPY={0};
AHRS_QuaternionTypeDef Q;
/**/
void Stable_OpenDevices ()
{
	char* DriverName;
	DeviceNeedFormate n1,n2,n3;
	DriverName="mpu6050";
	n1.Data=(uint32_t)GPIOB;
	n1.NextNeed=&n2;
	n2.Data=GPIO_Pin_9;
	n2.NextNeed=&n3;
	n3.Data=GPIO_Pin_8;
	n3.NextNeed=NULL;
	SYS_DEV_CREAT (DriverName,&n1);
	DriverName="ili9341";
	SYS_DEV_CREAT (DriverName,&n1);
	while (1)
	{
		SChandle=SYS_DEV_OPEN ("screen",1);
		AGhandle=SYS_DEV_OPEN ("senser",1);
		//BLhandle=SYS_DEV_OPEN ("breashlessmoto",1);
		if (AGhandle && SChandle) 
		{
			stable ();
			//xTaskCreate(stable,"test",100,NULL,4,NULL);
		}
	}
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
void zLine_Midpoint(int x1, int y1, int x2, int y2, int color)
{
	point_str point;
	int x = x1, y = y1;
	int a = y1 - y2, b = x2 - x1;
	int cx = (b >= 0 ? 1 : (b = -b, -1));
	int cy = (a <= 0 ? 1 : (a = -a, -1));
	point.X=x;point.Y=y;point.POINT_COLOR=color;
	SYS_DEV_CONTROL (SChandle,ILI9341_DRAW_POINT,&point,NULL);
	int d, d1, d2;
	if (-a <= b)     // 斜率绝对值 <= 1
	{
		if (x1>x2)
		{
			while (0);
		}
		d = 2 * a + b;
		d1 = 2 * a;
		d2 = 2 * (a + b);
		while(x != x2)
		{
			if (d < 0) y += cy,d += d2;
			else d += d1;
			x += cx;
			point.X=x;point.Y=y;
			SYS_DEV_CONTROL (SChandle,ILI9341_DRAW_POINT,&point,NULL);
		}
	}
	else                // 斜率绝对值 > 1
	{
		d = 2 * b + a; 
		d1 = 2 * b;
		d2 = 2 * (a + b);
		while(y != y2) 
		{ 
			if(d < 0) d += d1; 
			else x += cx, d += d2; 
			y += cy; 
			point.X=x;point.Y=y;
			SYS_DEV_CONTROL (SChandle,ILI9341_DRAW_POINT,&point,NULL);
		} 
	}
}
void D3D2()
{
	uint16_t mark,*pmark;
	pmark=&mark;
	static float p1[3],p2[3],p3[3],p4[3],p5[3];
	float static temp1[2]={0},temp2[2]={0},temp3[2]={0},temp4[2]={0},temp5[2]={0};
	float O,nx,ny,nz;
	float temp;

	while (1)
	{
		*pmark=uxTaskGetStackHighWaterMark(NULL);
		vTaskSuspendAll ();
		zLine_Midpoint (temp1[0]+120,temp1[1]+160,temp2[0]+120,temp2[1]+160,0xffff);
		zLine_Midpoint (temp2[0]+120,temp2[1]+160,temp3[0]+120,temp3[1]+160,0xffff);
		zLine_Midpoint (temp2[0]+120,temp2[1]+160,temp5[0]+120,temp5[1]+160,0xffff);
		zLine_Midpoint (temp2[0]+120,temp2[1]+160,temp4[0]+120,temp4[1]+160,0xffff);
		zLine_Midpoint (temp1[0]+120,temp1[1]+160,temp3[0]+120,temp3[1]+160,0xffff);
		zLine_Midpoint (temp3[0]+120,temp3[1]+160,temp5[0]+120,temp5[1]+160,0xffff);
		zLine_Midpoint (temp5[0]+120,temp5[1]+160,temp1[0]+120,temp1[1]+160,0xffff);
		
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
		
		temp1[0]=p1[1];temp1[1]=p1[2];
		temp2[0]=p2[1];temp2[1]=p2[2];
		temp3[0]=p3[1];temp3[1]=p3[2];
		temp4[0]=p4[1];temp4[1]=p4[2];
		temp5[0]=p5[1];temp5[1]=p5[2];
		
		zLine_Midpoint (temp1[0]+120,temp1[1]+160,temp2[0]+120,temp2[1]+160,0x7800);
		zLine_Midpoint (temp2[0]+120,temp2[1]+160,temp3[0]+120,temp3[1]+160,0x7800);
		zLine_Midpoint (temp2[0]+120,temp2[1]+160,temp5[0]+120,temp5[1]+160,0x7800);
		zLine_Midpoint (temp2[0]+120,temp2[1]+160,temp4[0]+120,temp4[1]+160,0x7800);
		zLine_Midpoint (temp1[0]+120,temp1[1]+160,temp3[0]+120,temp3[1]+160,0x7800);
		zLine_Midpoint (temp3[0]+120,temp3[1]+160,temp5[0]+120,temp5[1]+160,0x7800);
		zLine_Midpoint (temp5[0]+120,temp5[1]+160,temp1[0]+120,temp1[1]+160,0x7800);
		xTaskResumeAll ();
		vTaskDelay (50);
	}
}
void stable ()
{
	int i=0;
	xTaskCreate(D3D2,"test",500,NULL,3,NULL);
	while (1)
	{
		SYS_DEV_CONTROL (AGhandle,MPU6050_RA_ACCEL_GYRO,AGBuff,sizeof (AGBuff));//得到xyz加速度//得到xyz角加速度
		Q=IMUupdate3(AGBuff);
		vTaskDelay (5);
	}
}
void Stable_Init ()
{
	MENID=SYS_CallHEAP (500);
	xTaskCreate(Stable_OpenDevices,"test",100,NULL,4,NULL);
}
