#ifndef __SYS_DEVIC_H
#define __SYS_DEVIC_H
#include "SYS.h"

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

/**********************************/
/*�豸����*/
typedef struct DeviceLineFormate
{
	DEV_HANDLE * Handle;
	DEV_State	STATE;
	struct DeviceLineFormate* PNextDevice;
} DeviceLineFormate;
/*�豸ͷ����*/
typedef struct DeviceTreeHeadFormate
{
	char * DeviceName;
	struct DeviceLineFormate* PDeviceLine;
	struct DeviceTreeHeadFormate* PNextHead;
} DeviceTreeHeadFormate;
/*			________				*/
/*			||||||||				*/
/*			||||||||				*/
/*up�豸�����������up*/
extern DeviceTreeHeadFormate * DeviceTree;

DeviceTreeHeadFormate * SYS_DEV_FindNULLTreeHead (void);
void SYS_DEV_ChangeState (DEV_HANDLE * Handle,DEV_State State);
DEV_HANDLE * SYS_DEV_OPEN (char * DeviceName);
DEV_State SYS_DEV_CLOSE (DEV_HANDLE * Handle);
DEV_State SYS_DEV_CONTROL (DEV_HANDLE * Handle,uint16_t CMD,void * Buffer,uint32_t Buffersize);

#endif
