#ifndef __BLUE_H__
#define __BLUE_H__
#include "stm32f10x.h"
#include "uart.h"

// 蓝牙模块初始化函数
void bluetooth_init(void);

// 处理蓝牙命令函数
void handle_bluetooth_cmd(uint8_t *rxbuf);

// 处理蓝牙命令任务函数
void process_bluetooth_cmd(void);

#endif 

