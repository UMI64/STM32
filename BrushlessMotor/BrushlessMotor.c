#include "BrushlessMotor.h"
#define US 72
static BrushlessMotorData BrushlessMotordata;
volatile uint32_t pi=0;

typedef struct Delay
{
	uint32_t Delayhi;
	uint32_t Delay;
	uint8_t fanzhuan;
}Delay;
volatile Delay DelayA,DelayB,DelayC;
volatile uint32_t Min;;

void GPIOB_RCC_Init ()
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
}
void GPIOB012_Init()
{
	GPIO_InitTypeDef GPIO_InitData;
	GPIO_InitData.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2;
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

	TIM_TimeBaseInitStrecture.TIM_Period = Cycle;/*重装载寄存器*/
	TIM_TimeBaseInitStrecture.TIM_Prescaler = US;/*预分配*/
	TIM_TimeBaseInitStrecture.TIM_ClockDivision = TIM_CKD_DIV1;/*时钟分频*/
	TIM_TimeBaseInitStrecture.TIM_CounterMode = TIM_CounterMode_Up;/*向上计数*/
	TIM_TimeBaseInitStrecture.TIM_RepetitionCounter = 0;/*重复计数寄存器*/
	TIM_TimeBaseInit(TIM1,&TIM_TimeBaseInitStrecture);/*初始化*/

	TIM_ClearFlag(TIM1,TIM_FLAG_Update);/*清更新标志位*/
	TIM_ITConfig(TIM1,TIM_IT_Update,ENABLE);/*使能中断*/
}
void TIM1_UP_IRQHandler()
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStrecture;
	TIM_Cmd(TIM1,DISABLE);/*关闭计数*/
	TIM_ITConfig(TIM1,TIM_IT_Update,DISABLE);//关闭定时器中断
	/*减去前一次的延时量*/
	DelayA.Delayhi-=Min;
	DelayB.Delayhi-=Min;
	DelayC.Delayhi-=Min; //减去前一次的延时量
	/*找出归零并翻转并重装载延时*/
	if (DelayA.Delayhi==0) {if (DelayA.fanzhuan) {GPIO_SetBits(GPIOB,GPIO_Pin_0);DelayA.fanzhuan=0;DelayA.Delayhi=DelayA.Delay;if (DelayA.Delayhi==0) {GPIO_ResetBits(GPIOB,GPIO_Pin_0);DelayA.Delayhi=Cycle,DelayA.fanzhuan=1;}} else{GPIO_ResetBits(GPIOB,GPIO_Pin_0);DelayA.fanzhuan=1;DelayA.Delayhi=Cycle-DelayA.Delay;if (DelayA.Delayhi==0){GPIO_SetBits(GPIOB,GPIO_Pin_0);DelayA.fanzhuan=0;DelayA.Delayhi=Cycle;}}}
	if (DelayB.Delayhi==0) {if (DelayB.fanzhuan) {GPIO_SetBits(GPIOB,GPIO_Pin_1);DelayB.fanzhuan=0;DelayB.Delayhi=DelayB.Delay;if (DelayB.Delayhi==0) {GPIO_ResetBits(GPIOB,GPIO_Pin_1);DelayB.Delayhi=Cycle,DelayB.fanzhuan=1;}} else{GPIO_ResetBits(GPIOB,GPIO_Pin_1);DelayB.fanzhuan=1;DelayB.Delayhi=Cycle-DelayB.Delay;if (DelayB.Delayhi==0){GPIO_SetBits(GPIOB,GPIO_Pin_0);DelayB.fanzhuan=0;DelayB.Delayhi=Cycle;}}}
	if (DelayC.Delayhi==0) {if (DelayC.fanzhuan) {GPIO_SetBits(GPIOB,GPIO_Pin_2);DelayC.fanzhuan=0;DelayC.Delayhi=DelayC.Delay;if (DelayC.Delayhi==0) {GPIO_ResetBits(GPIOB,GPIO_Pin_2);DelayC.Delayhi=Cycle,DelayC.fanzhuan=1;}} else{GPIO_ResetBits(GPIOB,GPIO_Pin_2);DelayC.fanzhuan=1;DelayC.Delayhi=Cycle-DelayC.Delay;if (DelayC.Delayhi==0){GPIO_SetBits(GPIOB,GPIO_Pin_0);DelayC.fanzhuan=0;DelayC.Delayhi=Cycle;}}}
	/*找出最少延时*/
	if(DelayA.Delayhi>DelayB.Delayhi) { Min=DelayB.Delayhi;}
	else { Min=DelayA.Delayhi;}
	if (Min>DelayC.Delayhi) { Min=DelayC.Delayhi;}               
	/*延时*/
	if (Min==0) while (1);
	TIM_TimeBaseInitStrecture.TIM_Period = Min;/*重装载寄存器*/
	TIM_TimeBaseInitStrecture.TIM_Prescaler = US;/*预分配*/
	TIM_TimeBaseInitStrecture.TIM_ClockDivision = TIM_CKD_DIV1;/*时钟分频*/
	TIM_TimeBaseInitStrecture.TIM_CounterMode = TIM_CounterMode_Up;/*向上计数*/
	TIM_TimeBaseInitStrecture.TIM_RepetitionCounter = 0;/*重复计数寄存器*/
	TIM_TimeBaseInit(TIM1,&TIM_TimeBaseInitStrecture);/*初始化*/
	TIM_ClearFlag(TIM1,TIM_FLAG_Update);/*清中断标志*/
	TIM_ITConfig(TIM1,TIM_IT_Update,ENABLE);/*使能中断*/
  TIM_Cmd(TIM1,ENABLE);/*使能计数*/
}
void BrushlessMotor_SPWM()
{
	static uint16_t p;
	while (1)
	{
		p=pi;
		while (p>360) p-=360;
		DelayA.Delay=Cycle*((sinlist[p]+1)/2);
		p=pi+60;
		while (p>360) p-=360;
		DelayB.Delay=Cycle*((sinlist[p]+1)/2);
		p=pi+120;
		while (p>360) p-=360;
		DelayC.Delay=Cycle*((sinlist[p]+1)/2);
		vTaskDelay(20);
	}
}
void BrushlessMotor_SPWM_Init()
{
	uint32_t p=0;
	p=pi;
	while (p>360) p-=360;
	DelayA.Delay=Cycle*((sinlist[p]+1)/2);
	DelayA.Delayhi=DelayA.Delay;
	DelayA.fanzhuan=0;
	p=pi+60;
	while (p>360) p-=360;
	DelayB.Delay=Cycle*((sinlist[p]+1)/2);
	DelayB.Delayhi=DelayB.Delay;
	DelayB.fanzhuan=0;
	p=pi+120;
	while (p>360) p-=360;
	DelayC.Delay=Cycle*((sinlist[p]+1)/2);
	DelayC.Delayhi=DelayC.Delay;
	DelayC.fanzhuan=0;
}
void BrushlessMotor_Turn()
{
	++pi;
	if(pi==360) pi=0;
}
void BrushlessMotor_Init ()
{
	GPIOB012_Init ();
	GPIOB_RCC_Init ();
	Time1_Init ();
	BrushlessMotor_SPWM_Init();
	xTaskCreate(BrushlessMotor_SPWM,"test",50,NULL,1,NULL);
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
