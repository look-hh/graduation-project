#ifndef __UART_H__
#define __UART_H__
#include "stm32f10x.h"

// UART1初始化函数
void uart1_init(uint32_t BaudRate);

// UART2初始化函数
void uart2_init(uint32_t BaudRate);

// UART3初始化函数
void uart3_init(uint32_t BaudRate);

// 串口接收字符串函数
uint8_t usart_receiveString(USART_TypeDef* USARTx, uint8_t *buf, uint8_t maxLen, uint32_t timeout);

// 串口发送字符串函数
void usart_sendString(USART_TypeDef* USARTx, uint8_t *buf, uint8_t len);

#endif

