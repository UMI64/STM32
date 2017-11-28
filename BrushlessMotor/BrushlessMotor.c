#include "BrushlessMotor.h"
#include "math.h"
#define A GPIO_Pin_8
#define B GPIO_Pin_9
#define C GPIO_Pin_10
#define US 72
#define HalfCYCLE 800//250usΪһ��pwm����
#define PI_Init 260
#define SINRANG 1024                                                                                                                                                                                                                                                                                                                                                                                                                                                                           
#define DA 360
#define xiao 0

static BrushlessMotorData BrushlessMotordata;
static uint16_t freetime=10,change_time=65535;
static float sinlist[SINRANG],pi;
static uint8_t fb;
float jiao (uint16_t x)
{
	if (x<256) return (float)x/256;
	else if (x<768) return -(float)x/256+2;
	else return (float)x/256-4;
}
void sin_Init ()
{
	uint16_t i=0;
	for (i=0;i<SINRANG;++i)
	{
		float x=(float)i/(float)SINRANG*6.2831852f;
		sinlist[i]=sinf(x);
//		sinlist[i]=jiao(i);
	}
}
void GPIOA_RCC_Init ()
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
}
void GPIOA_8_9_10_Init()
{
	GPIO_InitTypeDef GPIO_InitData;
	GPIO_InitData.GPIO_Pin = A | B | C;
	GPIO_InitData.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitData.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &GPIO_InitData);
}
void Time1_Init ()
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStrecture;
	TIM_OCInitTypeDef TIM_OCInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1,ENABLE);/*ʹ�ܶ�ʱ��1��ʱ��*/
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM1_UP_IRQn;/*��ʱ��1���ж�ͨ��ʹ��*/
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;/*��ʱ��1���ж�ͨ��ʹ��*/
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 22;/*��ռ���ȼ�*/
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;/*��Ӧ���ȼ�*/
	NVIC_Init(&NVIC_InitStructure);/*�����жϷ��飬��ʹ���ж�*/

	TIM_ARRPreloadConfig(TIM1, ENABLE);/*����TIM1Ӱ�ӼĴ���*/
	
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
	TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_Low;
	TIM_OCInitStructure.TIM_Pulse = 0;
	TIM_OC1Init(TIM1, &TIM_OCInitStructure);
	
	TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable);/*����OCӰ�ӼĴ���*/
	
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable;
	TIM_OCInitStructure.TIM_Pulse = 0;
	TIM_OC2Init(TIM1, &TIM_OCInitStructure);
	
	TIM_OC2PreloadConfig(TIM1, TIM_OCPreload_Enable);
	
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable;
	TIM_OCInitStructure.TIM_Pulse = 0;
	TIM_OC3Init(TIM1, &TIM_OCInitStructure);
	
	TIM_OC3PreloadConfig(TIM1, TIM_OCPreload_Enable);
	
	TIM_CtrlPWMOutputs(TIM1,ENABLE);
	
	TIM_TimeBaseInitStrecture.TIM_Period =1.5f*freetime+HalfCYCLE;
	TIM_TimeBaseInitStrecture.TIM_Prescaler = US;/*Ԥ����*/
	TIM_TimeBaseInitStrecture.TIM_ClockDivision = TIM_CKD_DIV1;/*ʱ�ӷ�Ƶ*/
	TIM_TimeBaseInitStrecture.TIM_CounterMode = TIM_CounterMode_CenterAligned1;/*�������*/
	TIM_TimeBaseInitStrecture.TIM_RepetitionCounter = 0;/*�ظ������Ĵ���*/
	TIM_TimeBaseInit(TIM1,&TIM_TimeBaseInitStrecture);/*��ʼ��*/

	TIM_ClearFlag(TIM1,TIM_FLAG_Update);/*���־λ*/
	TIM_ITConfig(TIM1,TIM_IT_Update,ENABLE);/*ʹ���ж�*/
}
void BrushlessMotor_SVPWM()
{
	static uint16_t pwm_time=0;
	uint16_t block=0;
  	if (pwm_time>=change_time)
	{
		if (fb) 
		{
			if (pi>=DA) pi=xiao;
			pi+=1;
		}
		else 
		{
			if (pi<=xiao) pi=DA;
			pi-=1;
		}
		pwm_time=0;
	}
	++pwm_time;
	block=pi/60;
	TIM1->ARR=(1.5f*freetime)+HalfCYCLE;
	switch (block)
	{
		//freetime																		 	ʵ�� 1
		//freetime+HalfCYCLE														ʵ�� 0
		case 0 :/*0-59*/	TIM1->CCR1 = freetime+((sinlist[(uint16_t)(((float)pi/360)*SINRANG)-1]+sinlist[(uint16_t)(((60-(float)pi)/360)*SINRANG)-1])*HalfCYCLE);TIM1->CCR2=freetime+(sinlist[(uint16_t)(((float)pi/360)*SINRANG)-1]*HalfCYCLE);TIM1->CCR3=freetime;	//A:0 B:1-0 C:1
								break;
		case 1 :/*60-119*/TIM1->CCR1 = freetime+(sinlist[(uint16_t)(((120-(float)pi)/360)*SINRANG)-1]*HalfCYCLE);TIM1->CCR2=freetime+((sinlist[(uint16_t)((((float)pi-60)/360)*SINRANG)-1]+sinlist[(uint16_t)(((120-(float)pi)/360)*SINRANG)-1])*HalfCYCLE);TIM1->CCR3 =freetime;	//A:0-1 B:0 C:1
								break;
		case 2 :/*120-179*/TIM1->CCR1=freetime;TIM1->CCR2=freetime+((sinlist[(uint16_t)((((float)pi-120)/360)*SINRANG)-1]+sinlist[(uint16_t)(((180-(float)pi)/360)*SINRANG)-1])*HalfCYCLE);TIM1->CCR3=freetime+(sinlist[(uint16_t)((((float)pi-120)/360)*SINRANG)-1]*HalfCYCLE);	//A:1 B:0 C:1-0
								break;
		case 3 :/*180-239*/TIM1->CCR1 =freetime;TIM1->CCR2 = freetime+(sinlist[(uint16_t)(((240-(float)pi)/360)*SINRANG)-1]*HalfCYCLE);TIM1->CCR3 = freetime+((sinlist[(uint16_t)((((float)pi-180)/360)*SINRANG)-1]+sinlist[(uint16_t)(((240-(float)pi)/360)*SINRANG)-1])*HalfCYCLE); 	//A:1 B:0-1 C:0
								break;
		case 4 :/*240-299*/TIM1->CCR1 =freetime+(sinlist[(uint16_t)((((float)pi-240)/360)*SINRANG)-1]*HalfCYCLE);TIM1->CCR2=freetime;TIM1->CCR3=freetime+((sinlist[(uint16_t)((((float)pi-240)/360)*SINRANG)-1]+sinlist[(uint16_t)(((300-(float)pi)/360)*SINRANG)-1])*HalfCYCLE); 	//A:1-0 B:1 C:0
								break;
		case 5 :/*300-359*/TIM1->CCR1 = freetime+((sinlist[(uint16_t)((((float)pi-300)/360)*SINRANG)-1]+sinlist[(uint16_t)(((360-(float)pi)/360)*SINRANG)-1])*HalfCYCLE);TIM1->CCR2 =freetime;TIM1->CCR3 = freetime+(sinlist[(uint16_t)(((360-(float)pi)/360)*SINRANG)-1]*HalfCYCLE); 	//A:0 B:1 C:0-1
								break;
		case 6 :/*360*/		TIM1->CCR1 = freetime+((sinlist[(uint16_t)((((float)pi-360)/360)*SINRANG)-1]+sinlist[(uint16_t)(((420-(float)pi)/360)*SINRANG)-1])*HalfCYCLE);TIM1->CCR2=freetime+(sinlist[(uint16_t)((((float)pi-360)/360)*SINRANG)-1]*HalfCYCLE);TIM1->CCR3=freetime;	//A:0 B:1-0 C:1
	}
//	TIM1->CCR1=HalfCYCLE-((1+sinlist[(uint16_t)(((float)pi/360)*SINRANG)-1])/2*HalfCYCLE);
//	if (pi>240) flag=1;else flag=0;
//	TIM1->CCR2=HalfCYCLE-((1+sinlist[(uint16_t)(((float)((pi+120)-(360*flag))/360)*SINRANG)-1])/2*HalfCYCLE);
//	if (pi>120) flag=1;else flag=0;
//	TIM1->CCR3=HalfCYCLE-((1+sinlist[(uint16_t)(((float)((pi+240)-(360*flag))/360)*SINRANG)]-1)/2*HalfCYCLE);
}
void TIM1_UP_IRQHandler()
{
	
//	if (TIM1->CNT>HalfCYCLE>>1) //���е�
//	{
		TIM_ITConfig(TIM1,TIM_IT_Update,DISABLE);//�رն�ʱ���ж�
		BrushlessMotor_SVPWM();
		TIM_ClearFlag(TIM1,TIM_FLAG_Update);/*���жϱ�־*/
		TIM_ITConfig(TIM1,TIM_IT_Update,ENABLE);/*ʹ���ж�*/
//	}
//	else /*�ڱ�Ե*/TIM_ClearFlag(TIM1,TIM_FLAG_Update);/*���жϱ�־*/
}
void BrushlessMotor_SVPWM_Init()
{
	GPIO_SetBits(GPIOB, A);
	GPIO_SetBits(GPIOB, B);
	GPIO_SetBits(GPIOB, C);
	pi=PI_Init;
	BrushlessMotor_SVPWM();
}
void BrushlessMotor_Turn(float power,float speed,u8 FB)
{
	fb=FB;
	if (power>100) power=100;
	if (speed>100) speed=100;
	freetime=1000-(power*9.95f);//20 - 1000 us
	change_time=600-(speed*6);//0-600
}
void BrushlessMotor_Init ()
{
	sin_Init ();
	GPIOA_8_9_10_Init ();
	GPIOA_RCC_Init ();
	Time1_Init ();
	BrushlessMotor_SVPWM_Init();
	TIM_Cmd(TIM1,ENABLE);/*ʹ�ܼ���*/
}
TESTPASS BrushlessMotor_TEST ()
{
	return PASS;
}
void BrushlessMotor_LoadInfo (DriverInfoData* PDriverInfo)
{
	BrushlessMotordata.TurnPower=BrushlessMotor_Turn;
	PDriverInfo->Init=BrushlessMotor_Init;
	PDriverInfo->Test=BrushlessMotor_TEST;
	PDriverInfo->PInfo=&BrushlessMotordata;
	PDriverInfo->DriverID=51;
}
