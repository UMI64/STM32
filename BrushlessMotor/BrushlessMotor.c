#include "BrushlessMotor.h"
#define A GPIO_Pin_8
#define B GPIO_Pin_9
#define C GPIO_Pin_10
#define US 72
#define HalfCYCLE 500//500us为一个pwm周期
#define PI_Init 120

static uint8_t flag=1;//测试变量

static BrushlessMotorData BrushlessMotordata;
volatile uint16_t pi,freetime=20;

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
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High;
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
	
	TIM_TimeBaseInitStrecture.TIM_Period =(1.5f*freetime)+HalfCYCLE;/*重装载寄存器*/
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
	uint16_t block=0;
	block=pi/60;
	switch (block)
	{
		//HalfCYCLE+freetime 0 																实际 1
		//freetime+(1.1547*(sinlist[p])*HalfCYCLE) 减小       实际 增大
		//freetime+(1.1547*(sinlist[60-p])*HalfCYCLE) 增大    实际 减小 
		//freetime 1 																					实际 0
		case 0 :		TIM1->CCR1 = HalfCYCLE+freetime;TIM1->CCR2=freetime+(1.1547f*(sinlist[60-pi])*HalfCYCLE);TIM1->CCR3=freetime;	//A:0 B:1-0 C:1
								break;
		case 1 :/*59*/TIM1->CCR1 = freetime+(1.1547f*(sinlist[pi])*HalfCYCLE);TIM1->CCR2=freetime;TIM1->CCR3 =HalfCYCLE+freetime;	//A:0-1 B:0 C:1
								break;
		case 2 :/*0*/TIM1->CCR1=freetime;TIM1->CCR2=HalfCYCLE+freetime;TIM1->CCR3=freetime+(1.1547f*(sinlist[180-pi])*HalfCYCLE);	//A:1 B:0 C:1-0
								break;
		case 3 : 		TIM1->CCR1 =freetime;TIM1->CCR2 = freetime+(1.1547f*(sinlist[pi])*HalfCYCLE);TIM1->CCR3 = HalfCYCLE+freetime; 	//A:1 B:0-1 C:0
								break;
		case 4 : 		TIM1->CCR1 =freetime+(1.1547f*(sinlist[300-pi])*HalfCYCLE);TIM1->CCR2=freetime;TIM1->CCR3=HalfCYCLE+freetime; 	//A:1-0 B:1 C:0
								break;
		case 5 : 		TIM1->CCR1 = HalfCYCLE+freetime;TIM1->CCR2 =freetime;TIM1->CCR3 = freetime+(1.1547f*(sinlist[pi])*HalfCYCLE); 	//A:0 B:1 C:0-1
								break;
		case 6 :		TIM1->CCR1 = HalfCYCLE+freetime;TIM1->CCR2=freetime+(1.1547f*(sinlist[60-pi])*HalfCYCLE);TIM1->CCR3=freetime;	//A:0 B:1-0 C:1
	}
}
void TIM1_UP_IRQHandler()
{
	
	if (TIM1->CNT>HalfCYCLE>>1) //在中点
	{
		TIM_ITConfig(TIM1,TIM_IT_Update,DISABLE);//关闭定时器中断
		BrushlessMotor_SVPWM();
		TIM_ClearFlag(TIM1,TIM_FLAG_Update);/*清中断标志*/
		TIM_ITConfig(TIM1,TIM_IT_Update,ENABLE);/*使能中断*/
	}
	else /*在边缘*/TIM_ClearFlag(TIM1,TIM_FLAG_Update);/*清中断标志*/
}
void BrushlessMotor_SVPWM_Init()
{
	GPIO_SetBits(GPIOB, A);
	GPIO_SetBits(GPIOB, B);
	GPIO_SetBits(GPIOB, C);
	pi=PI_Init;
	BrushlessMotor_SVPWM();
}
void BrushlessMotor_Turn()
{
	if (flag)++pi;
	else --pi;
	if(pi>=121) flag=0;
	else if (pi<=119) flag=1;
}
void BrushlessMotor_Init ()
{
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
