#ifndef __SYS_DEVIC_H
#define __SYS_DEVIC_H
#include "SYS.h"
#include "SYS_Device.h"

typedef enum
{
	PASS=1,
	NOTPASS=0
} TESTPASS;
/*������Ϣ�ṹ��*/
typedef struct DriverInfoData
{
	uint16_t DriverID;
	void (* Init)();
	TESTPASS (* Test)();
	void* PInfo;
} DriverInfoData;
/*������*/
typedef struct DriverTreeData
{
	DriverInfoData DriverInfo;//������Ϣ�ṹ��
	struct DriverTreeData* PFront;//ǰһ������
	struct DriverTreeData* PNext;//��һ������
} DriverTreeData;
/*��ʾ���豸�ṹ��*/
typedef struct
{
	u16 id;
	u16 width;
	u16 length;
	u8  color_type;
	void (* DrawPoint)(u16 x,u16 y,u16 POINT_COLOR);
	void (* DrawRange)(u16 x1,u16 y1,u16 x2,u16 y2,u16 Color);
	void (* ReadColor)(u16 x,u16 y,uint16_t *Color);
} LCDData;
/*�Ǽ��ٶȣ����ٶ���Ϣ�ṹ��*/
typedef struct
{
	void (* Get_ACCEL)(float* ACCEL);
	void (* Get_GYRO)(float* GYRO);
} GyroscopeData;
/*����豸�ṹ��*/
typedef struct
{
	void (* TurnPower)(float power,float speed,u8 FB);
} BrushlessMotorData;
/*ͨ�����Ե������б�*/
extern DriverTreeData* PDriverPassTree;//�ⲿ���ã�ͨ�����Ե������б�

void SYS_DriverInit(void);
#endif
