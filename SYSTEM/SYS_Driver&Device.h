#ifndef __DRI_DEV_H
#define __DRI_DEV_H
#include <stm32f10x.h>
#include "stdlib.h"
/*�������ͷ�ļ�*/
#include "mpu6050.h"
#include "tft_ili9341.h"
/*״̬ö������*/
typedef enum
{
	DEV_SUCCESS=NULL,
	DEV_FAIL=1,
	DEV_OPEN=2,
	DEV_CLOSE=3,
	DEV_BUSY=4
} DEV_State;
/*�豸����*/
typedef struct DeviceNeedFormate
{
	char * NeedName;
	uint32_t Data;
	struct DeviceNeedFormate * NextNeed;
} DeviceNeedFormate;
/*�豸���*/
typedef struct DEV_HANDLE
{
	void * DevicePointer;								//ָ���豸
	DeviceNeedFormate * DriverNeed;				//�豸����
	DEV_State (* drv_DEV_OPEN)();					
	DEV_State (* drv_DEV_CLOSE)();
	DEV_State (* drv_DEV_CONTROL)(struct DEV_HANDLE * Handle,uint16_t CMD,void * Buffer,uint32_t Buffersize);//����ͨ�ÿ��ƺ���
} DEV_HANDLE;
/**********************************/
/*�豸����*/
typedef struct DeviceFormate
{
	DEV_HANDLE * Handle;//�豸���
	uint8_t DeviceID;
	DEV_State	STATE;  //�豸״̬
	struct DeviceFormate * PNextDevice; //��һ���豸
} DeviceFormate;
/*�豸��������*/
typedef struct DeviceTypeFormate
{
	char * DeviceTypeName; //��������
	struct DeviceFormate* PDevice;//ָ���豸
	struct DeviceTypeFormate* PNextType;//ָ����һ���豸����
} DeviceTypeFormate;
/*			________				*/
/*			||||||||				*/
/*			||||||||				*/
/*up�豸�����������up*/
/********************************************/
/*��������*/
typedef struct DriverListFormate
{
	char * DriverName;//������
	DeviceNeedFormate * DriverNeed;
	DEV_HANDLE * (* drv_RegisterDevice)(DeviceNeedFormate * DriverNeed);//����ע���豸����
	struct DriverListFormate * NextDriver;//��һ������
} DriverListFormate;
/*����ע��ṹ*/
typedef struct DriverREGFormate
{
	char * DriverName;//������
	DEV_HANDLE * (* drv_RegisterDevice)(DeviceNeedFormate * DriverNeed);//����ע���豸����
	DeviceNeedFormate * Need;//��������
} DriverREGFormate;
/*�������ܽӿ�*/
typedef struct DriverFUNFormate
{
	DEV_State (* drv_DEV_OPEN)();
	DEV_State (* drv_DEV_CLOSE)();
	DEV_State (* drv_DEV_CONTROL)(DEV_HANDLE * Handle,uint16_t CMD,void * Buffer,uint32_t Buffersize);
} DriverFUNFormate;
/*�豸ע��ṹ*/
typedef struct DeviceREGFormate
{
	char * DeviceType;
	uint8_t DeviceID;
	DeviceNeedFormate * Needs;
	DriverFUNFormate FUN;
} DeviceREGFormate;
/*����*/
DriverListFormate * SYS_DRI_ReturnDriverList (void);
void SYS_DRI_RegisterDriver (DriverREGFormate DriverREG);
DEV_HANDLE * SYS_DRI_RegisterDevice (DeviceREGFormate DeviceREG);
void SYS_DRI_Init (void);
/*�豸*/
DeviceFormate * SYS_DEV_FindNULLDeviceTree (char * DeviceType);
void SYS_DEV_ChangeState (DEV_HANDLE * Handle,DEV_State State);
DEV_State SYS_DEV_GetState (DEV_HANDLE * Handle);
DEV_HANDLE * SYS_DEV_CREAT (char * DriverName,DeviceNeedFormate * DeviceNeed);
DEV_HANDLE * SYS_DEV_OPEN (char * DeviceTypeName,uint8_t DeviceID);
DEV_State SYS_DEV_CLOSE (DEV_HANDLE * Handle);
DEV_State SYS_DEV_CONTROL (DEV_HANDLE * Handle,uint16_t CMD,void * Buffer,uint32_t Buffersize);
#endif
