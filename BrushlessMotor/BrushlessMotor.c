#include "BrushlessMotor.h"
#include "math.h"
#define A GPIO_Pin_8
#define B GPIO_Pin_9
#define C GPIO_Pin_10
#define US 72
#define HalfCYCLE 800//250us为一个pwm周期
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
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1,ENABLE);/*使能定时器1的时钟*/
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM1_UP_IRQn;/*定时器1的中断通道使能*/
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;/*定时器1的中断通道使能*/
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 22;/*抢占优先级*/
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;/*响应优先级*/
	NVIC_Init(&NVIC_InitStructure);/*配置中断分组，并使能中断*/

	TIM_ARRPreloadConfig(TIM1, ENABLE);/*启动TIM1影子寄存器*/
	
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
	TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_Low;
	TIM_OCInitStructure.TIM_Pulse = 0;
	TIM_OC1Init(TIM1, &TIM_OCInitStructure);
	
	TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable);/*启动OC影子寄存器*/
	
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
	TIM_TimeBaseInitStrecture.TIM_Prescaler = US;/*预分配*/
	TIM_TimeBaseInitStrecture.TIM_ClockDivision = TIM_CKD_DIV1;/*时钟分频*/
	TIM_TimeBaseInitStrecture.TIM_CounterMode = TIM_CounterMode_CenterAligned1;/*中央计数*/
	TIM_TimeBaseInitStrecture.TIM_RepetitionCounter = 0;/*重复计数寄存器*/
	TIM_TimeBaseInit(TIM1,&TIM_TimeBaseInitStrecture);/*初始化*/

	TIM_ClearFlag(TIM1,TIM_FLAG_Update);/*清标志位*/
	TIM_ITConfig(TIM1,TIM_IT_Update,ENABLE);/*使能中断*/
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
		//freetime																		 	实际 1
		//freetime+HalfCYCLE														实际 0
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
	
//	if (TIM1->CNT>HalfCYCLE>>1) //在中点
//	{
		TIM_ITConfig(TIM1,TIM_IT_Update,DISABLE);//关闭定时器中断
		BrushlessMotor_SVPWM();
		TIM_ClearFlag(TIM1,TIM_FLAG_Update);/*清中断标志*/
		TIM_ITConfig(TIM1,TIM_IT_Update,ENABLE);/*使能中断*/
//	}
//	else /*在边缘*/TIM_ClearFlag(TIM1,TIM_FLAG_Update);/*清中断标志*/
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
	TIM_Cmd(TIM1,ENABLE);/*使能计数*/
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
