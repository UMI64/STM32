#ifndef __SYS_DEVIC_H
#define __SYS_DEVIC_H
#include "SYS.h"
#include "SYS_Device.h"

typedef enum
{
	PASS=1,
	NOTPASS=0
} TESTPASS;
/*驱动信息结构体*/
typedef struct DriverInfoData
{
	uint16_t DriverID;
	TESTPASS (* Test)();
	void* PInfo;
} DriverInfoData;
/*驱动树*/
typedef struct DriverTreeData
{
	DriverInfoData DriverInfo;//驱动信息结构体
	struct DriverTreeData* PFront;//前一个驱动
	struct DriverTreeData* PNext;//后一个驱动
} DriverTreeData;
/*显示屏设备信息结构体*/
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
/*通过测试的驱动列表*/
extern DriverTreeData* PDriverPassTree;//外部调用，通过测试的驱动列表

void SYS_DriverInit(void);
#endif
