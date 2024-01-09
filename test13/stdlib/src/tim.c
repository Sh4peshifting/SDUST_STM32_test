#include "tim.h"
#include "led.h"

void TIM3_Int_Init(uint16_t arr, uint16_t psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

	TIM_TimeBaseStructure.TIM_Period = arr;
	TIM_TimeBaseStructure.TIM_Prescaler = psc;
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);

	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	TIM_Cmd(TIM3, ENABLE);
}

void TIM3_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)
	{
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
		if (LED1 == 0)
		{
			GPIO_SetBits(GPIOE, GPIO_Pin_5);
		}
		else
		{
			GPIO_ResetBits(GPIOE, GPIO_Pin_5);
		}
	}
}

// TIM3 PWM 部分初始化
// PWM 输出初始化
// arr：自动重装值
// psc：时钟预分频数
void TIM3_PWM_Init(u16 arr, u16 psc)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_OCInitTypeDef TIM_OCInitStructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); // ①使能定时器 3 时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB |
							   RCC_APB2Periph_AFIO,
						   ENABLE);						 // ①使能 GPIO 和 AFIO 复用功能时钟
	GPIO_PinRemapConfig(GPIO_PartialRemap_TIM3, ENABLE); // ②重映射 TIM3_CH2->PB5
	// 设置该引脚为复用输出功能,输出 TIM3 CH2 的 PWM 脉冲波形 GPIOB.5
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;		// TIM_CH2
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; // 复用推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure); // ①初始化 GPIO
	// 初始化 TIM3
	TIM_TimeBaseStructure.TIM_Period = arr;						// 设置在自动重装载周期值
	TIM_TimeBaseStructure.TIM_Prescaler = psc;					// 设置预分频值
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;				// 设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; // TIM 向上计数模式
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);				// ③初始化 TIMx
	// 初始化 TIM3 Channel2 PWM 模式
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;			  // 选择 PWM 模式 2
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; // 比较输出使能
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;	  // 输出极性高
	TIM_OC2Init(TIM3, &TIM_OCInitStructure);					  // ④初始化外设 TIM3 OC2
	TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable);			  // 使能预装载寄存器
	TIM_Cmd(TIM3, ENABLE);										  // ⑤使能 TIM3
}