#ifndef __DRI_DEV_H
#define __DRI_DEV_H
#include <stm32f10x.h>
#include "stdlib.h"
/*添加驱动头文件*/
#include "mpu6050.h"
#include "tft_ili9341.h"
/*状态枚举类型*/
typedef enum
{
	DEV_SUCCESS=NULL,
	DEV_FAIL=1,
	DEV_OPEN=2,
	DEV_CLOSE=3,
	DEV_BUSY=4
} DEV_State;
/*设备需求*/
typedef struct DeviceNeedFormate
{
	char * NeedName;
	uint32_t Data;
	struct DeviceNeedFormate * NextNeed;
} DeviceNeedFormate;
/*设备句柄*/
typedef struct DEV_HANDLE
{
	void * DevicePointer;								//指向设备
	DeviceNeedFormate * DriverNeed;				//设备需求
	DEV_State (* drv_DEV_OPEN)();					
	DEV_State (* drv_DEV_CLOSE)();
	DEV_State (* drv_DEV_CONTROL)(struct DEV_HANDLE * Handle,uint16_t CMD,void * Buffer,uint32_t Buffersize);//驱动通用控制函数
} DEV_HANDLE;
/**********************************/
/*设备链表*/
typedef struct DeviceFormate
{
	DEV_HANDLE * Handle;//设备句柄
	uint8_t DeviceID;
	DEV_State	STATE;  //设备状态
	struct DeviceFormate * PNextDevice; //下一个设备
} DeviceFormate;
/*设备类型链表*/
typedef struct DeviceTypeFormate
{
	char * DeviceTypeName; //类型名字
	struct DeviceFormate* PDevice;//指向设备
	struct DeviceTypeFormate* PNextType;//指向下一个设备类型
} DeviceTypeFormate;
/*			________				*/
/*			||||||||				*/
/*			||||||||				*/
/*up设备链表长这个样子up*/
/********************************************/
/*驱动链表*/
typedef struct DriverListFormate
{
	char * DriverName;//驱动名
	DeviceNeedFormate * DriverNeed;
	DEV_HANDLE * (* drv_RegisterDevice)(DeviceNeedFormate * DriverNeed);//驱动注册设备函数
	struct DriverListFormate * NextDriver;//下一个驱动
} DriverListFormate;
/*驱动注册结构*/
typedef struct DriverREGFormate
{
	char * DriverName;//驱动名
	DEV_HANDLE * (* drv_RegisterDevice)(DeviceNeedFormate * DriverNeed);//驱动注册设备函数
	DeviceNeedFormate * Need;//驱动需求
} DriverREGFormate;
/*驱动功能接口*/
typedef struct DriverFUNFormate
{
	DEV_State (* drv_DEV_OPEN)();
	DEV_State (* drv_DEV_CLOSE)();
	DEV_State (* drv_DEV_CONTROL)(DEV_HANDLE * Handle,uint16_t CMD,void * Buffer,uint32_t Buffersize);
} DriverFUNFormate;
/*设备注册结构*/
typedef struct DeviceREGFormate
{
	char * DeviceType;
	uint8_t DeviceID;
	DeviceNeedFormate * Needs;
	DriverFUNFormate FUN;
} DeviceREGFormate;
/*驱动*/
DriverListFormate * SYS_DRI_ReturnDriverList (void);
void SYS_DRI_RegisterDriver (DriverREGFormate DriverREG);
DEV_HANDLE * SYS_DRI_RegisterDevice (DeviceREGFormate DeviceREG);
void SYS_DRI_Init (void);
/*设备*/
DeviceFormate * SYS_DEV_FindNULLDeviceTree (char * DeviceType);
void SYS_DEV_ChangeState (DEV_HANDLE * Handle,DEV_State State);
DEV_State SYS_DEV_GetState (DEV_HANDLE * Handle);
DEV_HANDLE * SYS_DEV_CREAT (char * DriverName,DeviceNeedFormate * DeviceNeed);
DEV_HANDLE * SYS_DEV_OPEN (char * DeviceTypeName,uint8_t DeviceID);
DEV_State SYS_DEV_CLOSE (DEV_HANDLE * Handle);
DEV_State SYS_DEV_CONTROL (DEV_HANDLE * Handle,uint16_t CMD,void * Buffer,uint32_t Buffersize);
#endif
