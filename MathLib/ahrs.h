/*-----------------------------------------------------------------------------------
										STM32F401+MPU6050+AHRS�㷨ʵ��
	
	���: ͨ����Ԫ����������Ѽ��ٶȼƺ������ǵ����ݽ����ںϼ����ŷ����.
			 
	����: BoX
	�汾: V1.0
	ƽ̨: STM32F401RE Nucleo Board
	����: 2014��7��6��
------------------------------------------------------------------------------------*/
#ifndef __AHRS__H__
#define __AHRS__H__

#include "stm32f10x.h"
#include "math.h"
/* include "arm_math.h" */

/*------------------------------------------
						�궨����ѧ����
------------------------------------------*/
#define squa( Sq )      (((float)Sq)*((float)Sq))
#define toRad( Math_D )	((float)(Math_D)*0.0174532925f)
#define toDeg( Math_R )	((float)(Math_R)*57.2957795f)
	
#define MPU6050_SAMPLERATE_FEQ  	 ((uint16_t)500) // 500Hz
#define MPU6050_SAMPLERATE_PRI  	 ((float)0.005f) // 2.0ms
#define MPU6050_SAMPLERATE_HALFPRI ((float)0.01f) // 1.0ms
/*------------------------------------------
							ŷ���ǽṹ��
------------------------------------------*/
typedef volatile struct 
{
  float Pitch;
  float Roll;
  float Yaw;
}AHRS_EulerAngleTypeDef;
/*------------------------------------------
							��Ԫ���ṹ��
------------------------------------------*/
typedef volatile struct 
{
  float q0;
  float q1;
  float q2;
  float q3;
}AHRS_QuaternionTypeDef;
/*------------------------------------------
							��������
------------------------------------------*/
AHRS_EulerAngleTypeDef IMUupdate2(float * AGBuff);
AHRS_QuaternionTypeDef IMUupdate3(float * AGBuff);

#endif

