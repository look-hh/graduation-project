#include "PWM.h"
#include "OLED.h"
#include "GY39.h"
#include "adc.h"
#include "Delay.h"
#include "stdio.h"
#include "FreeRTOS.h"
#include "Task.h"
#include "queue.h"
// 包含STM32F10x外设驱动头文件
#include "stm32f10x_tim.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "misc.h"

// 定时器计数器1ms中断一次
uint32_t timer_count = 0;
// 1分钟对应的时间为1 * 60 * 1000 = 60000ms
#define INTERVAL_MS 60000


void TIM_Init(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	// 使用TIM2时钟
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
  
  // 配置时基参数
  // 系统时钟为72MHz，分频后时钟频率为1MHz
  TIM_TimeBaseStructure.TIM_Period = 1000 - 1; // 设置周期为1000个计数，即1ms中断一次
  TIM_TimeBaseStructure.TIM_Prescaler = 72 - 1; // 分频系数为72(1ms为一个计数)
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
  
  // 使用TIM2更新中断
  TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
  
  // 配置NVIC中断优先级
  NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  
  // 使能定时器
  TIM_Cmd(TIM2, ENABLE);	

}

extern uint16_t soil_moisture;
// 定义一个队列用于传递GY39数据请求
QueueHandle_t gy39DataQueue;

// 初始化队列
void TIM_FunctionInit(void) {
    gy39DataQueue = xQueueCreate(10, sizeof(uint8_t));
}

// 定时器定时采集函数
void TIM_Function(void) {
    // 不在此处清屏，避免与任务中的显示冲突
    if (recv_ok == 1) 
    {   
        // 发送获取GY39数据的请求到队列
        uint8_t request = 1;
        xQueueSendFromISR(gy39DataQueue, &request, NULL);
    }
    // 请求获取土壤湿度数据
    uint8_t soilRequest = 2;
    xQueueSendFromISR(gy39DataQueue, &soilRequest, NULL);
}

// 处理GY39数据获取的任务
void vGY39DataTask(void *pvParameters) {
    uint8_t request;
    for (;;) {
        // 等待队列中的请求
        if (xQueueReceive(gy39DataQueue, &request, portMAX_DELAY) == pdTRUE) {
            if (request == 1) {
                // 获取GY39数据
                get_gy39_data(); 
                // 启动下一个GY39测量
                gy39_start(gy39_mode == GUANGZHAO ? OTHER : GUANGZHAO); 
            } else if (request == 2) {
                // 获取土壤湿度数据
                soil_moisture = Get_soil_moisture_percent();   
            }
        }
    }
}


// TIM2中断服务函数
void TIM2_IRQHandler(void)
{
  // 检查TIM2更新中断标志
  if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
  {
    // 清除中断标志位
    TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
    
    // 计数器累加
    timer_count++;
    
    // 检查是否达到设定时间
    if (timer_count >= INTERVAL_MS)
    {
      // 重置计数器
      timer_count = 0;
      
      TIM_Function();//定时采集刷新数据
      printf("TIM FUNCTION\r\n");
    }
  }
}



/**
  * @brief  PWM初始化（舵机PB8 TIM4_CH3，50Hz）
  * @param  无
  * @retval 无
  */
void PWM_Init(void)
{
    /* 使能定时器时钟 */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    
    /* GPIO初始化：PB8复用推挽输出 */
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    
    /* 选择内部时钟源 */
    TIM_InternalClockConfig(TIM4);
    
    /* 时基配置：50Hz（周期20ms） */
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInitStructure.TIM_Period = 20000 - 1;               // 自动重装值ARR
    TIM_TimeBaseInitStructure.TIM_Prescaler = 72 - 1;               // 预分频值PSC
    TIM_TimeBaseInit(TIM4, &TIM_TimeBaseInitStructure);
    
    /* 输出比较配置：PWM1模式 */
    TIM_OCInitTypeDef TIM_OCInitStructure;
    TIM_OCStructInit(&TIM_OCInitStructure);
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse = 0;                              // 初始CCR值
    TIM_OC3Init(TIM4, &TIM_OCInitStructure);                        // 通道3：舵机PB8
    
    /* 启动TIM4 */
    TIM_Cmd(TIM4, ENABLE);
}


// 风扇PWM初始化 — 共用TIM4时基（由PWM_Init设定50Hz），仅配置CH4输出
void PWM_Fan_Init(void)
{
	/*开启GPIOB时钟（PWM_Init已开启，重复使能无影响）*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	/*GPIO初始化 — PB9 复用推挽输出*/
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	/*仅配置输出比较通道4（不重新初始化时基，时基由PWM_Init统一管理）*/
	TIM_OCInitTypeDef TIM_OCInitStructure;
	TIM_OCStructInit(&TIM_OCInitStructure);
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = 0;
	TIM_OC4Init(TIM4, &TIM_OCInitStructure);				//TIM4_CH4
	
	/*TIM4已在PWM_Init中使能，无需重复使能*/
}


/**
  * @brief  设置PWM比较值CCR
  * @param  Compare 要写入的CCR值，范围：0~20000
  * @retval 无
  * @note   CCR与ARR共同决定占空比，此函数仅设置CCR值，不直接设置占空比
  *          占空比Duty = CCR / (ARR + 1)
  */
void PWM_SetCompare3(uint16_t Compare)
{
	TIM_SetCompare3(TIM4, Compare);		//设置CCR3的值，通道3
}


void PWM_Fan_SetCompare(uint16_t Compare)
{
	TIM_SetCompare4(TIM4, Compare);		//设置CCR4的值, TIM4_CH4
}



