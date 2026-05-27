#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_usart.h"
#include "stm32f10x_rcc.h"
#include "misc.h"
#include "uart.h"
#include "stdio.h"
#include "string.h"
#include "GY39.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"

extern SemaphoreHandle_t xTask3_Sem;  // 添加extern声明以解决未定义问题

//UART1_TX PA9
//UART1_RX PA10

void uart1_init(uint32_t BaudRate)//串口参数配置
{
	GPIO_InitTypeDef  GPIO_InitStruct;
	USART_InitTypeDef  USART_InitStruct;
	NVIC_InitTypeDef  NVIC_InitStruct;
	
	//GPIO配置
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10;
	GPIO_Init(GPIOA, &GPIO_InitStruct);

	//串口初始化
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	USART_InitStruct.USART_BaudRate = BaudRate;
	USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStruct.USART_Mode = USART_Mode_Rx|USART_Mode_Tx;
	USART_InitStruct.USART_Parity = USART_Parity_No;
	USART_InitStruct.USART_StopBits = USART_StopBits_1;
	USART_InitStruct.USART_WordLength = USART_WordLength_8b;
	USART_Init(USART1, &USART_InitStruct);
	//中断配置
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//使能接收中断事件
	NVIC_InitStruct.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 2;
	NVIC_Init(&NVIC_InitStruct);
	//UART使能
	USART_Cmd(USART1, ENABLE);
	
}

//UART2_TX PA2
//UART2_RX PA3
//蓝牙模块的初始化
void uart2_init(uint32_t BaudRate)
{
	GPIO_InitTypeDef  GPIO_InitStruct;
	USART_InitTypeDef  USART_InitStruct;
	NVIC_InitTypeDef  NVIC_InitStruct;
	
	//GPIO配置
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;//TX 配置为复用推挽输出
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;//RX 配置为浮空输入模式
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_3;
	GPIO_Init(GPIOA, &GPIO_InitStruct);

	//UART初始化
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	USART_InitStruct.USART_BaudRate = BaudRate;
	USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStruct.USART_Mode = USART_Mode_Rx|USART_Mode_Tx;
	USART_InitStruct.USART_Parity = USART_Parity_No;
	USART_InitStruct.USART_StopBits = USART_StopBits_1;
	USART_InitStruct.USART_WordLength = USART_WordLength_8b;
	USART_Init(USART2, &USART_InitStruct);
	//uart中断配置
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
	NVIC_InitStruct.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 3;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 4;
	NVIC_Init(&NVIC_InitStruct);
	//UART使能
	USART_Cmd(USART2, ENABLE);

}



//UART3_TX PB10
//UART3_RX PB11
void uart3_init(uint32_t BaudRate)
{
	GPIO_InitTypeDef  GPIO_InitStruct;
	USART_InitTypeDef  USART_InitStruct;
	NVIC_InitTypeDef  NVIC_InitStruct;


	//GPIO配置
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStruct);
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_11;
	GPIO_Init(GPIOB, &GPIO_InitStruct);

	//串口初始化
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
	USART_InitStruct.USART_BaudRate = BaudRate;
	USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStruct.USART_Mode = USART_Mode_Rx|USART_Mode_Tx;
	USART_InitStruct.USART_Parity = USART_Parity_No;
	USART_InitStruct.USART_StopBits = USART_StopBits_1;
	USART_InitStruct.USART_WordLength = USART_WordLength_8b;
	USART_Init(USART3, &USART_InitStruct);
	//中断配置
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);//使能接收中断事件
	NVIC_InitStruct.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;
	NVIC_Init(&NVIC_InitStruct);
	//UART使能
	USART_Cmd(USART3, ENABLE);

}


//串口接收字符串
uint8_t usart_receiveString(USART_TypeDef* USARTx, uint8_t *buf, uint8_t maxLen, uint32_t timeout)
{
	uint8_t len = 0;

	while(len < maxLen)
	{
		uint32_t t = timeout;

		while(USART_GetFlagStatus(USARTx, USART_FLAG_RXNE == RESET))
		{
			if(--t == 0)
			{
				vTaskDelay(1);
				buf[len] = '\0';
				return 0;
			}
		}
		buf[len++] = USART_ReceiveData(USARTx);

		// 检查是否为回车或换行符（命令结束符）
    	if(buf[len-1] == '\r' || buf[len-1] == '\n')
        break;
	}

	buf[len] = '\0';
	return len;
            

}

uint8_t rx_buf[50] = {0};  
uint8_t rx_len = 0;        // 已接收的字符数

extern volatile uint8_t cmd_ready; 
void USART2_IRQHandler(void)
{
    // 检查是否是接收中断（RXNE标志表示数据寄存器非空）
    if (USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
    {
        // 读取接收到的单个字符
        uint8_t ch = USART_ReceiveData(USART2);
        
        // 检查字符是否为回车或换行符（命令结束符）
        if (ch == '\r' || ch == '\n') {
            // 只有当已接收到数据时才处理命令
            if (rx_len > 0) {
                // 将结束符替换为字符串结束符'\0'
                rx_buf[rx_len] = '\0';
                // 设置命令接收完成标志
                cmd_ready = 1;
                // 打印接收到的完整命令用于调试
                printf("Received command via bluetooth: [%s]\r\n", rx_buf);
            }
            // 重置接收长度计数器，准备下次接收
            rx_len = 0;
        } else {
            // 检查字符缓冲区防止溢出（保留1个位置给'\0'）
            if (rx_len < sizeof(rx_buf) - 1) {
                rx_buf[rx_len++] = ch;
            } else {
                // 缓冲区溢出，重置接收状态
                rx_len = 0;
                memset(rx_buf, 0, sizeof(rx_buf));
            }
        }
        
        // 清除中断标志位，为下一次接收做准备
        USART_ClearITPendingBit(USART2, USART_IT_RXNE);
        if (USART_GetITStatus(USART2, USART_IT_ORE) != RESET)
        {
            USART_ClearITPendingBit(USART2, USART_IT_ORE);  // 清除溢出错误标志
            (void)USART_ReceiveData(USART2);  // 读取数据寄存器以清除ORE标志
        }
    }
}



void USART3_IRQHandler(void)
{
    u8 data;
		if (USART_GetITStatus(USART3, USART_IT_ORE) != RESET) 
		{
			USART_ClearITPendingBit(USART3, USART_IT_ORE);  // 清除错误标志
			(void)USART_ReceiveData(USART3);  // 读取数据寄存器以清除ORE标志
		}
    if(USART_GetITStatus(USART3, USART_IT_RXNE) == SET)
    {
        data = USART_ReceiveData(USART3);
        // 判断帧头是否匹配
        if (data_len == 0)
        {
            if (data != 0x5A)  // 判断帧头是否为0x5A,否则
            {
                data_len = 0;
            }
            else  // 帧头正确
            {
                recvbuf[data_len++] = data;
            }
        }
        else if (data_len == 1)//第二个字节(帧头)
        {
            if (data != 0x5A)  
            {
                data_len = 0;
            }
            else  
            {
                recvbuf[data_len++] = data;
            }
        }
        else  
        {
            if (data_len < 32)  
            {
                recvbuf[data_len++] = data;
            }
        }

        if (gy39_mode == GUANGZHAO && data_len == 9)
        {

            u8 checksum = 0;
            for (u8 i = 0; i < 8; i++) checksum += recvbuf[i];
            if (checksum == recvbuf[8])  
            {
                recv_ok = 1;
                // 新增：数据到达时释放信号量
                BaseType_t xHigherPriorityTaskWoken = pdFALSE;
                xSemaphoreGiveFromISR(xTask3_Sem, &xHigherPriorityTaskWoken);
                portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
            }
            else 
            {
                data_len = 0;
            }
        }
        else if (gy39_mode == OTHER && data_len == 15)
        {
            u8 checksum = 0;
            for (u8 i = 0; i < 14; i++) 
                checksum += recvbuf[i];
            if (checksum == recvbuf[14])
            {
                recv_ok = 1;
                // 新增：数据到达时释放信号量
                BaseType_t xHigherPriorityTaskWoken = pdFALSE;
                xSemaphoreGiveFromISR(xTask3_Sem, &xHigherPriorityTaskWoken);
                portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
            }
            else 
            {
                data_len = 0;
            }
        }

        USART_ClearITPendingBit(USART3, USART_IT_RXNE);
    }
}





void usart_sendString(USART_TypeDef* USARTx, uint8_t *buf, uint8_t len)
{
	for(int i=0;i<len;i++)
	{
		USART_SendData(USARTx, buf[i]);
		while(USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET);
	}
}

//标准库需要的支持函数
struct __FILE
{
	int handle;
};

FILE  __stdout;


//重定义fputc
int fputc(int ch, FILE *f)
{
	while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);
    // 发送一个字节数据
    USART_SendData(USART2, (uint8_t)ch);
	return ch;
}
