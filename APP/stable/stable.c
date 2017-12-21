#include "stable.h"
#include "math.h"
/**/
GyroscopeData* GetGyroscopeData;
BrushlessMotorData* BrushlessMotordata;
/*�ڴ�ID*/
uint8_t MENID;
/*���ٶȽ��ٶȱ���*/
float ABuff[3]={0},GBuff[3]={0};
/*��Ԫ������*/
#define x 0
#define y 1
#define z 2
#define Kp 100.0f                        // ��������֧�������������ٶȼ�/��ǿ��
#define Ki 0.002f                // ��������֧���ʵ�������ƫ�����ν�
#define halfT 0.001f                // �������ڵ�һ��
float q0 = 1, q1 = 0, q2 = 0, q3 = 0;          // ��Ԫ����Ԫ�أ�������Ʒ���
float exInt = 0, eyInt = 0, ezInt = 0;        // ��������С�������
float Yaw,Pitch,Roll;  //ƫ���ǣ������ǣ�������
/**/
void Stable_LoadFunction ()
{
	DriverTreeData* PTEMP;
	GyroscopeData* PGyroscopeData;
	BrushlessMotorData* PBrushlessMotordata;
	for (PTEMP=PDriverPassTree;PTEMP->DriverInfo.DriverID!=50 && PTEMP->PNext!=NULL;PTEMP=PTEMP->PNext);//Ѱ���Ѽ�������
	if (PTEMP->DriverInfo.DriverID==50)//����з��ϵ������ͰѺ�����������
	{/*������ȷ*/
		GetGyroscopeData=SYS_CallMem (sizeof (GyroscopeData),MENID);
		PGyroscopeData=PTEMP->DriverInfo.PInfo;
		GetGyroscopeData->Get_ACCEL=PGyroscopeData->Get_ACCEL;
		GetGyroscopeData->Get_GYRO=PGyroscopeData->Get_GYRO;
	}
	else
	{/*��������*/
		
	}
	for (PTEMP=PDriverPassTree;PTEMP->DriverInfo.DriverID!=51 && PTEMP->PNext!=NULL;PTEMP=PTEMP->PNext);//Ѱ���Ѽ�������
	if (PTEMP->DriverInfo.DriverID==51)//����з��ϵ������ͰѺ�����������
	{/*������ȷ*/
		BrushlessMotordata=SYS_CallMem (sizeof (BrushlessMotorData),MENID);
		PBrushlessMotordata=PTEMP->DriverInfo.PInfo;
		BrushlessMotordata->TurnPower=PBrushlessMotordata->TurnPower;
	}
	else
	{/*��������*/
		
	}
}
void drawpic(WDID id,float Yaw,float Pitch,float Roll)
{
	static float px=0,py;
	if (px>=240){GUI_2DLib_Clear (id,0xffff);px=0;}
	else px+=0.1;
	py=50+Roll*0.56f;
	GUI_2DLib_DrawPoint (id,px,py,0x7800);//roll��
	py=150+Pitch*0.56f;
	GUI_2DLib_DrawPoint (id,px,py,0x07E0);//Pitch��
	py=250+Yaw*0.56f;
	GUI_2DLib_DrawPoint (id,px,py,0x001F);//Yaw��
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
		GetGyroscopeData->Get_ACCEL (ABuff);//�õ�xyz���ٶ�
		GetGyroscopeData->Get_GYRO (GBuff);//�õ�xyz�Ǽ��ٶ�
		IMUupdate(ABuff,GBuff);//��Ԫ����̬�ں�
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
        // ����������
        norm = sqrt(ABuff[x]*ABuff[x] + ABuff[y]*ABuff[y] + ABuff[z]*ABuff[z]);      
        ABuff[x] = ABuff[x] / norm;                   //��λ��
        ABuff[y] = ABuff[y] / norm;
        ABuff[z] = ABuff[z] / norm;      
        // ���Ʒ��������
        vx = 2*(q1*q3 - q0*q2);
        vy = 2*(q0*q1 + q2*q3);
        vz = q0*q0 - q1*q1 - q2*q2 + q3*q3;
        // ���������ͷ��򴫸��������ο�����֮��Ľ���˻����ܺ�
        ex = (ABuff[y]*vz - ABuff[z]*vy);
        ey = (ABuff[z]*vx - ABuff[x]*vz);
        ez = (ABuff[x]*vy - ABuff[y]*vx);
        // ������������������
        exInt = exInt + ex*Ki;
        eyInt = eyInt + ey*Ki;
        ezInt = ezInt + ez*Ki;
        // ������������ǲ���
        GBuff[x] = GBuff[x] + Kp*ex + exInt;
        GBuff[y] = GBuff[y] + Kp*ey + eyInt;
				GBuff[z] = GBuff[z] + Kp*ez + ezInt;
        // ������Ԫ���ʺ�������
        q0 = q0 + (-q1*GBuff[x] - q2*GBuff[y] - q3*GBuff[z])*halfT;
        q1 = q1 + (q0*GBuff[x] + q2*GBuff[z] - q3*GBuff[y])*halfT;
        q2 = q2 + (q0*GBuff[y] - q1*GBuff[z] + q3*GBuff[x])*halfT;
        q3 = q3 + (q0*GBuff[z] + q1*GBuff[y] - q2*GBuff[x])*halfT;  
        // ��������Ԫ
        norm = sqrt(q0*q0 + q1*q1 + q2*q2 + q3*q3);
        q0 = q0 / norm;
        q1 = q1 / norm;
        q2 = q2 / norm;
        q3 = q3 / norm;
        Pitch  = asin(-2 * q1 * q3 + 2 * q0* q2)* 57.3f; // pitch ,ת��Ϊ����
        Roll = atan2(2 * q2 * q3 + 2 * q0 * q1, -2 * q1 * q1 - 2 * q2* q2 + 1)* 57.3f; // roll
        Yaw = atan2(2*(q1*q2 + q0*q3),q0*q0+q1*q1-q2*q2-q3*q3) * 57.3f;                // yaw
}
void Stable_Init ()
{
	MENID=SYS_CallHEAP (500);
	Stable_LoadFunction ();
	xTaskCreate(stable,"test",100,NULL,1,NULL);
}
