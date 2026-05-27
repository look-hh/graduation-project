#include "servo.h"
#include "PWM.h"
#include "led.h"

extern float Angle;
extern uint8_t Fan_flag;

void Servo_init(void)
{
	PWM_Init();
}



//设置舵机角度
void Servo_SetAngle(float Angle)
{	
//	Angle / 180表示角度值的一个比例，范围在 0-1 之间
//  2000表示舵机角度对应的脉冲宽度范围：2500-500=2000
//  500表示脉冲宽度的最小值，500微秒
	
	PWM_SetCompare3(Angle / 180 * 2000 + 500);	//设置占空比
												//将角度按比例转换为对应的脉冲宽度
	Angle = Angle;
											
}

//设置风扇速度
void Fan_SetSpeed(uint16_t Compare)
{
	PWM_Fan_SetCompare(Compare);
	led_ctrl(LED3, LED_ON);	
	Fan_flag = 1;
}

//关闭风扇
void Fan_SetStop(void)
{
	PWM_Fan_SetCompare(0);
	led_ctrl(LED3, LED_OFF);	
	Fan_flag = 0;
}

//设置风扇状态(开启/关闭)

void Fan_SetState(int Fan_state)
{
	if(Fan_state == FAN_ON)
	{
		Fan_SetSpeed(10000);
		Fan_flag = 1;
	}
	else if(Fan_state == FAN_OFF)
	{
		Fan_SetStop();
		Fan_flag = 0;

	}
	else if(Fan_state == FAN_TOGGLE)
	{
		if(Fan_flag == 0)
		{
			Fan_SetSpeed(10000);
			Fan_flag = 1;
		}
		else if(Fan_flag == 1)
		{
			Fan_SetStop();
			Fan_flag = 0;
		}
	}

}
