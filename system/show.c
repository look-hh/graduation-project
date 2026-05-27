#include "show.h"
#include "GY39.h"
#include "OLED.h"
#include "adc.h"
#include "Delay.h"
#include "servo.h"
#include "stdio.h"
#include "led.h"
#include "beep.h"
#include "FreeRTOS.h"
#include "Task.h"
#include "queue.h"

extern float Angle;
extern uint8_t Fan_flag;
extern uint8_t WP_flag;
extern uint8_t SUN_flag;
extern uint16_t soil_moisture;
extern uint32_t SET_WD;
extern uint32_t SET_SD;
extern uint32_t SET_LUX;
extern uint32_t Lux;
extern uint32_t WD;
extern uint32_t QY;
extern uint32_t SD;
extern uint32_t HB;
extern unsigned char gy39_mode;

/* 警报系统前向声明 */
static const char* alarm_get_name(uint8_t type);

/* ========== 警报系统全局状态 ========== */
static uint8_t  alarm_active = 0;
static uint8_t  alarm_type = 0;
static uint32_t alarm_value = 0;
static uint8_t  alarm_pulse = 0;

/* ========== 手动模式标志位 ========== */
// 风扇手动模式：手动开启后，自动控制不参与关闭
static uint8_t Fan_Manual_Mode = 0;
// 补光手动模式：手动开启后，自动控制不参与关闭
static uint8_t Light_Manual_Mode = 0;

/* ========== 蓝牙发送缓存（每15秒发送一次）========== */
// 缓存上一轮采集的数据，确保15秒发送时数据完整
static uint32_t cache_Lux = 0;
static uint32_t cache_Soil = 0;
static uint32_t cache_WD = 0;
static uint32_t cache_QY = 0;
static uint32_t cache_SD = 0;
static uint32_t cache_HB = 0;
static uint8_t cache_mode = 0;  // 记录上次采集的模式

/**
 * @brief  更新GY39数据缓存（采集完成后调用）
 * @note   数据由 get_gy39_data() 解析后自动更新到全局变量，此处同步到缓存
 */
void update_gy39_cache(void)
{
    // 同步全局变量到缓存
    cache_Lux = Lux;
    cache_Soil = soil_moisture;
    cache_WD = WD;
    cache_QY = QY;
    cache_SD = SD;
    cache_HB = HB;
    cache_mode = gy39_mode;
}

/**
 * @brief  发送蓝牙数据（每15秒调用一次）
 */
void send_bluetooth_data(void)
{
    printf("\r\n");
    printf("========== Sensor Data ==========\r\n");
    printf("[ Light ] %d lux\r\n", cache_Lux);
    printf("[ Soil  ] %d %%\r\n", cache_Soil);
    printf("[ Temp  ] %d C\r\n", cache_WD);
    printf("[ Press ] %d Pa\r\n", cache_QY);
    printf("[ Humid ] %d %%\r\n", cache_SD);
    printf("[ Alt   ] %d m\r\n", cache_HB);
    printf("[ Alarm ] %s\r\n", alarm_active ? (char*)alarm_get_name(alarm_type) : "None");
    printf("==================================\r\n");
}

/**
 * @brief  风扇进入手动模式（手动开启后调用）
 */
void Fan_SetManualMode(void)
{
    Fan_Manual_Mode = 1;
    // printf("[MODE] Fan entered MANUAL mode\r\n");
}

/**
 * @brief  风扇退出手动模式（开启自动控制时调用）
 */
void Fan_ClearManualMode(void)
{
    Fan_Manual_Mode = 0;
    // printf("[MODE] Fan exited MANUAL mode\r\n");
}

/**
 * @brief  补光进入手动模式（手动开启后调用）
 */
void Light_SetManualMode(void)
{
    Light_Manual_Mode = 1;
    // printf("[MODE] Light entered MANUAL mode\r\n");
}

/**
 * @brief  补光退出手动模式（开启自动控制时调用）
 */
void Light_ClearManualMode(void)
{
    Light_Manual_Mode = 0;
    // printf("[MODE] Light exited MANUAL mode\r\n");
}

// 轮流显示GY39数据（光照或环境参数）
void show_gy39_data(void)
{
    OLED_Clear();
    if (gy39_mode == GUANGZHAO) {
        OLED_ShowString(1, 1, "Light:");
        OLED_ShowNum(1, 7, Lux, 5);
        OLED_ShowString(1, 13, "lux");
        show_soil_moisture();
    } else {
        OLED_ShowString(1, 1, "Temp:");
        OLED_ShowNum(1, 6, (uint16_t)WD, 2);
        OLED_ShowString(1, 9, "C");
        OLED_ShowString(2, 1, "Hum/RH:");
        OLED_ShowNum(2, 8, (uint16_t)SD, 2);
        OLED_ShowString(2, 10, "%");
        OLED_ShowString(3, 1, "Pres:");
        OLED_ShowNum(3, 6, (uint32_t)QY, get_num_len(QY));
        OLED_ShowString(3, get_num_len(QY)+6, "pa");
        OLED_ShowString(4, 1, "Alt:");
        OLED_ShowNum(4, 5, (uint16_t)HB, get_num_len(HB));
        OLED_ShowString(4, get_num_len(HB)+5, "m");
    }
    // 添加短暂延时，防止刷屏过快
    vTaskDelay(10 / portTICK_PERIOD_MS);
}

void show_Servo_Fan_data(void)
{
    OLED_Clear();
    OLED_ShowString(1, 1, "Angle:");
    OLED_ShowNum(1, 7, Angle, get_num_len(Angle));
    OLED_ShowString(2, 1, "WPstatus:");
    if (WP_flag == 1)
        OLED_ShowString(2, 10, "open");
    else
        OLED_ShowString(2, 10, "close");
    OLED_ShowString(3, 1, "Fanstatus:");
    if (Fan_flag == 1)
        OLED_ShowString(3, 11, "open");
    else
        OLED_ShowString(3, 11, "close");
    OLED_ShowString(4, 1, "SUNstatus:");
    if (SUN_flag == 1)
        OLED_ShowString(4, 11, "open");
    else
        OLED_ShowString(4, 11, "close");
    // 添加短暂延时，防止刷屏过快
    vTaskDelay(50 / portTICK_PERIOD_MS);
}

void show_soil_moisture(void)
{
    soil_moisture = Get_soil_moisture_percent();
    // 减少延时时间，避免阻塞太长时间
    vTaskDelay(10 / portTICK_PERIOD_MS);
    OLED_ShowString(2, 1, "Soil:");
    OLED_ShowNum(2, 6, soil_moisture, get_num_len(soil_moisture));
    OLED_ShowString(2, 6 + get_num_len(soil_moisture), "%");
}

/* ========== 风扇PID控制（位置式PID + 手动优先）========== */
extern int FAN_AUTO_CTRL;

/* PID参数（论文仿真值 Kp=2.0, Ki=0.1, Kd=0.5，经适当缩放适配本系统）
 * 控制周期 T=2s（myTask1周期）
 * 公式：u(k) = Kp*e(k) + Ki*Σ[e(j)*T] + Kd*[e(k)-e(k-1)]/T
 * e = 当前温度 - 目标温度（正偏差=偏热→加速）
 * 输出映射到PWM占空比 0~10000
 */
#define FAN_KP    200.0f    // 比例系数
#define FAN_KI     10.0f    // 积分系数
#define FAN_KD     50.0f    // 微分系数
#define FAN_PID_T   2.0f    // 采样周期
#define FAN_PWM_MAX 10000.0f
#define FAN_PWM_MIN 3000     // 风扇最小启动PWM（低于此值风扇转不动，约30%占空比）

/* PID状态变量 */
static float fan_pid_integral = 0.0f;
static float fan_prev_error = 0.0f;

void Fan_PID_ctrl(uint32_t set_wd)
{
    float setpoint, pv, error;
    float p_out, i_out, d_out, output;
    uint16_t pwm_val;

    /* 手动模式检查 */
    if (Fan_Manual_Mode == 1) return;
    if (FAN_AUTO_CTRL != AUTO_CTRL_ON) return;

    setpoint = (float)set_wd;
    pv = (float)WD;

    /* 偏差：正数=太热，需要降温 */
    error = pv - setpoint;

    /* 死区：偏差在±0.3°C以内视为达到目标，关闭风扇 */
    if (error > -0.3f && error < 0.3f)
    {
        error = 0.0f;
        fan_pid_integral = 0.0f;
        if (Fan_flag == 1)
        {
            Fan_SetStop();
            Fan_flag = 0;
        }
        return;
    }

    /* ===== 比例项 ===== */
    p_out = FAN_KP * error;

    /* ===== 积分项（带抗饱和限幅）===== */
    fan_pid_integral += error * FAN_PID_T;
    /* 积分限幅：防止积分饱和 */
    if (fan_pid_integral > 100.0f)    fan_pid_integral = 100.0f;
    if (fan_pid_integral < 0.0f)      fan_pid_integral = 0.0f;
    i_out = FAN_KI * fan_pid_integral;

    /* ===== 微分项 ===== */
    d_out = FAN_KD * (error - fan_prev_error) / FAN_PID_T;
    fan_prev_error = error;

    /* ===== 合成输出 ===== */
    output = p_out + i_out + d_out;
    /* 输出限幅 0~PWM_MAX */
    if (output > FAN_PWM_MAX) output = FAN_PWM_MAX;
    if (output < 0.0f)        output = 0.0f;

    /* ===== 最小启动PWM（风扇低于此值转不动）===== */
    if (output > 0.0f && output < FAN_PWM_MIN)
    {
        output = (float)FAN_PWM_MIN;
    }

    /* ===== 应用到风扇 ===== */
    pwm_val = (uint16_t)output;
    if (pwm_val > 0)
    {
        Fan_SetSpeed(pwm_val);
        Fan_flag = 1;
    }
    else
    {
        if (Fan_flag == 1)
        {
            Fan_SetStop();
            Fan_flag = 0;
        }
    }
}

/* ========== 补光自动控制（带滞回 + 手动优先）========== */
extern int LED_AUTO_CTRL;
void LED_auto_ctrl(uint32_t set_lux)
{
    /* 手动模式检查：手动开启后，自动控制不再干预 */
    if (Light_Manual_Mode == 1)
    {
        // printf("[AUTO] Light in MANUAL mode, skip\r\n");
        return;
    }

    if (LED_AUTO_CTRL != AUTO_CTRL_ON)
    {
        return;
    }

    /* 滞回控制：
     * 开启条件：光照 < 阈值（200 lux）
     * 关闭条件：光照 >= 阈值+20（220 lux）
     */
    if (Lux < set_lux)
    {
        if (SUN_flag == 0)
        {
            led_ctrl(LED2, LED_ON);
            // printf("[AUTO] Light ON (Lux<%d, Lux=%d)\r\n", set_lux, Lux);
        }
    }
    else if (Lux >= set_lux + 20)
    {
        if (SUN_flag == 1)
        {
            led_ctrl(LED2, LED_OFF);
            // printf("[AUTO] Light OFF (Lux>=%d, Lux=%d)\r\n", set_lux + 20, Lux);
        }
    }
    /* 滞回区间（set_lux <= Lux < set_lux+20）保持现状 */
}

extern int WP_AUTO_CTRL;
void WP_auto_ctrl(uint32_t set_sd)
{
    if (WP_AUTO_CTRL == AUTO_CTRL_ON)
    {
        /* 滞回控制：
         * 开启条件：土壤湿度 < 阈值（太干，需要浇水）
         * 关闭条件：土壤湿度 >= 阈值+10（足够湿润）
         * 滞回区间（set_sd <= soil_moisture < set_sd+10）保持现状
         */
        if (soil_moisture < set_sd)
        {
            if (WP_flag == 0)
            {
                Angle = 180;
                Servo_SetAngle(180);
                WP_flag = 1;
                led_ctrl(LED1, LED_ON);     // 水泵开启指示灯
            }
        }
        else if (soil_moisture >= set_sd + 10)
        {
            if (WP_flag == 1)
            {
                Angle = 0;
                Servo_SetAngle(0);
                WP_flag = 0;
                led_ctrl(LED1, LED_OFF);    // 水泵关闭指示灯
            }
        }
    }
}

/* ========== 警报系统（滞回+脉冲+OLED+蓝牙）========== */
extern int BEEP_AUTO_CTRL;

/* 报警类型编码 */
#define ALARM_NONE    0
#define ALARM_TEMP_H  1   // 温度过高
#define ALARM_TEMP_L  2   // 温度过低
#define ALARM_SOIL_L  3   // 土壤过干

/* 滞回阈值：触发值 / 恢复值 */
#define AH_TH_HIGH_TRIG  35    // 温度过高触发
#define AH_TH_HIGH_REC   33    // 温度恢复
#define AH_TH_LOW_TRIG    5    // 温度过低触发
#define AH_TH_LOW_REC     7    // 温度恢复
#define AH_SOIL_LOW_TRIG 30    // 土壤过干触发
#define AH_SOIL_LOW_REC  35    // 土壤恢复

/* 报警消息字符串映射 */
static const char* alarm_get_name(uint8_t type)
{
    switch (type) {
        case ALARM_TEMP_H: return "Temp^H";
        case ALARM_TEMP_L: return "Temp^L";
        case ALARM_SOIL_L: return "Soil^L";
        default:           return "";
    }
}

void Alarm_auto_ctrl(void)
{
    uint8_t new_alarm = 0;
    uint8_t new_type = ALARM_NONE;
    uint32_t new_val = 0;

    if (BEEP_AUTO_CTRL != AUTO_CTRL_ON) return;

    /* ---- 逐通道检查（带滞回，优先级递减）---- */

    /* 1. 温度过高 */
    if (alarm_type == ALARM_TEMP_H) {
        if (WD > AH_TH_HIGH_REC) { new_alarm = 1; new_type = ALARM_TEMP_H; new_val = WD; }
    } else {
        if (WD > AH_TH_HIGH_TRIG) { new_alarm = 1; new_type = ALARM_TEMP_H; new_val = WD; }
    }

    /* 2. 温度过低 */
    if (!new_alarm) {
        if (alarm_type == ALARM_TEMP_L) {
            if (WD < AH_TH_LOW_REC) { new_alarm = 1; new_type = ALARM_TEMP_L; new_val = WD; }
        } else {
            if (WD < AH_TH_LOW_TRIG) { new_alarm = 1; new_type = ALARM_TEMP_L; new_val = WD; }
        }
    }

    /* 3. 土壤过干 */
    if (!new_alarm) {
        if (alarm_type == ALARM_SOIL_L) {
            if (soil_moisture < AH_SOIL_LOW_REC) { new_alarm = 1; new_type = ALARM_SOIL_L; new_val = soil_moisture; }
        } else {
            if (soil_moisture < AH_SOIL_LOW_TRIG) { new_alarm = 1; new_type = ALARM_SOIL_L; new_val = soil_moisture; }
        }
    }

    /* ---- 状态变化：蓝牙通知 ---- */
    if (new_alarm != alarm_active)
    {
        if (new_alarm)
            printf("[ALARM] %s = %d\r\n", alarm_get_name(new_type), new_val);
        else
            printf("[ALARM] All clear\r\n");
    }

    /* 更新全局状态 */
    alarm_active = new_alarm;
    alarm_type   = new_type;
    alarm_value  = new_val;

    /* ---- 蜂鸣器：脉冲模式（2秒交替）---- */
    if (alarm_active)
    {
        alarm_pulse ^= 1;          // 每2s翻转一次
        beep_ctrl(alarm_pulse);
    }
    else
    {
        alarm_pulse = 0;
        beep_ctrl(0);
    }
}
