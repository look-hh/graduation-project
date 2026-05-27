#include "adc.h"

//PB0 ==> ADC1_IN8
void Adc1_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStruct;
	
	ADC_InitTypeDef  ADC_InitStruct;	

	//使能时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);

	//配置GPIO口为模拟模式
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_InitStruct.GPIO_Pin =GPIO_Pin_0;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStruct);

	//配置adc1参数
	ADC_InitStruct.ADC_Mode = ADC_Mode_Independent;//独立模式 只使用adc1
	ADC_InitStruct.ADC_ScanConvMode = DISABLE;//关闭扫描模式
	ADC_InitStruct.ADC_ContinuousConvMode = DISABLE;//关闭连续转换
	ADC_InitStruct.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;//软件触发
	ADC_InitStruct.ADC_DataAlign = ADC_DataAlign_Right;//右对齐
	ADC_InitStruct.ADC_NbrOfChannel = 1;//通道数目为1
	ADC_Init(ADC1, &ADC_InitStruct);

	//配置通道8转换顺序和采样时间
	ADC_RegularChannelConfig(ADC1,ADC_Channel_8, 1, ADC_SampleTime_71Cycles5);


	//使能ADC1
	ADC_Cmd(ADC1, ENABLE);

	//ADC校准
    ADC_ResetCalibration(ADC1);  // 复位校准寄存器
    while(ADC_GetResetCalibrationStatus(ADC1));  // 等待复位完成
    
    ADC_StartCalibration(ADC1);  // 开始校准
    while(ADC_GetCalibrationStatus(ADC1));  // 等待校准完成
	
	
	
}

//读取ADC1通道8转换值
uint16_t ADC1_IN8_Read(void)
{
	ADC_SoftwareStartConvCmd(ADC1,ENABLE);
	while(!ADC_GetFlagStatus(ADC1,ADC_FLAG_EOC));//等待转换完成
	ADC_ClearFlag(ADC1, ADC_FLAG_EOC); 
	return ADC_GetConversionValue(ADC1);//返回转换数据寄存器
}

//土壤湿度模块初始化
void Soil_moisture_Init(void)
{
	Adc1_Init();	
}

uint16_t dry_value = 1250;   // 干燥阈值（空气中约1214）
uint16_t wet_value = 850;    // 湿润阈值（湿润土壤约900）

uint16_t Get_soil_moisture_value(void)
{
    uint16_t adc_val = ADC1_IN8_Read();
    // 调试输出原始ADC值
    //printf("[SOIL] Raw ADC: %d\r\n", adc_val);
    // 简单限幅，防止异常值
    if (adc_val > 4095) adc_val = 4095;
    return adc_val;
}

// 将ADC值转换为湿度百分比（0-100%）
// 特性：干土=高ADC值(高电阻)，湿土=低ADC值(低电阻)
uint8_t Get_soil_moisture_percent(void)
{
    uint16_t adc_val = Get_soil_moisture_value();
    //printf("[SOIL] Calc: dry=%d, wet=%d, range=%d\r\n", dry_value, wet_value, dry_value - wet_value);

    // 超出范围时做边界处理
    if (adc_val >= dry_value) return 0;    // 高ADC(干)返回0%
    if (adc_val <= wet_value) return 100;  // 低ADC(湿)返回100%

    // 公式：低ADC(湿) -> 高百分比，高ADC(干) -> 低百分比
    // 使用 (dry-adc)/(dry-wet) 而不是 (adc-wet)/(dry-wet)
    uint8_t percent = (dry_value - adc_val) * 100 / (dry_value - wet_value);
    //printf("[SOIL] Percent: %d%%\r\n", percent);
    return percent;
}
