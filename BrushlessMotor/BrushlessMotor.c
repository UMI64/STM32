#include "BrushlessMotor.h"
#define Clack 50000 //50khz
#define A GPIO_Pin_0
#define B GPIO_Pin_1
#define C GPIO_Pin_2
#define HalfUS 18
#define CYCLE 2000//500us为一个pwm周期
#define PI_Init 120

static uint8_t flag=1; 
typedef struct Delaydata
{
	volatile uint16_t Min;
	void (*GPIO)();
} Delaydata;
static BrushlessMotorData BrushlessMotordata;
volatile uint16_t pi;
volatile Delaydata Delay[7];
volatile uint8_t states=0,block=0;

void GPIOB_RCC_Init ()
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
}
void GPIOB012_Init()
{
	GPIO_InitTypeDef GPIO_InitData;
	GPIO_InitData.GPIO_Pin = A | B | C;
	GPIO_InitData.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitData.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOB, &GPIO_InitData);
}
void Time1_Init ()
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStrecture;
	NVIC_InitTypeDef NVIC_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1,ENABLE);/*使能定时器1的时钟*/
	
	 NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);

	NVIC_InitStructure.NVIC_IRQChannel = TIM1_UP_IRQn;/*定时器1的中断通道使能*/
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;/*定时器1的中断通道使能*/
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 22;/*抢占优先级*/
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;/*响应优先级*/
	NVIC_Init(&NVIC_InitStructure);/*配置中断分组，并使能中断*/

	TIM_TimeBaseInitStrecture.TIM_Period = CYCLE;/*重装载寄存器*/
	TIM_TimeBaseInitStrecture.TIM_Prescaler = HalfUS;/*预分配*/
	TIM_TimeBaseInitStrecture.TIM_ClockDivision = TIM_CKD_DIV1;/*时钟分频*/
	TIM_TimeBaseInitStrecture.TIM_CounterMode = TIM_CounterMode_Up;/*向上计数*/
	TIM_TimeBaseInitStrecture.TIM_RepetitionCounter = 0;/*重复计数寄存器*/
	TIM_TimeBaseInit(TIM1,&TIM_TimeBaseInitStrecture);/*初始化*/

	TIM_ClearFlag(TIM1,TIM_FLAG_Update);/*清更新标志位*/
	TIM_ITConfig(TIM1,TIM_IT_Update,ENABLE);/*使能中断*/
}
void AU ()
{
	GPIOB->BRR = A;
}
void  AD ()
{
	GPIOB->BSRR = A;
}
void  BU ()
{
	GPIOB->BRR = B;
}
void  BD ()
{
	GPIOB->BSRR = B;
}
void  CU ()
{
	GPIOB->BRR = C;
}
void  CD ()
{
	GPIOB->BSRR = C;
}
void BrushlessMotor_SVPWM()
{
	uint16_t p,HA=0,HB=0,HC=0;
	p=pi;
	block=0;
	while (1)
	{
		if (p<60)
		{
			switch (block)
			{
				case 0 :		HA=0;HB=1.1547*(sinlist[60-p])*CYCLE;HC=CYCLE;
										Delay[0].Min=10;Delay[0].GPIO=CU;
										Delay[1].Min=(CYCLE-HB)/2;Delay[1].GPIO=BU;
										Delay[2].Min=HB/2;Delay[2].GPIO=AU;
										Delay[3].Min=10;Delay[3].GPIO=AD;
										Delay[4].Min=Delay[2].Min;Delay[4].GPIO=BD;
										Delay[5].Min=Delay[1].Min;Delay[5].GPIO=CD;
										Delay[6].Min=10;Delay[6].GPIO=CD;
										break;
				case 1 : 		HA=1.1547*(sinlist[p])*CYCLE;HB=0;HC=CYCLE;//59
										Delay[0].Min=10;Delay[0].GPIO=CU;
										Delay[1].Min=(CYCLE-HA)/2;Delay[1].GPIO=AU;
										Delay[2].Min=HA/2;Delay[2].GPIO=BU;
										Delay[3].Min=10;Delay[3].GPIO=BD;
										Delay[4].Min=Delay[2].Min;Delay[4].GPIO=AD;
										Delay[5].Min=Delay[1].Min;Delay[5].GPIO=CD;
										Delay[6].Min=10;Delay[6].GPIO=CD;
										break;
				case 2 : 		HA=CYCLE;HB=0;HC=1.1547*(sinlist[60-p])*CYCLE;//0
										Delay[0].Min=10;Delay[0].GPIO=AU;
										Delay[1].Min=(CYCLE-HC)/2;Delay[1].GPIO=CU;
										Delay[2].Min=HC/2;Delay[2].GPIO=BU;
										Delay[3].Min=10;Delay[3].GPIO=BD;
										Delay[4].Min=Delay[2].Min;Delay[4].GPIO=CD;
										Delay[5].Min=Delay[1].Min;Delay[5].GPIO=AD;
										Delay[6].Min=10;Delay[6].GPIO=AD;
										break;
				case 3 : 		HA=CYCLE;HB=1.1547*(sinlist[p])*CYCLE;HC=0;
										Delay[0].Min=10;Delay[0].GPIO=AU;
										Delay[1].Min=(CYCLE-HB)/2;Delay[1].GPIO=BU;
										Delay[2].Min=HB/2;Delay[2].GPIO=CU;
										Delay[3].Min=10;Delay[3].GPIO=CD;
										Delay[4].Min=Delay[2].Min;Delay[4].GPIO=BD;
										Delay[5].Min=Delay[1].Min;Delay[5].GPIO=AD;
										Delay[6].Min=10;Delay[6].GPIO=AD;
										break;
				case 4 : 		HA=1.1547*(sinlist[60-p])*CYCLE;HB=CYCLE;HC=0;
										Delay[0].Min=10;Delay[0].GPIO=BU;
										Delay[1].Min=(CYCLE-HA)/2;Delay[1].GPIO=AU;
										Delay[2].Min=HA/2;Delay[2].GPIO=CU;
										Delay[3].Min=10;Delay[3].GPIO=CD;
										Delay[4].Min=Delay[2].Min;Delay[4].GPIO=AD;
										Delay[5].Min=Delay[1].Min;Delay[5].GPIO=BD;
										Delay[6].Min=10;Delay[6].GPIO=BD;
										break;
				case 5 : 		HA=0;HB=CYCLE;HC=1.1547*(sinlist[p])*CYCLE;
										Delay[0].Min=10;Delay[0].GPIO=BU;
										Delay[1].Min=(CYCLE-HC)/2;Delay[1].GPIO=CU;
										Delay[2].Min=HC/2;Delay[2].GPIO=AU;
										Delay[3].Min=10;Delay[3].GPIO=AD;
										Delay[4].Min=Delay[2].Min;Delay[4].GPIO=CD;
										Delay[5].Min=Delay[1].Min;Delay[5].GPIO=BD;
										Delay[6].Min=10;Delay[6].GPIO=BD;
			}
			break;
		}
		else {p-=60;++block;}
	}
}
void TIM1_UP_IRQHandler()
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStrecture;
	TIM_Cmd(TIM1,DISABLE);/*关闭计数*/
	TIM_ITConfig(TIM1,TIM_IT_Update,DISABLE);//关闭定时器中断
	
	if (states==7)	{BrushlessMotor_SVPWM();states=0;}
	else if (states==0){;}
	else Delay[states-1].GPIO ();
	while (Delay[states].Min==0)
	{
		++states;
		Delay[states-1].GPIO ();
	}	
	TIM_TimeBaseInitStrecture.TIM_Period = Delay[states].Min;/*重装载寄存器*/
	TIM_TimeBaseInitStrecture.TIM_Prescaler = HalfUS;/*预分配*/
	TIM_TimeBaseInitStrecture.TIM_ClockDivision = TIM_CKD_DIV1;/*时钟分频*/
	TIM_TimeBaseInitStrecture.TIM_CounterMode = TIM_CounterMode_Up;/*向上计数*/
	TIM_TimeBaseInitStrecture.TIM_RepetitionCounter = 0;/*重复计数寄存器*/
	TIM_TimeBaseInit(TIM1,&TIM_TimeBaseInitStrecture);/*初始化*/
	TIM_ClearFlag(TIM1,TIM_FLAG_Update);/*清中断标志*/
	TIM_ITConfig(TIM1,TIM_IT_Update,ENABLE);/*使能中断*/
  TIM_Cmd(TIM1,ENABLE);/*使能计数*/
	++states;
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
	GPIOB012_Init ();
	GPIOB_RCC_Init ();
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
