#include "blue.h"
#include "beep.h"
#include "led.h"
#include "string.h"
#include "uart.h"
#include "servo.h"
#include "stdio.h"
#include "show.h"
#include "GY39.h"
#include "Delay.h"
#include "OLED.h"
#include "FreeRTOS.h"
#include "Task.h"
#include "queue.h"


void bluetooth_init(void)
{
	uart2_init(9600);		
}

volatile uint8_t cmd_ready = 0; //命令接收完成标志位

static uint8_t get_cmd_code(const char *cmd) 
{		
		printf("\r\n");
		printf("____recvcmd: [%s]____\r\n",cmd);
	    if (strcmp(cmd, "BEEPON") == 0) return 1;		//打开蜂鸣器
	    if (strcmp(cmd, "BEEPOFF") == 0) return 2;	//关闭蜂鸣器
	    if (strcmp(cmd, "Servo+") == 0) return 3;		//舵机角度+
	    if (strcmp(cmd, "Servo-") == 0) return 4;		//舵机角度-
		if (strcmp(cmd, "SETWDLOW") == 0) return 5;	//温度调低
		if (strcmp(cmd, "SETWDHIGH") == 0) return 6;//温度调高
		if (strcmp(cmd, "FANSET") == 0) return 7;		//通风开关
		if (strcmp(cmd, "GY39") == 0) return 8;			//获取gy39数据
		if (strcmp(cmd, "RECVDATA") == 0) return 9;	//获取系统数据
		if (strcmp(cmd, "FANAUTOSET") == 0) return 10;//温度通风控制
		if (strcmp(cmd, "SETSDLOW") == 0) return 11;	//湿度调低
		if (strcmp(cmd, "SETSDHIGH") == 0) return 12;//湿度调高
		if (strcmp(cmd, "WPAUTOSET") == 0) return 13;//土壤湿度控制
		if (strcmp(cmd, "SETLIGHT") == 0) return 14;//太阳灯开关
		if (strcmp(cmd, "LIGHTAUTO") == 0) return 15;//太阳灯自动
		if (strcmp(cmd, "SETLUXLOW") == 0) return 16;	//光照调低
		if (strcmp(cmd, "SETLUXHIGH") == 0) return 17;//光照调高
		if (strcmp(cmd, "ALARMAUTO") == 0) return 18;//报警自动


    return 0; // 未知命令
}

extern float Angle;
int speed = 10;
extern uint8_t Fan_flag;
extern uint8_t WP_flag; 
extern uint8_t SUN_flag;

extern uint16_t soil_moisture;
extern unsigned char recv_ok;
extern uint32_t SET_WD;
extern uint32_t SET_SD;
extern uint32_t SET_LUX;

extern int FAN_AUTO_CTRL;
extern int WP_AUTO_CTRL;
extern int LED_AUTO_CTRL;
extern int BEEP_AUTO_CTRL;



void handle_bluetooth_cmd(uint8_t *rx_buf)
{
	  uint8_t cmd_code = get_cmd_code((char*)rx_buf);    
    // 使用switch处理不同命令
    switch (cmd_code) 
		{
			case 1: // BEEPON
					led_ctrl(LED0, LED_ON);
					beep_ctrl(BEEP_ON);
					break;
					
			case 2: // BEEPOFF
					led_ctrl(LED0, LED_OFF);
					beep_ctrl(BEEP_OFF);
					break;
					
					
			case 3: // Servo+
					Angle += 30;
					if (Angle > 180)
					{
							Angle = 0;
					}
					printf("[ Angle ] %.2f deg\r\n", Angle);
					Servo_SetAngle(Angle);
					break;

			case 4: // Servo-
				Angle -= 30;
				if (Angle < 0)
					{
						Angle = 180;
				}
				printf("[ Angle ] %.2f deg\r\n", Angle);
				Servo_SetAngle(Angle);
					break;
			case 5:
					SET_WD--;
					printf("[ Set_WD ] %d C\r\n", SET_WD);
					break;
			case 6:
					SET_WD++;
					printf("[ Set_WD ] %d C\r\n", SET_WD);
					break;
			case 7 :
				// 风扇手动切换（手动模式优先级最高）
				if(Fan_flag == 0)
				{
					Fan_SetSpeed(10000);
					Fan_flag = 1;
					Fan_SetManualMode();  // 手动开启，进入手动模式
					printf("[ Fan   ] OPEN (Manual)\r\n");
				}
				else
				{
					Fan_SetStop();
					Fan_flag = 0;
					Fan_SetManualMode();  // 手动关闭，也保持手动模式
					printf("[ Fan   ] CLOSE (Manual)\r\n");
				}
				break;
			case 8:
				printf("\r\n");
				printf("========== GY39 Data ==========\r\n");
				if (recv_ok == 1)
				{
						OLED_Clear();
						get_gy39_data();
						vTaskDelay(100);
						gy39_start(gy39_mode == GUANGZHAO ? OTHER : GUANGZHAO);
						printf("============================\r\n");
				}
				break;
		case 9:
				printf("\r\n");
				printf("========== System Status ==========\r\n");
				printf("[ Angle ] %7.2f deg\r\n", Angle);
				printf("[ WP     ] %s\r\n", WP_flag ? "OPEN" : "CLOSE");
				printf("[ Fan    ] %s\r\n", Fan_flag ? "OPEN" : "CLOSE");
				printf("[ Light  ] %s\r\n", SUN_flag ? "OPEN" : "CLOSE");
				printf("----------------------------------\r\n");
				printf("[ Fan_A  ] %s\r\n", FAN_AUTO_CTRL == AUTO_CTRL_ON ? "ON" : "OFF");
				printf("[ WP_A   ] %s\r\n", WP_AUTO_CTRL == AUTO_CTRL_ON ? "ON" : "OFF");
				printf("[ Lgt_A  ] %s\r\n", LED_AUTO_CTRL == AUTO_CTRL_ON ? "ON" : "OFF");
				printf("[ Alp_A  ] %s\r\n", BEEP_AUTO_CTRL == AUTO_CTRL_ON ? "ON" : "OFF");
				printf("----------------------------------\r\n");
				printf("[ Set_WD ] %d C\r\n", SET_WD);
				printf("[ Set_SD ] %d %%\r\n", SET_SD);
				printf("[ Set_Lx ] %d\r\n", SET_LUX);
				printf("==================================\r\n");
				break;
			case 10:
					FAN_AUTO_CTRL =( FAN_AUTO_CTRL == AUTO_CTRL_ON) ? AUTO_CTRL_OFF : AUTO_CTRL_ON;
					if(FAN_AUTO_CTRL == AUTO_CTRL_ON)
					{
						Fan_ClearManualMode();  // 开启自动控制时清除手动模式
						printf("[ Fan_A ] ON\r\n");
					}
					else
						printf("[ Fan_A ] OFF\r\n");
					break;
			case 11:
					SET_SD--;
					printf("[ Set_SD ] %d %%\r\n", SET_SD);
					break;
			case 12:
					SET_SD++;
					printf("[ Set_SD ] %d %%\r\n", SET_SD);
					break;
			case 13:
					WP_AUTO_CTRL =( WP_AUTO_CTRL == AUTO_CTRL_ON) ? AUTO_CTRL_OFF : AUTO_CTRL_ON;
					if(WP_AUTO_CTRL == AUTO_CTRL_ON)
						printf("[ WP_A  ] ON\r\n");
					else if(WP_AUTO_CTRL == AUTO_CTRL_OFF)
						printf("[ WP_A  ] OFF\r\n");
					break;
			case 14:
				// 补光灯手动切换（手动模式优先级最高）
				if(SUN_flag == 0)
				{
					led_ctrl(LED2, LED_ON);
					SUN_flag = 1;
					Light_SetManualMode();  // 开启补光，进入手动模式
					printf("[ Light ] OPEN (Manual)\r\n");
				}
				else
				{
					led_ctrl(LED2, LED_OFF);
					SUN_flag = 0;
					Light_SetManualMode();  // 关闭补光，也保持手动模式
					printf("[ Light ] CLOSE (Manual)\r\n");
				}
				break;
			case 15:
					LED_AUTO_CTRL =( LED_AUTO_CTRL == AUTO_CTRL_ON) ? AUTO_CTRL_OFF : AUTO_CTRL_ON;
					if(LED_AUTO_CTRL == AUTO_CTRL_ON)
					{
						Light_ClearManualMode();  // 开启自动控制时清除手动模式
						printf("[ Lgt_A ] ON\r\n");
					}
					else
						printf("[ Lgt_A ] OFF\r\n");
					break;
			case 16:
					SET_LUX-=100;
					printf("[ Set_Lx ] %d\r\n", SET_LUX);
					break;
			case 17:
					SET_LUX+=100;
					printf("[ Set_Lx ] %d\r\n", SET_LUX);
					break;
			case 18:
					BEEP_AUTO_CTRL =( BEEP_AUTO_CTRL == AUTO_CTRL_ON) ? AUTO_CTRL_OFF : AUTO_CTRL_ON;
					if(BEEP_AUTO_CTRL == AUTO_CTRL_ON)
						printf("[ Alp_A ] ON\r\n");
					else if(BEEP_AUTO_CTRL == AUTO_CTRL_OFF)
						printf("[ Alp_A ] OFF\r\n");
					break;	
			default :
				break;
		}	

}

extern volatile uint8_t cmd_ready; 
extern uint8_t rx_buf[50];
void process_bluetooth_cmd(void)
{
    if (cmd_ready)
    {
        __set_PRIMASK(1);  // 关闭中断，防止读取时被修改
        uint8_t temp_buf[50];
				memcpy(temp_buf, rx_buf, sizeof(temp_buf));  // 复制命令时保持不变
				handle_bluetooth_cmd(temp_buf);  // 处理命令
        __set_PRIMASK(0);  // 恢复中断
        
        cmd_ready = 0;      // 清除命令接收完成标志

    }
}
