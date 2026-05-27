#ifndef __LED_H__
#define __LED_H__

#include "stm32f10x.h"     

// LED编号枚举
enum LED_NUM{LED0, LED1, LED2, LED3};

// LED状态枚举
enum LED_STATE{LED_ON, LED_OFF,LED_TOGGLE};

// LED初始化函数
void led_init(void);

// LED控制函数
void led_ctrl(int led_num, int led_state);

#endif








