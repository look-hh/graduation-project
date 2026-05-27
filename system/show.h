#ifndef __SHOW_H__
#define __SHOW_H__

#include "stm32f10x.h"

// 显示GY39数据函数
void show_gy39_data(void);

// 显示舵机和风扇数据函数
void show_Servo_Fan_data(void);

// 显示土壤湿度函数
void show_soil_moisture(void);

// 风扇PID控制函数
void Fan_PID_ctrl(uint32_t set_wd);

// 水泵自动控制函数
void WP_auto_ctrl(uint32_t set_sd);

// LED自动控制函数
void LED_auto_ctrl(uint32_t set_lux);

// 报警自动控制函数
void Alarm_auto_ctrl(void);

// 自动控制状态枚举
enum AUTO_CTRL{AUTO_CTRL_OFF,AUTO_CTRL_ON};

// 手动模式控制函数（在blue.c和main.c中调用）
void Fan_SetManualMode(void);   // 风扇进入手动模式
void Fan_ClearManualMode(void); // 风扇退出手动模式
void Light_SetManualMode(void); // 补光进入手动模式
void Light_ClearManualMode(void);// 补光退出手动模式

// 蓝牙数据发送函数（每15秒调用一次）
void send_bluetooth_data(void);
void update_gy39_cache(void);

#endif



