#ifndef __BEEP_H__
#define __BEEP_H__
#include "stm32f10x.h"  

enum BEEP_STATE{BEEP_OFF,BEEP_ON};//高电平开启 低电平关闭

void beep_init(void);

void beep_ctrl(int beep_state);




#endif
