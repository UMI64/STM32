/*-----------------------------------------------------------------------------------
							STM32F103+MPU6050+IMU算法实现
	
	简介: 通过四元数矩阵运算把加速度计和陀螺仪的数据进行融合计算出欧拉角.
			 
	作者: UMI
	版本: V1.0
	平台: STM32F103Z8T6
	日期: 2017年12月11日
------------------------------------------------------------------------------------*/
#include "ahrs.h"
/*------------------------------------------
								IMU参数
------------------------------------------*/
#define Kp 							  15.0f
#define Ki 							  0.02f
/*------------------------------------------
								全局变量
------------------------------------------*/
/* 四元数结构体	*/
AHRS_QuaternionTypeDef  QuadNumber = {1, 0, 0, 0};
/* 欧拉角结构体 */
AHRS_EulerAngleTypeDef  EulerAngle = {0};
float InvSqrt(float x)
{
    float xhalf = 0.5f*x;
    int i = *(int*)&x;       // get bits for floating value
    i = 0x5f3759df - (i>>1); // gives initial guess y0
    x = *(float*)&i;         // convert bits back to float
    x = x*(1.5f-xhalf*x*x);  // Newton step, repeating increases accuracy
    return x;
}
AHRS_EulerAngleTypeDef IMUupdate2(float * AGBuff)
{
	static float AngZ_Temp = 0.0f;
	static float exInt = 0.0f, eyInt = 0.0f, ezInt = 0.0f;
	float norm;
	float vx, vy, vz;
	float ex, ey, ez;  
	float gx, gy, gz;
	float q0q0 = QuadNumber.q0 * QuadNumber.q0;
	float q0q1 = QuadNumber.q0 * QuadNumber.q1;
	float q0q2 = QuadNumber.q0 * QuadNumber.q2;
	float q0q3 = QuadNumber.q0 * QuadNumber.q3;
	float q1q1 = QuadNumber.q1 * QuadNumber.q1;
	float q1q2 = QuadNumber.q1 * QuadNumber.q2;
	float q1q3 = QuadNumber.q1 * QuadNumber.q3;
	float q2q2 = QuadNumber.q2 * QuadNumber.q2;
	float q2q3 = QuadNumber.q2 * QuadNumber.q3;
	float q3q3 = QuadNumber.q3 * QuadNumber.q3;
	// 测量正常化
	norm = InvSqrt(AGBuff[0]*AGBuff[0] + AGBuff[1]*AGBuff[1] + AGBuff[2]*AGBuff[2]);      
	AGBuff[0] = AGBuff[0] * norm;                   //单位化
	AGBuff[1] = AGBuff[1] * norm;
	AGBuff[2] = AGBuff[2] * norm;      
	// 估计方向的重力
	vx = 2*(q1q3 - q0q2);
	vy = 2*(q0q1 + q2q3);
	vz = q0q0 - q1q1 - q2q2 + q3q3;
	// 错误的领域和方向传感器测量参考方向之间的交叉乘积的总和
	ex = (AGBuff[1]*vz - AGBuff[2]*vy);
	ey = (AGBuff[2]*vx - AGBuff[0]*vz);
	ez = (AGBuff[0]*vy - AGBuff[1]*vx);
	// 积分误差比例积分增益
	exInt = exInt + ex*Ki*MPU6050_SAMPLERATE_HALFPRI;
	eyInt = eyInt + ey*Ki*MPU6050_SAMPLERATE_HALFPRI;
	ezInt = ezInt + ez*Ki*MPU6050_SAMPLERATE_HALFPRI;
	// 调整后的陀螺仪测量
	gx = toRad(AGBuff[3]) + Kp*ex + exInt;
	gy = toRad(AGBuff[4]) + Kp*ey + eyInt;
	gz = toRad(AGBuff[5]) + Kp*ez + ezInt;
	// 整合四元数率和正常化
	QuadNumber.q0 = QuadNumber.q0 + (-QuadNumber.q1*gx - QuadNumber.q2*gy - QuadNumber.q3*gz)*MPU6050_SAMPLERATE_HALFPRI;
	QuadNumber.q1 = QuadNumber.q1 + (QuadNumber.q0*gx + QuadNumber.q2*gz - QuadNumber.q3*gy)*MPU6050_SAMPLERATE_HALFPRI;
	QuadNumber.q2 = QuadNumber.q2 + (QuadNumber.q0*gy - QuadNumber.q1*gz + QuadNumber.q3*gx)*MPU6050_SAMPLERATE_HALFPRI;
	QuadNumber.q3 = QuadNumber.q3 + (QuadNumber.q0*gz + QuadNumber.q1*gy - QuadNumber.q2*gx)*MPU6050_SAMPLERATE_HALFPRI;  
	// 正常化四元
	norm = InvSqrt(squa(QuadNumber.q0) + squa(QuadNumber.q1) + squa(QuadNumber.q2) + squa(QuadNumber.q3));
	QuadNumber.q0 = QuadNumber.q0 * norm;
	QuadNumber.q1 = QuadNumber.q1 * norm;
	QuadNumber.q2 = QuadNumber.q2 * norm;
	QuadNumber.q3 = QuadNumber.q3 * norm;
	EulerAngle.Pitch  = asin(- 2 * (QuadNumber.q1* QuadNumber.q3 - QuadNumber.q0 * QuadNumber.q2)); // pitch 
	EulerAngle.Roll = atan2(2 * (QuadNumber.q2 * QuadNumber.q3 + QuadNumber.q0 * QuadNumber.q1),1 - 2 * (squa(QuadNumber.q1) + squa(QuadNumber.q2))); // roll
//	EulerAngle.Yaw = atan2(2*(QuadNumber.q1*QuadNumber.q2 + QuadNumber.q0*QuadNumber.q3),1 - 2 * (squa(QuadNumber.q2) + squa(QuadNumber.q3))); // yaw
	EulerAngle.Pitch=toDeg(EulerAngle.Pitch);
	EulerAngle.Roll=toDeg(EulerAngle.Roll);
//	EulerAngle.Yaw=toDeg(EulerAngle.Yaw);
//	/* 互补滤波 */
//  #define CF_A 1f
//  #define CF_B 0f
  AngZ_Temp = AngZ_Temp + AGBuff[5] * 0.0165f;
//  AngZ_Temp = CF_A * AngZ_Temp + CF_B * EulerAngle.Yaw;
	
  if(AngZ_Temp>180.0f)
    EulerAngle.Yaw = -360.0f - AngZ_Temp;
  else if(AngZ_Temp<-180.0f)
    EulerAngle.Yaw = 360.0f + AngZ_Temp;
  else
    EulerAngle.Yaw = AngZ_Temp;
	return EulerAngle;
}
AHRS_QuaternionTypeDef IMUupdate3(float * AGBuff)
{
	static float exInt = 0.0f, eyInt = 0.0f, ezInt = 0.0f;
	float norm;
	float vx, vy, vz;
	float ex, ey, ez;  
	float gx, gy, gz;
	float q0q0 = QuadNumber.q0 * QuadNumber.q0;
	float q0q1 = QuadNumber.q0 * QuadNumber.q1;
	float q0q2 = QuadNumber.q0 * QuadNumber.q2;
	float q0q3 = QuadNumber.q0 * QuadNumber.q3;
	float q1q1 = QuadNumber.q1 * QuadNumber.q1;
	float q1q2 = QuadNumber.q1 * QuadNumber.q2;
	float q1q3 = QuadNumber.q1 * QuadNumber.q3;
	float q2q2 = QuadNumber.q2 * QuadNumber.q2;
	float q2q3 = QuadNumber.q2 * QuadNumber.q3;
	float q3q3 = QuadNumber.q3 * QuadNumber.q3;
	// 测量正常化
	norm = InvSqrt(AGBuff[0]*AGBuff[0] + AGBuff[1]*AGBuff[1] + AGBuff[2]*AGBuff[2]);      
	AGBuff[0] = AGBuff[0] * norm;                   //单位化
	AGBuff[1] = AGBuff[1] * norm;
	AGBuff[2] = AGBuff[2] * norm;      
	// 估计方向的重力
	vx = 2*(q1q3 - q0q2);
	vy = 2*(q0q1 + q2q3);
	vz = q0q0 - q1q1 - q2q2 + q3q3;
	// 错误的领域和方向传感器测量参考方向之间的交叉乘积的总和
	ex = (AGBuff[1]*vz - AGBuff[2]*vy);
	ey = (AGBuff[2]*vx - AGBuff[0]*vz);
	ez = (AGBuff[0]*vy - AGBuff[1]*vx);
	// 积分误差比例积分增益
	exInt = exInt + ex*Ki*MPU6050_SAMPLERATE_HALFPRI;
	eyInt = eyInt + ey*Ki*MPU6050_SAMPLERATE_HALFPRI;
	ezInt = ezInt + ez*Ki*MPU6050_SAMPLERATE_HALFPRI;
	// 调整后的陀螺仪测量
	gx = toRad(AGBuff[3]) + Kp*ex + exInt;
	gy = toRad(AGBuff[4]) + Kp*ey + eyInt;
	gz = toRad(AGBuff[5]) + Kp*ez + ezInt;
	// 整合四元数率和正常化
	QuadNumber.q0 = QuadNumber.q0 + (-QuadNumber.q1*gx - QuadNumber.q2*gy - QuadNumber.q3*gz)*MPU6050_SAMPLERATE_HALFPRI;
	QuadNumber.q1 = QuadNumber.q1 + (QuadNumber.q0*gx + QuadNumber.q2*gz - QuadNumber.q3*gy)*MPU6050_SAMPLERATE_HALFPRI;
	QuadNumber.q2 = QuadNumber.q2 + (QuadNumber.q0*gy - QuadNumber.q1*gz + QuadNumber.q3*gx)*MPU6050_SAMPLERATE_HALFPRI;
	QuadNumber.q3 = QuadNumber.q3 + (QuadNumber.q0*gz + QuadNumber.q1*gy - QuadNumber.q2*gx)*MPU6050_SAMPLERATE_HALFPRI;  
	// 正常化四元
	norm = InvSqrt(squa(QuadNumber.q0) + squa(QuadNumber.q1) + squa(QuadNumber.q2) + squa(QuadNumber.q3));
	QuadNumber.q0 = QuadNumber.q0 * norm;
	QuadNumber.q1 = QuadNumber.q1 * norm;
	QuadNumber.q2 = QuadNumber.q2 * norm;
	QuadNumber.q3 = QuadNumber.q3 * norm;
	return QuadNumber;
}

