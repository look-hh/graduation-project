#ifndef __OLED_H
#define __OLED_H

// OLED初始化函数
void OLED_Init(void);

// OLED清屏函数
void OLED_Clear(void);

// OLED显示字符函数
void OLED_ShowChar(uint8_t Line, uint8_t Column, char Char);

// OLED显示字符串函数
void OLED_ShowString(uint8_t Line, uint8_t Column, char *String);

// OLED显示数字（十进制，正数）函数
void OLED_ShowNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length);

// OLED显示数字（十进制，带符号数）函数
void OLED_ShowSignedNum(uint8_t Line, uint8_t Column, int32_t Number, uint8_t Length);

// OLED显示数字（十六进制，正数）函数
void OLED_ShowHexNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length);

// OLED显示数字（二进制，正数）函数
void OLED_ShowBinNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length);

// OLED显示测试函数
void OLED_Show(void);

// 获取数字长度函数
uint8_t get_num_len(uint32_t num);

#endif



