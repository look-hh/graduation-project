#ifndef __SERVO_H__
#define __SERVO_H__

#include "stm32f10x.h"

// 风扇状态枚举
enum FAN_STATE{FAN_ON, FAN_OFF,FAN_TOGGLE};

// 水泵状态枚举
enum WP_STATE{WP_ON, WP_OFF,WP_TOGGLE};

// 舵机初始化函数
void Servo_init(void);

// 设置舵机角度函数
void Servo_SetAngle(float Angle);

// 设置风扇速度函数
void Fan_SetSpeed(uint16_t Compare);

// 风扇停止函数
void Fan_SetStop(void);

// 设置风扇状态函数
void Fan_SetState(int Fan_state);

#endif



