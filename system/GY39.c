#include "GY39.h"
#include "uart.h"
#include "stdio.h"
#include "string.h"
#include "OLED.h"
#include "Delay.h"
#include "show.h"
#include "adc.h"
unsigned char gy39_cmd1[] = {0xA5,0x81,0x26};//光照强度
unsigned char gy39_cmd2[] = {0xA5,0x82,0x27};//其他
unsigned char gy39_mode=0;
unsigned char recvbuf[32];
unsigned char data_len;
unsigned char recv_ok;
extern uint16_t soil_moisture;
uint32_t Lux = 0;    // 光照强度，lux
uint32_t WD = 0;     // 温度，°C
uint32_t QY = 0;     // 气压，pa
uint32_t SD = 0;     // 湿度，%
uint32_t HB = 0;     // 海拔，m

void GY39_init(void)
{
	uart3_init(9600);
	gy39_start(GUANGZHAO);
}

void gy39_start(unsigned char mode)
{	
    gy39_mode = mode;
    data_len = 0;
    recv_ok = 0;
    
    if(gy39_mode == GUANGZHAO)
    {
        usart_sendString(USART3, gy39_cmd1, 3);
    }
    if(gy39_mode == OTHER)
    {
        usart_sendString(USART3, gy39_cmd2, 3);
    }
}

//超时判断
uint8_t gy39_send_cmd(unsigned char mode)
{
    uint32_t timeout = 0;
    gy39_mode = mode;
    data_len = 0;
    recv_ok = 0; 

    if(mode == GUANGZHAO)
        usart_sendString(USART3, gy39_cmd1, 3);
    else
        usart_sendString(USART3, gy39_cmd2, 3);

    while (recv_ok == 0 && timeout < 1000)
    {
        Delay_us(1);
        timeout++;
    }
    return recv_ok;  
}

void get_gy39_data(void)
{

    if(gy39_mode == GUANGZHAO)
    {
        Lux=(recvbuf[4]<<24)|(recvbuf[5]<<16)|(recvbuf[6]<<8)|recvbuf[7];
        Lux = Lux / 100;

        soil_moisture = Get_soil_moisture_percent();
        // 更新蓝牙发送缓存
        update_gy39_cache();
    }
    else if(gy39_mode == OTHER)
    {
        WD = recvbuf[4]<<8 | recvbuf[5];
        WD = WD/100;
        QY = recvbuf[6]<<24 | recvbuf[7]<<16 | recvbuf[8]<<8 | recvbuf[9];
        QY = QY/100;
        SD = recvbuf[10]<<8 | recvbuf[11];
        SD = SD/100;
        HB = recvbuf[12]<<8 | recvbuf[13];
        // 更新蓝牙发送缓存
        update_gy39_cache();
    }


}


