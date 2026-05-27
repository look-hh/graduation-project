#include "stm32f10x.h"                  // Device header
#include "FreeRTOS.h"
#include "Task.h"
#include "queue.h"
#include "semphr.h" // 添加信号量头文件

#include "Delay.h"
#include "led.h"
#include "beep.h"
#include "key.h"
#include "OLED.h"
#include "uart.h"
#include "stdio.h"
#include "PWM.h"
#include "servo.h"
#include "adc.h"
#include "GY39.h"
#include "blue.h"
#include "show.h"

// 硬件引脚定义
// LED6 PA11, LED7 PA12, LED8 PA15, LED9 PB3

float Angle = 0;              // 舵机角度变量
uint16_t soil_moisture = 0;   // 土壤湿度

extern uint32_t WD;           // 温度（来自GY39）

/* ========== 自动控制阈值（初始默认值）========== */
uint32_t SET_WD = 28;         // 风扇自动控制温度阈值（°C）
uint32_t SET_SD = 50;         // 水泵自动控制湿度阈值（%）
uint32_t SET_LUX = 200;       // 补光灯自动控制光照阈值（lux）

/* ========== 硬件状态标志位 ========== */
uint8_t Fan_flag = 0;        // 风扇状态 0=关 1=开
uint8_t WP_flag = 0;          // 水泵（舵机）状态 0=关 1=开
uint8_t SUN_flag = 0;         // 补光灯状态 0=关 1=开

/* ========== 自动控制开关（默认开启）========== */
int FAN_AUTO_CTRL = AUTO_CTRL_ON;    // 风扇自动控制开关（默认开启）
int WP_AUTO_CTRL = AUTO_CTRL_OFF;    // 水泵自动控制开关
int LED_AUTO_CTRL = AUTO_CTRL_ON;     // 补光自动控制开关（默认开启）
int BEEP_AUTO_CTRL = AUTO_CTRL_OFF;   // 报警自动控制开关（默认关闭）

/* ========== 按键事件标志位 ========== */
uint8_t key3_flag = 0;  // KEY3 PA0
uint8_t key2_flag = 0;  // KEY2 PA1
uint8_t key1_flag = 0;  // KEY1 PA6
uint8_t key0_flag = 0;  // KEY0 PA7

/* ========== FreeRTOS 任务句柄 ========== */
TaskHandle_t myTaskHandler1;
TaskHandle_t myTaskHandler2;
TaskHandle_t myTaskHandler3;
SemaphoreHandle_t xTask3_Sem = NULL; // 信号量句柄

/**
 * @brief  按键控制处理
 * 
 * 按键功能映射：
 *   KEY3 (PA0) - 光照主动开关（补光灯）
 *   KEY2 (PA1) - 舵机主动开关（水泵）
 *   KEY1 (PA6) - 风扇手动开关
 *   KEY0 (PA7) - 显示舵机/风扇/水泵/补光灯状态
 */
void key_ctrl(void)
{
    // 处理KEY3事件：光照开关（补光灯）
    if (key3_flag) 
    {
        vTaskDelay(20);
        if (key_get_status(KEY3) == KEY_DOWN) 
        {
            // 切换补光灯状态
            if (SUN_flag == 0)
            {
                led_ctrl(LED2, LED_ON);
                SUN_flag = 1;
                // 手动操作后进入手动模式，自动控制不干预
                Light_SetManualMode();
                printf("[KEY] Light ON (Manual mode)\r\n");
            }
            else
            {
                led_ctrl(LED2, LED_OFF);
                SUN_flag = 0;
                // 手动关闭后保持手动模式，自动控制也不能开启
                Light_SetManualMode();
                printf("[KEY] Light OFF (Manual mode)\r\n");
            }
        }
        key3_flag = 0;
    }
    
    // 处理KEY2事件：舵机开关（水泵）
    if (key2_flag) 
    {
        vTaskDelay(20);
        if (key_get_status(KEY2) == KEY_DOWN) 
        {
            // 切换舵机/水泵状态
            if (WP_flag == 0)
            {
                Angle = 180;
                Servo_SetAngle(180);
                WP_flag = 1;
                led_ctrl(LED1, LED_ON);     // 水泵开启指示灯
                printf("[KEY] WP ON (Servo 180)\r\n");
            }
            else
            {
                Angle = 0;
                Servo_SetAngle(0);
                WP_flag = 0;
                led_ctrl(LED1, LED_OFF);    // 水泵关闭指示灯
                printf("[KEY] WP OFF (Servo 0)\r\n");
            }
        }
        key2_flag = 0;
    }
    
    // 处理KEY1事件：风扇开关
    if (key1_flag) 
    {
        vTaskDelay(20);
        if (key_get_status(KEY1) == KEY_DOWN) 
        {
            // 切换风扇状态
            if (Fan_flag == 0)
            {
                Fan_SetSpeed(10000);
                Fan_flag = 1;
                // 手动操作后进入手动模式，自动控制不干预
                Fan_SetManualMode();
                printf("[KEY] Fan ON (Manual mode)\r\n");
            }
            else
            {
                Fan_SetStop();
                Fan_flag = 0;
                // 手动关闭后保持手动模式，自动控制也不能开启
                Fan_SetManualMode();
                printf("[KEY] Fan OFF (Manual mode)\r\n");
            }
        }
        key1_flag = 0;
    }

    // 处理KEY0事件：显示设备状态
    if (key0_flag) 
    {
        vTaskDelay(20);
        if (key_get_status(KEY0) == KEY_DOWN) 
        {
            OLED_Clear();
            show_Servo_Fan_data();
            printf("[KEY] Status displayed\r\n");
        }
        key0_flag = 0;
    }
}

/**
 * @brief  Task1：自动控制任务（2秒周期）
 */
void myTask1(void *arg)
{
    TickType_t last_send_tick = 0;  // 蓝牙上次发送时间

    while(1)
    {
        // 自动控制逻辑：2秒周期（保持快速响应）
        Fan_PID_ctrl(SET_WD);       // 风扇PID控制（位置式PID + 手动优先）
        WP_auto_ctrl(SET_SD);       // 水泵自动控制（土壤湿度，带滞回）
        LED_auto_ctrl(SET_LUX);      // 补光自动控制（带滞回+手动优先）
        Alarm_auto_ctrl();           // 报警自动控制

        // 蓝牙发送：15秒周期（使用缓存数据）
        if ((xTaskGetTickCount() - last_send_tick) >= pdMS_TO_TICKS(15000))
        {
            send_bluetooth_data();
            last_send_tick = xTaskGetTickCount();
        }

        vTaskDelay(2000);  // 2秒周期
    }
}

/**
 * @brief  Task2：交互任务（100ms周期）
 */
void myTask2(void *arg)
{
    while(1)
    {
        process_bluetooth_cmd(); // 处理蓝牙命令
        key_ctrl();              // 处理按键事件
        vTaskDelay(100);
    }
}

/**
 * @brief  Task3：传感器数据处理任务（信号量驱动）+ 定时采集触发
 */
void myTask3(void *arg)
{
    TickType_t last_collect_tick = 0;  // 上次采集触发时间

    // 首次触发：开始光照采集
    gy39_start(GUANGZHAO);
    last_collect_tick = xTaskGetTickCount();

    while(1)
    {
        if (xSemaphoreTake(xTask3_Sem, portMAX_DELAY) == pdTRUE)
        {
            if (recv_ok == 1)
            {
                OLED_Clear();
                get_gy39_data();  // 解析数据并更新蓝牙发送缓存
                show_gy39_data();
                recv_ok = 0;
                data_len = 0;
            }
            soil_moisture = Get_soil_moisture_percent();
        }

        // 每2秒触发一次GY39采集（保持原有采集频率）
        if ((xTaskGetTickCount() - last_collect_tick) >= pdMS_TO_TICKS(2000))
        {
            gy39_start(gy39_mode == GUANGZHAO ? OTHER : GUANGZHAO);
            last_collect_tick = xTaskGetTickCount();
        }
    }
}

/**
 * @brief  主函数
 */
int main(void)
{
    // 外设初始化
    OLED_Init();             // OLED初始化
    led_init();              // LED初始化
    beep_init();             // 蜂鸣器初始化
    key_init();              // 按键初始化
    key_exti_init();         // 按键中断初始化
    uart1_init(9600);       // 串口1初始化（调试）
    PWM_Init();              // PWM初始化（舵机 PB8, TIM4_CH3）
    PWM_Fan_Init();          // 风扇PWM初始化（PB9, TIM4_CH4，与舵机共用50Hz时基）
    Soil_moisture_Init();    // 土壤湿度模块初始化
    GY39_init();             // GY39初始化
    bluetooth_init();        // 蓝牙模块初始化
    
    // 舵机初始角度（水阀关闭）
    Servo_SetAngle(0);
    printf("[INIT] Servo initialized to 0\r\n");

    // 创建信号量
    xTask3_Sem = xSemaphoreCreateBinary();
    if (xTask3_Sem == NULL)
    {
        while(1); // 信号量创建失败
    }

    // 创建任务
    xTaskCreate(myTask1, "myTask1", 256, NULL, 3, &myTaskHandler1);
    xTaskCreate(myTask2, "myTask2", 256, NULL, 2, &myTaskHandler2);
    xTaskCreate(myTask3, "myTask3", 512, NULL, 4, &myTaskHandler3);

    // 启动调度器
    vTaskStartScheduler();

    while(1);
}
