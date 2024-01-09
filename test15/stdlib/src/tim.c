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
void TIM3_PWM_Init(uint16_t arr, uint16_t psc)
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

// 定时器 5 通道 1 输入捕获配置
TIM_ICInitTypeDef TIM5_ICInitStructure;
void TIM5_Cap_Init(uint16_t arr, uint16_t psc)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);  // ①使能 TIM5 时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); // ①使能 GPIOA 时钟
	// 初始化 GPIOA.0 ①
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;	  // PA0 设置
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; // PA0 输入
	GPIO_Init(GPIOA, &GPIO_InitStructure);		  // 初始化 GPIOA.0
	GPIO_ResetBits(GPIOA, GPIO_Pin_0);			  // PA0 下拉
	// ②初始化 TIM5 参数
	TIM_TimeBaseStructure.TIM_Period = arr;						// 设定计数器自动重装值
	TIM_TimeBaseStructure.TIM_Prescaler = psc;					// 预分频器
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;		// TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; // TIM 向上计数模式
	TIM_TimeBaseInit(TIM5, &TIM_TimeBaseStructure);				// 初始化 TIMx
	// ③初始化 TIM5 输入捕获通道 1
	TIM5_ICInitStructure.TIM_Channel = TIM_Channel_1;				 // 选择输入端 IC1 映射到 TI1 上
	TIM5_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;	 // 上升沿捕获
	TIM5_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI; // 映射到 TI1 上
	TIM5_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;			 // 配置输入分频,不分频
	TIM5_ICInitStructure.TIM_ICFilter = 0x00;						 // IC1F=0000 配置输入滤波器 不滤波
	TIM_ICInit(TIM5, &TIM5_ICInitStructure);						 // 初始化 TIM5 输入捕获通道 1
	// ⑤初始化 NVIC 中断优先级分组
	NVIC_InitStructure.NVIC_IRQChannel = TIM5_IRQn;			  // TIM3 中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2; // 先占优先级 2 级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		  // 从优先级 0 级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			  // IRQ 通道被使能
	NVIC_Init(&NVIC_InitStructure);							  // 初始化 NVIC
	TIM_ITConfig(TIM5, TIM_IT_Update | TIM_IT_CC1, ENABLE);	  // ④允许更新中断捕获中断
	TIM_Cmd(TIM5, ENABLE);									  // ⑥使能定时器 5
}
uint8_t TIM5CH1_CAPTURE_STA = 0; // 输入捕获状态
uint16_t TIM5CH1_CAPTURE_VAL;	// 输入捕获值
// ⑤定时器 5 中断服务程序
void TIM5_IRQHandler(void)
{
	if ((TIM5CH1_CAPTURE_STA & 0X80) == 0) // 还未成功捕获
	{
		if (TIM_GetITStatus(TIM5, TIM_IT_Update) != RESET)
		{
			if (TIM5CH1_CAPTURE_STA & 0X40) // 已经捕获到高电平了
			{
				if ((TIM5CH1_CAPTURE_STA & 0X3F) == 0X3F) // 高电平太长了
				{
					TIM5CH1_CAPTURE_STA |= 0X80; // 标记成功捕获了一次
					TIM5CH1_CAPTURE_VAL = 0XFFFF;
				}
				else
					TIM5CH1_CAPTURE_STA++;
			}
		}
		if (TIM_GetITStatus(TIM5, TIM_IT_CC1) != RESET) // 捕获 1 发生捕获事件
		{
			if (TIM5CH1_CAPTURE_STA & 0X40) // 捕获到一个下降沿
			{
				TIM5CH1_CAPTURE_STA |= 0X80; // 标记成功捕获到一次上升沿
				TIM5CH1_CAPTURE_VAL = TIM_GetCapture1(TIM5);
				TIM_OC1PolarityConfig(TIM5, TIM_ICPolarity_Rising); // 设置为上升沿捕获
			}
			else // 还未开始,第一次捕获上升沿
			{
				TIM5CH1_CAPTURE_STA = 0; // 清空
				TIM5CH1_CAPTURE_VAL = 0;
				TIM_SetCounter(TIM5, 0);
				TIM5CH1_CAPTURE_STA |= 0X40;						 // 标记捕获到了上升沿
				TIM_OC1PolarityConfig(TIM5, TIM_ICPolarity_Falling); // 设置为下降沿捕获
			}
		}
	}
	TIM_ClearITPendingBit(TIM5, TIM_IT_CC1 | TIM_IT_Update); // 清除中断标志位
}