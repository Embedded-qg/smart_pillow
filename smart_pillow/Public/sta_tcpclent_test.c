#include "sta_tcpclent_test.h"
#include "SysTick.h"
#include "usart.h"
#include "esp8266_drive.h"


int         TcpCloseFlag = 0;
volatile u8 TcpCheeekFlag ;
uint32_t    send_data_time;


void WIFI_ESP8266_Init()
{
	
	ESP8266_Init();
	
	printf ( "\r\n正在配置ESP8266请耐心等待...\r\n" );

	ESP8266_CH_PD_Pin_SetH;
	
	
	ESP8266_AT_Test();
	printf("AT成功\r\n");
	
	//WIFI模块做客户端
	ESP8266_Net_Mode_Choose(STA);
	while(!ESP8266_JoinAP(User_ESP8266_SSID, User_ESP8266_PWD));
	ESP8266_Enable_MultipleId ( DISABLE );
	while(!ESP8266_Link_Server(enumTCP, User_ESP8266_TCPServer_IP, User_ESP8266_TCPServer_PORT, Single_ID_0));

	while(!ESP8266_UnvarnishSend());  	//进入透传模式
		
}


void WIFI_ESP8266_Send_Data(char *content)
{
	ESP8266_SendString ( ENABLE, content, 0, Single_ID_0 );	   //发送数据	
	send_data_time++;                                          //每发送一次，发送计数加一
	
	
	//每次发送数据都会接收到后台发来的字符r作为应答，如果接收到r，send_data_time被置为0
	//每次数据发送成功send_data_time+1，如果超过6次都没接受到字符r，说明断开连接，需要重新连接
	if(send_data_time >= 20)
	{
		send_data_time = 0;
		printf("\r\n超过20次没接受到应答\r\n");
		TcpCheeekFlag = 1;
	}
	
	
	if(TcpCheeekFlag) //如果超过6次都没接受到字符r，说明断开连接，需要重新连接
	{
		delay_ms(500);

		TcpCheeekFlag = 0;   		     //完成后置为0
		ESP8266_ExitUnvarnishSend(); //退出透传模式(先退出透传模式才能检查)
		
		Link_Tcp_Again:
		
		printf ( "\r\n请稍等，正在重连热点和服务器...\r\n" );
		
		while (!ESP8266_JoinAP(User_ESP8266_SSID, User_ESP8266_PWD ) );
		
		while (!ESP8266_Link_Server(enumTCP, User_ESP8266_TCPServer_IP, User_ESP8266_TCPServer_PORT, Single_ID_0 ) );
		
		printf ( "\r\n重连热点和服务器成功\r\n" );
		delay_ms(100);
		if(ESP8266_UnvarnishSend()!=true||TcpCloseFlag==1)
		{
			printf("\r\nTCPCloseFlag为1\r\n");
			TcpCloseFlag = 0;
			goto Link_Tcp_Again;
		}
		printf ( "\r\n配置透传成功\r\n" );
		
	}
	
}	

/*

**********wifi模块引脚连接**********
UTXD  :   PD2
URXD  :   PC12
VCC   :   3.3V
GND   :   GND
CH_PD :   PG7
GOIO16:   PE6
************************************

************串口引脚连接************
UTXD1  :   PA9
URXD1  :   PA10
************************************

*/


