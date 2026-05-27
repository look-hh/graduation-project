#ifndef  __KEY_H__
#define  __KEY_H__
#include "stm32f10x.h"
#include "Delay.h"
#include "led.h"
#include "beep.h"

// 按键初始化函数
void key_init(void);

// 获取按键状态函数
int key_get_status(int key_num);

// 按键外部中断初始化函数
void key_exti_init(void);

// 按键编号枚举
enum KEY_NUM{KEY0, KEY1, KEY2, KEY3};

// 按键状态枚举
enum KEY_STATUS{KEY_DOWN, KEY_UP, KEY_UNKNOW};

#endif

