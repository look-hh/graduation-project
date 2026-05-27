#include "led.h"

//led6 PA11 
//led7 PA12 
//led8 PA15 
//led9 PB3 


void led_init(void)
{
		/*开启时钟*/
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB, ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable , ENABLE);//STM32f103 PA13/14/15 PB3/4复位后初始为JATG功能 
																//要重映射为GPIO口
	
	/*GPIO初始化*/
	GPIO_InitTypeDef GPIO_InitStructure;					//定义结构体变量
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;		//GPIO模式，赋值为推挽输出模式
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 |GPIO_Pin_12|GPIO_Pin_15;				//GPIO引脚，赋值为第0号引脚
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		//GPIO速度，赋值为50MHz
	
	GPIO_Init(GPIOA, &GPIO_InitStructure);					//将赋值后的构体变量传递给GPIO_Init函数
															//函数内部会自动根据结构体的参数配置相应寄存器
															//实现GPIOA的初始化
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_Init(GPIOB, &GPIO_InitStructure);	

	//设置默认状态
	GPIO_SetBits(GPIOA, GPIO_Pin_11|GPIO_Pin_12 |GPIO_Pin_15);
	GPIO_SetBits(GPIOB, GPIO_Pin_3);


}

extern uint8_t SUN_flag;
void led_ctrl(int led_num, int led_state)
{
    switch(led_num)
    {
        case LED0:
            if(led_state == LED_TOGGLE)
            {
                // 反转状态：1 - 当前状态
                GPIO_WriteBit(GPIOA, GPIO_Pin_11,(BitAction)(1 - GPIO_ReadOutputDataBit(GPIOA, GPIO_Pin_11)));
            }
            else
            {
                // 直接设置为目标状态(ON/OFF)
                GPIO_WriteBit(GPIOA, GPIO_Pin_11, (BitAction)led_state);
            }
            break;
        
        case LED1:
            if(led_state == LED_TOGGLE)
            {
                GPIO_WriteBit(GPIOA, GPIO_Pin_12,(BitAction)(1 - GPIO_ReadOutputDataBit(GPIOA, GPIO_Pin_12)));
            }
            else
            {
                GPIO_WriteBit(GPIOA, GPIO_Pin_12, (BitAction)led_state);
            }
            break;
        
        case LED2:  // 补光LED
            if(led_state == LED_TOGGLE)
            {
                GPIO_WriteBit(GPIOA, GPIO_Pin_15,(BitAction)(1 - GPIO_ReadOutputDataBit(GPIOA, GPIO_Pin_15)));
            }
            else
            {
                GPIO_WriteBit(GPIOA, GPIO_Pin_15, (BitAction)led_state);
            }
						if(GPIO_ReadOutputDataBit(GPIOA, GPIO_Pin_15) == 0)
						{
								SUN_flag = 1;  // 低电平点亮时设置标志位
						}
						else
						{
								SUN_flag = 0;  // 高电平时清除标志位
						}
            break;
        
        case LED3:
            if(led_state == LED_TOGGLE)
            {
                GPIO_WriteBit(GPIOB, GPIO_Pin_3,(BitAction)(1 - GPIO_ReadOutputDataBit(GPIOB, GPIO_Pin_3)));
            }
            else
            {
                GPIO_WriteBit(GPIOB, GPIO_Pin_3, (BitAction)led_state);
            }
            break;
    }
}



