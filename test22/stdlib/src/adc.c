#include "adc.h"
#include "delay.h"

void Adc_Init(void)
{
    ADC_InitTypeDef ADC_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA |
                               RCC_APB2Periph_ADC1,
                           ENABLE);   // 使能 ADC1 通道时钟
    RCC_ADCCLKConfig(RCC_PCLK2_Div6); // 设置 ADC 分频因子 6
    // 72M/6=12,ADC 最大时间不能超过 14M
    // PA1 作为模拟通道输入引脚
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;                       // 模拟输入
    GPIO_Init(GPIOA, &GPIO_InitStructure);                              // 初始化 GPIOA.1
    ADC_DeInit(ADC1);                                                   // 复位 ADC1,将外设 ADC1 的全部寄存器重设为缺省值
    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;                  // ADC 独立模式
    ADC_InitStructure.ADC_ScanConvMode = DISABLE;                       // 单通道模式
    ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;                 // 单次转换模式
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None; // 转换由
    // 软件而不是外部触发启动
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right; // ADC 数据右对齐
    ADC_InitStructure.ADC_NbrOfChannel = 1;                // 顺序进行规则转换的 ADC 通道的数目
    ADC_Init(ADC1, &ADC_InitStructure);                    // 根据指定的参数初始化外设 ADCx
    ADC_Cmd(ADC1, ENABLE);                                 // 使能指定的 ADC1
    ADC_ResetCalibration(ADC1);                            // 开启复位校准
    while (ADC_GetResetCalibrationStatus(ADC1))
        ;                       // 等待复位校准结束
    ADC_StartCalibration(ADC1); // 开启 AD 校准
    while (ADC_GetCalibrationStatus(ADC1))
        ; // 等待校准结束
}
// 获得 ADC 值
// ch:通道值 0~3
uint16_t Get_Adc(uint8_t ch)
{
    // 设置指定 ADC 的规则组通道，设置它们的转化顺序和采样时间
    ADC_RegularChannelConfig(ADC1, ch, 1, ADC_SampleTime_239Cycles5);
    // 通道 1,规则采样顺序值为 1,采样时间为 239.5 周期
    ADC_SoftwareStartConvCmd(ADC1, ENABLE); // 使能软件转换功能
    while (!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC))
        ;                                // 等待转换结束
    return ADC_GetConversionValue(ADC1); // 返回最近一次 ADC1 规则组的转换结果
}
uint16_t Get_Adc_Average(uint8_t ch, uint8_t times)
{
    uint32_t temp_val = 0;
    uint8_t t;
    for (t = 0; t < times; t++)
    {
        temp_val += Get_Adc(ch);
        Delay_ms(5);
    }
    return temp_val / times;
}
