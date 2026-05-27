#ifndef __ADC_H__
#define __ADC_H__
#include "stm32f10x.h"

// ADC1初始化函数
void Adc1_Init(void);

// 读取ADC1通道8转换值函数
uint16_t ADC1_IN8_Read(void);

// 土壤湿度模块初始化函数
void Soil_moisture_Init(void);

// 获取土壤湿度值函数
uint16_t Get_soil_moisture_value(void);

// 获取土壤湿度百分比函数
uint8_t Get_soil_moisture_percent(void);

#endif




