#ifndef __GY39_H__
#define __GY39_H__
#include "stm32f10x.h"

// GY39模块工作模式
enum GY39MODE{GUANGZHAO,OTHER};

// GY39模块相关变量声明
extern unsigned char gy39_mode;
extern unsigned char recvbuf[32];
extern unsigned char data_len;
extern unsigned char recv_ok;

// GY39模块初始化
void GY39_init(void);

// 发送命令到GY39模块
uint8_t gy39_send_cmd(unsigned char mode);

// 启动GY39模块数据采集
void gy39_start(unsigned char mode);

// 获取GY39模块数据
void get_gy39_data(void);

#endif





