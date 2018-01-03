/*-----------------------------------------------------------------------------------
										STM32F401+MPU6050+AHRS算法实现
	
	简介: 通过四元数矩阵运算把加速度计和陀螺仪的数据进行融合计算出欧拉角.
			 
	作者: BoX
	版本: V1.0
	平台: STM32F401RE Nucleo Board
	日期: 2014年7月6日
------------------------------------------------------------------------------------*/
#ifndef __AHRS__H__
#define __AHRS__H__

#include "stm32f10x.h"
#include "math.h"
/* include "arm_math.h" */

/*------------------------------------------
						宏定义数学运算
------------------------------------------*/
#define invSqrtf( iSq ) (1.0f/sqrtf((float)iSq))
#define squa( Sq )      (((float)Sq)*((float)Sq))
#define toRad( Math_D )	((float)(Math_D)*0.0174532925f)
#define toDeg( Math_R )	((float)(Math_R)*57.2957795f)
	
/* 采样率500Hz,利用中断定时来保证,当动态范围大的时候可适当提高采样率 */
#define MPU6050_SAMPLERATE_FEQ  	 ((uint16_t)500) // 500Hz
#define MPU6050_SAMPLERATE_PRI  	 ((float)0.005f) // 2.0ms
#define MPU6050_SAMPLERATE_HALFPRI ((float)0.0025f) // 1.0ms
/*------------------------------------------
							欧拉角结构体
------------------------------------------*/
typedef volatile struct 
{
  float Pitch;
  float Roll;
  float Yaw;
}AHRS_EulerAngleTypeDef;
/*------------------------------------------
							四元数结构体
------------------------------------------*/
typedef volatile struct 
{
  float q0;
  float q1;
  float q2;
  float q3;
}AHRS_QuaternionTypeDef;
/*------------------------------------------
							函数声明
------------------------------------------*/
void Euler_ToQuaternion(AHRS_QuaternionTypeDef *pQ, AHRS_EulerAngleTypeDef *pE);
void Quaternion_ToEuler(AHRS_QuaternionTypeDef *pQ, AHRS_EulerAngleTypeDef *pE);
void Quaternion_Normalize(AHRS_QuaternionTypeDef *pQ);
void RungeKutta(AHRS_QuaternionTypeDef *pQ, float GyrX, float GyrY, float GyrZ, float helfTimes);
void AHRS_Init(AHRS_QuaternionTypeDef *pQ, AHRS_EulerAngleTypeDef *pE);
AHRS_EulerAngleTypeDef AHRS_Update(float * AGBuff);
AHRS_QuaternionTypeDef Quaternion_Multiply(AHRS_QuaternionTypeDef NowQ , AHRS_QuaternionTypeDef OldQ);
AHRS_EulerAngleTypeDef IMUupdate(float * AGBuff);
AHRS_EulerAngleTypeDef IMUupdate2(float * AGBuff);
AHRS_QuaternionTypeDef IMUupdate3(float * AGBuff);

#endif

