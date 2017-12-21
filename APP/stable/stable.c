#include "stable.h"
#include "math.h"
/**/
GyroscopeData* GetGyroscopeData;
BrushlessMotorData* BrushlessMotordata;
/*内存ID*/
uint8_t MENID;
/*加速度角速度变量*/
float ABuff[3]={0},GBuff[3]={0};
/*四元数变量*/
#define x 0
#define y 1
#define z 2
#define Kp 100.0f                        // 比例增益支配率收敛到加速度计/磁强计
#define Ki 0.002f                // 积分增益支配率的陀螺仪偏见的衔接
#define halfT 0.001f                // 采样周期的一半
float q0 = 1, q1 = 0, q2 = 0, q3 = 0;          // 四元数的元素，代表估计方向
float exInt = 0, eyInt = 0, ezInt = 0;        // 按比例缩小积分误差
float Yaw,Pitch,Roll;  //偏航角，俯仰角，翻滚角
/**/
void Stable_LoadFunction ()
{
	DriverTreeData* PTEMP;
	GyroscopeData* PGyroscopeData;
	BrushlessMotorData* PBrushlessMotordata;
	for (PTEMP=PDriverPassTree;PTEMP->DriverInfo.DriverID!=50 && PTEMP->PNext!=NULL;PTEMP=PTEMP->PNext);//寻找已加载驱动
	if (PTEMP->DriverInfo.DriverID==50)//如果有符合的驱动就把函数链接起来
	{/*驱动正确*/
		GetGyroscopeData=SYS_CallMem (sizeof (GyroscopeData),MENID);
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
		BrushlessMotordata=SYS_CallMem (sizeof (BrushlessMotorData),MENID);
		PBrushlessMotordata=PTEMP->DriverInfo.PInfo;
		BrushlessMotordata->TurnPower=PBrushlessMotordata->TurnPower;
	}
	else
	{/*驱动错误*/
		
	}
}
void drawpic(WDID id,float Yaw,float Pitch,float Roll)
{
	static float px=0,py;
	if (px>=240){GUI_2DLib_Clear (id,0xffff);px=0;}
	else px+=0.1;
	py=50+Roll*0.56f;
	GUI_2DLib_DrawPoint (id,px,py,0x7800);//roll红
	py=150+Pitch*0.56f;
	GUI_2DLib_DrawPoint (id,px,py,0x07E0);//Pitch绿
	py=250+Yaw*0.56f;
	GUI_2DLib_DrawPoint (id,px,py,0x001F);//Yaw蓝
}
void stable ()
{
	WDFormat WD;
	WDID id;
	WD.Length=320;
	WD.Width=240;
	WD.XPosition=0;
	WD.YPosition=0;
	id=GUI_WINDOW_CreatWD(WD);
	while (1)
	{
		GetGyroscopeData->Get_ACCEL (ABuff);//得到xyz加速度
		GetGyroscopeData->Get_GYRO (GBuff);//得到xyz角加速度
		IMUupdate(ABuff,GBuff);//四元数姿态融合
		drawpic(id,Yaw,Pitch,Roll);
		BrushlessMotordata->TurnPower (100,(uint8_t)(Roll*0.6),1);
		vTaskDelay (10);
	}
}
void IMUupdate(float * ABuff,float * GBuff)
{
        float norm;
        float vx, vy, vz;
        float ex, ey, ez;  
        // 测量正常化
        norm = sqrt(ABuff[x]*ABuff[x] + ABuff[y]*ABuff[y] + ABuff[z]*ABuff[z]);      
        ABuff[x] = ABuff[x] / norm;                   //单位化
        ABuff[y] = ABuff[y] / norm;
        ABuff[z] = ABuff[z] / norm;      
        // 估计方向的重力
        vx = 2*(q1*q3 - q0*q2);
        vy = 2*(q0*q1 + q2*q3);
        vz = q0*q0 - q1*q1 - q2*q2 + q3*q3;
        // 错误的领域和方向传感器测量参考方向之间的交叉乘积的总和
        ex = (ABuff[y]*vz - ABuff[z]*vy);
        ey = (ABuff[z]*vx - ABuff[x]*vz);
        ez = (ABuff[x]*vy - ABuff[y]*vx);
        // 积分误差比例积分增益
        exInt = exInt + ex*Ki;
        eyInt = eyInt + ey*Ki;
        ezInt = ezInt + ez*Ki;
        // 调整后的陀螺仪测量
        GBuff[x] = GBuff[x] + Kp*ex + exInt;
        GBuff[y] = GBuff[y] + Kp*ey + eyInt;
				GBuff[z] = GBuff[z] + Kp*ez + ezInt;
        // 整合四元数率和正常化
        q0 = q0 + (-q1*GBuff[x] - q2*GBuff[y] - q3*GBuff[z])*halfT;
        q1 = q1 + (q0*GBuff[x] + q2*GBuff[z] - q3*GBuff[y])*halfT;
        q2 = q2 + (q0*GBuff[y] - q1*GBuff[z] + q3*GBuff[x])*halfT;
        q3 = q3 + (q0*GBuff[z] + q1*GBuff[y] - q2*GBuff[x])*halfT;  
        // 正常化四元
        norm = sqrt(q0*q0 + q1*q1 + q2*q2 + q3*q3);
        q0 = q0 / norm;
        q1 = q1 / norm;
        q2 = q2 / norm;
        q3 = q3 / norm;
        Pitch  = asin(-2 * q1 * q3 + 2 * q0* q2)* 57.3f; // pitch ,转换为度数
        Roll = atan2(2 * q2 * q3 + 2 * q0 * q1, -2 * q1 * q1 - 2 * q2* q2 + 1)* 57.3f; // roll
        Yaw = atan2(2*(q1*q2 + q0*q3),q0*q0+q1*q1-q2*q2-q3*q3) * 57.3f;                // yaw
}
void Stable_Init ()
{
	MENID=SYS_CallHEAP (500);
	Stable_LoadFunction ();
	xTaskCreate(stable,"test",100,NULL,1,NULL);
}
