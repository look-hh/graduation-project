#ifndef __PWM_H__
#define __PWM_H__

#include "stm32f10x.h"

// 定时器初始化函数
void TIM_Init(void);

// 定时器功能函数
void TIM_Function(void);

// PWM初始化函数
void PWM_Init(void);

// 设置比较值3函数（舵机）
void PWM_SetCompare3(uint16_t Compare);

// 风扇PWM初始化函数
void PWM_Fan_Init(void);

// 风扇PWM设置占空比函数
void PWM_Fan_SetCompare(uint16_t Compare);

#endif

