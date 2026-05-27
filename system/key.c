#include "key.h"
#include "GY39.h"
#include "show.h"

//按键 PA7/6    PA1/0

void key_init(void)
{	

	GPIO_InitTypeDef  GPIO_InitStruct;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_7 |GPIO_Pin_6 |GPIO_Pin_1 |GPIO_Pin_0 ;
	GPIO_Init(GPIOA, &GPIO_InitStruct);

	
}

void key_exti_init(void)
{
	EXTI_InitTypeDef  EXTI_InitStruct;
	NVIC_InitTypeDef  NVIC_InitStruct;
	key_init();
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);	
	 // 配置AFIO中断映射
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource0); // PA0 -> EXTI0
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource1); // PA1 -> EXTI1
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource6); // PA6 -> EXTI6
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource7); // PA7 -> EXTI7

	/*EXTI外部中断的配置*/
	EXTI_InitStruct.EXTI_Line = EXTI_Line0;
	EXTI_InitStruct.EXTI_LineCmd = ENABLE;
	EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_Init(&EXTI_InitStruct);

	EXTI_InitStruct.EXTI_Line = EXTI_Line1;
	EXTI_Init(&EXTI_InitStruct);

	EXTI_InitStruct.EXTI_Line = EXTI_Line6;
	EXTI_Init(&EXTI_InitStruct);

	EXTI_InitStruct.EXTI_Line = EXTI_Line7;
	EXTI_Init(&EXTI_InitStruct);

	
	/*NVIC 配置*/
	NVIC_InitStruct.NVIC_IRQChannel = EXTI0_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 1;
	NVIC_Init(&NVIC_InitStruct);
	NVIC_InitStruct.NVIC_IRQChannel = EXTI1_IRQn;
	NVIC_Init(&NVIC_InitStruct);
	NVIC_InitStruct.NVIC_IRQChannel = EXTI9_5_IRQn;
	NVIC_Init(&NVIC_InitStruct);
	
	

}
int key_get_status(int key_num)
{
	switch(key_num)
	{
		case 0:
			return GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_7);
		case 1:
			return GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_6);
		case 2:
			return GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_1);
		case 3:
			return GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0);	
	}
	return 0;

}


extern uint8_t key3_flag;  
extern uint8_t key2_flag;
extern uint8_t key1_flag;
extern uint8_t key0_flag;



//按键KEY3
// 按键3中断服务函数 (PA0 -> EXTI0)
// 优化后的中断服务函数
void EXTI0_IRQHandler(void) 
	{
    if (EXTI_GetITStatus(EXTI_Line0) != RESET) 
			{
        // 1. 快速清除中断标志（放在最前面，避免重复触发）
        EXTI_ClearITPendingBit(EXTI_Line0);
        
        // 2. 非阻塞消抖：仅检测按键状态，不等待
        if (key_get_status(KEY3) == KEY_DOWN) 
				{  // 检测到按键按下
            // 3. 记录按键事件（用标志位，不在ISR中执行具体操作）
            key3_flag = 1;  // 仅置位标志位，主循环处理实际逻辑
        }
    }
}

//按键KEY2
// 按键2中断服务函数 (PA1 -> EXTI1)
void EXTI1_IRQHandler(void) 
{
    if (EXTI_GetITStatus(EXTI_Line1) != RESET) 
    {
        // 1. 优先清除中断标志，避免重复触发
        EXTI_ClearITPendingBit(EXTI_Line1);
        
        // 2. 非阻塞检测按键状态（仅判断当前是否按下，不等待）
        if (key_get_status(KEY2) == KEY_DOWN)
        {
            // 3. 置位标志位，主循环将处理具体逻辑
            key2_flag = 1;
        }
    }
}


//按键KEY0 按键KEY1
// 按键0 1中断服务函数 (PA6 -> EXTI6) (PA7 -> EXTI7)
void EXTI9_5_IRQHandler(void) 
{	
    // 处理KEY1（EXTI_Line6）
    if (EXTI_GetITStatus(EXTI_Line6) != RESET) 
    {
        // 1. 立即清除中断标志
        EXTI_ClearITPendingBit(EXTI_Line6);
        
        // 2. 检测按键状态（非阻塞）
        if (key_get_status(KEY1) == KEY_DOWN)
        {
            // 3. 置位事件标志
            key1_flag = 1;
        }
    }

    // 处理KEY0（EXTI_Line7）
    if (EXTI_GetITStatus(EXTI_Line7) != RESET) 
    {
        // 1. 立即清除中断标志
        EXTI_ClearITPendingBit(EXTI_Line7);
        
        // 2. 检测按键状态（非阻塞）
        if (key_get_status(KEY0) == KEY_DOWN)
        {
            // 3. 置位事件标志
            key0_flag = 1;
        }
    }
}






