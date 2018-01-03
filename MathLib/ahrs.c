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
#define Kp 							  50.0f
#define Ki 							  0.005f
/*------------------------------------------
								全局变量
------------------------------------------*/
/* 四元数结构体	*/
AHRS_QuaternionTypeDef  QuadNumber = {1, 0, 0, 0};
/* 欧拉角结构体 */
AHRS_EulerAngleTypeDef  EulerAngle = {0};
/*------------------------------------------
 函数功能: 欧拉角转四元数
 函数参数: - Pointer of struct AHRS_QuaternionTypeDef
					 - Pointer of struct AHRS_EulerAngleTypeDef
------------------------------------------*/
void Euler_ToQuaternion(AHRS_QuaternionTypeDef *pQ, AHRS_EulerAngleTypeDef *pE)
{
  float halfP = pE->Pitch / 2.0f;
  float halfR = pE->Roll  / 2.0f;
  float halfY = pE->Yaw   / 2.0f;

  float sinP = sinf(halfP);
  float cosP = cosf(halfP);
  float sinR = sinf(halfR);
  float cosR = cosf(halfR);
  float sinY = sinf(halfY);
  float cosY = cosf(halfY);
  
  pQ->q0 = sinY*cosR*cosP + cosY*sinR*sinP;
  pQ->q1 = cosY*sinR*cosP - sinY*cosR*sinP;  
  pQ->q2 = cosY*cosR*sinP + sinY*sinR*cosP;
  pQ->q3 = cosY*cosR*cosP - sinY*sinR*sinP;
  
}
/*------------------------------------------
 函数功能: 四元数转欧拉角
 函数参数: - Pointer of struct AHRS_QuaternionTypeDef
					 - Pointer of struct AHRS_EulerAngleTypeDef
------------------------------------------*/
void Quaternion_ToEuler(AHRS_QuaternionTypeDef *pQ, AHRS_EulerAngleTypeDef *pE)
{
  float NumQ_YawU = 2.0f * (pQ->q0*pQ->q3 + pQ->q1*pQ->q2);
	float NumQ_YawD = 1.0f - (2.0f * (pQ->q2*pQ->q2 + pQ->q3*pQ->q3));
  float NumQ_pitch = - 2.0f * (pQ->q1*pQ->q3 - pQ->q0*pQ->q2);
  float NumQ_RollU = 2.0f * (pQ->q0*pQ->q3 + pQ->q1*pQ->q2);
	float NumQ_RollD = 1.0f - (2.0f * (pQ->q1*pQ->q1 + pQ->q2*pQ->q2));
	
	pE->Yaw    = atan2f(NumQ_RollU, NumQ_RollD);
  pE->Pitch  = asinf(NumQ_pitch);
  pE->Roll   = atan2f(NumQ_YawU, NumQ_YawD);
}
/*------------------------------------------
 函数功能: 四元数归一化
 函数参数: - Pointer of struct AHRS_QuaternionTypeDef
------------------------------------------*/
void Quaternion_Normalize(AHRS_QuaternionTypeDef *pQ)
{
  float Normalize = 0.0f;

  Normalize = invSqrtf(squa(pQ->q0) + squa(pQ->q1) + squa(pQ->q2) + squa(pQ->q3));

  pQ->q0 = pQ->q0 * Normalize;
  pQ->q1 = pQ->q1 * Normalize;
  pQ->q2 = pQ->q2 * Normalize;
  pQ->q3 = pQ->q3 * Normalize;
}
/*------------------------------------------
 函数功能: 一阶龙格库塔法更新四元数
 函数参数: - Pointer of struct AHRS_QuaternionTypeDef
					 - GyrX (弧度)
					 - GyrY (弧度)
				   - GyrZ (弧度)
					 - helfTimes (采样周期的一半,单位秒)
------------------------------------------*/
void RungeKutta(AHRS_QuaternionTypeDef *pQ, float GyrX, float GyrY, float GyrZ, float helfTimes)
{
	pQ->q0 = pQ->q0 + (-pQ->q1*GyrX - pQ->q2*GyrY - pQ->q3*GyrZ) * helfTimes;
//  pQ->q0 = pQ->q0 + (-pQ->q2*GyrX - pQ->q2*GyrY - pQ->q3*GyrZ) * helfTimes;
	pQ->q1 = pQ->q1 + ( pQ->q0*GyrX - pQ->q3*GyrY + pQ->q2*GyrZ) * helfTimes;
//  pQ->q1 = pQ->q1 + ( pQ->q3*GyrX - pQ->q3*GyrY + pQ->q2*GyrZ) * helfTimes;
	pQ->q2 = pQ->q2 + ( pQ->q3*GyrX + pQ->q0*GyrY - pQ->q1*GyrZ) * helfTimes;
//  pQ->q2 = pQ->q2 + ( pQ->q0*GyrX + pQ->q0*GyrY - pQ->q1*GyrZ) * helfTimes;
	pQ->q3 = pQ->q3 + (-pQ->q2*GyrX + pQ->q1*GyrY + pQ->q0*GyrZ) * helfTimes;
//  pQ->q3 = pQ->q3 + (-pQ->q1*GyrX + pQ->q1*GyrY + pQ->q0*GyrZ) * helfTimes;
}
/*------------------------------------------
 函数功能: 初始化AHRS结构体
 函数参数: - Pointer of struct AHRS_QuaternionTypeDef
					 - Pointer of strfuct AHRS_EulerAngleTypeDef
------------------------------------------*/
void AHRS_Init(AHRS_QuaternionTypeDef *pQ, AHRS_EulerAngleTypeDef *pE)
{
  pQ->q0 		= 1.0f;
  pQ->q1 		= 0.0f;
  pQ->q2 		= 0.0f;
  pQ->q3    = 0.0f;
  pE->Pitch = 0.0f;
  pE->Roll  = 0.0f;
  pE->Yaw   = 0.0f;
}
/*------------------------------------------
 函数功能: 更新AHRS
 函数参数: 无
------------------------------------------*/
AHRS_EulerAngleTypeDef AHRS_Update(float * AGBuff)
{
  float Normalize;
  float gx, gy, gz;

  float ErrX, ErrY, ErrZ;
  float AccX, AccY, AccZ;
  float GyrX, GyrY, GyrZ;

  float Mq13,Mq23,Mq33;

  static float AngZ_Temp = 0.0f;
  static float exInt = 0.0f, eyInt = 0.0f, ezInt = 0.0f;

  
	Mq13 = 2.0f*(QuadNumber.q1 * QuadNumber.q3 - QuadNumber.q0 * QuadNumber.q2);

  Mq23 = 2.0f*(QuadNumber.q0 * QuadNumber.q1 + QuadNumber.q2 * QuadNumber.q3);

	Mq33 = squa(QuadNumber.q3) - 0.5f + squa(QuadNumber.q0);


  Normalize = invSqrtf(squa(AGBuff[0]) + squa(AGBuff[1]) + squa(AGBuff[2])); 
	
  AccX = AGBuff[0] * Normalize;
  AccY = AGBuff[1] * Normalize;
  AccZ = AGBuff[2] * Normalize;


  gx = Mq13;
  gy = Mq23;
  gz = Mq33;


  ErrX = (AccY*gz - AccZ*gy);
  ErrY = (AccZ*gx - AccX*gz);
  ErrZ = (AccX*gy - AccY*gx);

  exInt = exInt + ErrX*Ki;
  eyInt = eyInt + ErrY*Ki;
  ezInt = ezInt + ErrZ*Ki;

  GyrX = toRad(AGBuff[3]);
  GyrY = toRad(AGBuff[4]);
  GyrZ = toRad(AGBuff[5]);

  GyrX = GyrX + Kp*ErrX + exInt;
  GyrY = GyrY + Kp*ErrY + eyInt;
  GyrZ = GyrZ + Kp*ErrZ + ezInt;

  RungeKutta(&QuadNumber, GyrX, GyrY, GyrZ, MPU6050_SAMPLERATE_HALFPRI);
  Quaternion_Normalize(&QuadNumber);
  Quaternion_ToEuler(&QuadNumber, &EulerAngle);

  EulerAngle.Pitch = toDeg(EulerAngle.Pitch);
  EulerAngle.Roll  = toDeg(EulerAngle.Roll);
  EulerAngle.Yaw   = toDeg(EulerAngle.Yaw);

  /* 互补滤波 */
  #define CF_A 0.9f
  #define CF_B 0.1f
  AngZ_Temp = AngZ_Temp + GyrZ * MPU6050_SAMPLERATE_PRI;
  AngZ_Temp = CF_A * AngZ_Temp + CF_B * EulerAngle.Yaw;
	
  if(AngZ_Temp>360.0f)
    EulerAngle.Yaw = AngZ_Temp - 360.0f;
  else if(AngZ_Temp<0.0f)
    EulerAngle.Yaw = AngZ_Temp + 360.0f;
  else
    EulerAngle.Yaw = AngZ_Temp;
	return EulerAngle;
}
AHRS_EulerAngleTypeDef IMUupdate(float * AGBuff)
{
	static float AngZ_Temp = 0.0f;
	
	static float exInt = 0.0f, eyInt = 0.0f, ezInt = 0.0f;
	float norm;
	float vx, vy, vz;
	float ex, ey, ez;  
	// 测量正常化
	norm = sqrt(AGBuff[0]*AGBuff[0] + AGBuff[1]*AGBuff[1] + AGBuff[2]*AGBuff[2]);      
	AGBuff[0] = AGBuff[0] / norm;                   //单位化
	AGBuff[1] = AGBuff[1] / norm;
	AGBuff[2] = AGBuff[2] / norm;      
	// 估计方向的重力
	vx = 2*(QuadNumber.q1*QuadNumber.q3 - QuadNumber.q0*QuadNumber.q2);
	vy = 2*(QuadNumber.q0*QuadNumber.q1 + QuadNumber.q2*QuadNumber.q3);
	vz = QuadNumber.q0*QuadNumber.q0 - QuadNumber.q1*QuadNumber.q1 - QuadNumber.q2*QuadNumber.q2 + QuadNumber.q3*QuadNumber.q3;
	// 错误的领域和方向传感器测量参考方向之间的交叉乘积的总和
	ex = (AGBuff[1]*vz - AGBuff[2]*vy);
	ey = (AGBuff[2]*vx - AGBuff[0]*vz);
	ez = (AGBuff[0]*vy - AGBuff[1]*vx);
	// 积分误差比例积分增益
	exInt = exInt + ex*Ki;
	eyInt = eyInt + ey*Ki;
	ezInt = ezInt + ez*Ki;
	// 调整后的陀螺仪测量
	AGBuff[3] = toRad(AGBuff[3]) + Kp*ex + exInt;
	AGBuff[4] = toRad(AGBuff[4]) + Kp*ey + eyInt;
	AGBuff[5] = toRad(AGBuff[5]) + Kp*ez + ezInt;
	// 整合四元数率和正常化
	QuadNumber.q0 = QuadNumber.q0 + (-QuadNumber.q1*AGBuff[3] - QuadNumber.q2*AGBuff[4] - QuadNumber.q3*AGBuff[5])*MPU6050_SAMPLERATE_HALFPRI;
	QuadNumber.q1 = QuadNumber.q1 + (QuadNumber.q0*AGBuff[3] + QuadNumber.q2*AGBuff[5] - QuadNumber.q3*AGBuff[4])*MPU6050_SAMPLERATE_HALFPRI;
	QuadNumber.q2 = QuadNumber.q2 + (QuadNumber.q0*AGBuff[4] - QuadNumber.q1*AGBuff[5] + QuadNumber.q3*AGBuff[3])*MPU6050_SAMPLERATE_HALFPRI;
	QuadNumber.q3 = QuadNumber.q3 + (QuadNumber.q0*AGBuff[5] + QuadNumber.q1*AGBuff[4] - QuadNumber.q2*AGBuff[3])*MPU6050_SAMPLERATE_HALFPRI;  
	// 正常化四元
	norm = sqrt(squa(QuadNumber.q0) + squa(QuadNumber.q1) + squa(QuadNumber.q2) + squa(QuadNumber.q3));
	QuadNumber.q0 = QuadNumber.q0 / norm;
	QuadNumber.q1 = QuadNumber.q1 / norm;
	QuadNumber.q2 = QuadNumber.q2 / norm;
	QuadNumber.q3 = QuadNumber.q3 / norm;
	EulerAngle.Pitch  = asin(-2 * QuadNumber.q1 * QuadNumber.q3 + 2 * QuadNumber.q0* QuadNumber.q2); // pitch ,转换为度数
	EulerAngle.Roll = atan2(2 * QuadNumber.q2 * QuadNumber.q3 + 2 * QuadNumber.q0 * QuadNumber.q1, -2 * QuadNumber.q1 * QuadNumber.q1 - 2 * QuadNumber.q2* QuadNumber.q2 + 1); // roll
	EulerAngle.Yaw = atan2(2*(QuadNumber.q1*QuadNumber.q2 + QuadNumber.q0*QuadNumber.q3),squa(QuadNumber.q0) + squa(QuadNumber.q1) - squa(QuadNumber.q2) - squa(QuadNumber.q3)); // yaw
	EulerAngle.Pitch=toDeg(EulerAngle.Pitch);
	EulerAngle.Roll=toDeg(EulerAngle.Roll);
	EulerAngle.Yaw=toDeg(EulerAngle.Yaw);
//	/* 互补滤波 */
//  #define CF_A 0.9f
//  #define CF_B 0.1f
//  AngZ_Temp = AngZ_Temp + AGBuff[5] * MPU6050_SAMPLERATE_PRI;
//  AngZ_Temp = CF_A * AngZ_Temp + CF_B * EulerAngle.Yaw;
//	
//  if(AngZ_Temp>360.0f)
//    EulerAngle.Yaw = AngZ_Temp - 360.0f;
//  else if(AngZ_Temp<0.0f)
//    EulerAngle.Yaw = AngZ_Temp + 360.0f;
//  else
//    EulerAngle.Yaw = AngZ_Temp;
	return EulerAngle;
}
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

