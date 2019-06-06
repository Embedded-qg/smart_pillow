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
	
	printf ( "\r\n��������ESP8266�����ĵȴ�...\r\n" );

	ESP8266_CH_PD_Pin_SetH;
	
	
	ESP8266_AT_Test();
	printf("AT�ɹ�\r\n");
	
	//WIFIģ�����ͻ���
	ESP8266_Net_Mode_Choose(STA);
	while(!ESP8266_JoinAP(User_ESP8266_SSID, User_ESP8266_PWD));
	ESP8266_Enable_MultipleId ( DISABLE );
	while(!ESP8266_Link_Server(enumTCP, User_ESP8266_TCPServer_IP, User_ESP8266_TCPServer_PORT, Single_ID_0));

	while(!ESP8266_UnvarnishSend());  	//����͸��ģʽ
		
}


void WIFI_ESP8266_Send_Data(char *content)
{
	ESP8266_SendString ( ENABLE, content, 0, Single_ID_0 );	   //��������	
	send_data_time++;                                          //ÿ����һ�Σ����ͼ�����һ
	
	
	//ÿ�η������ݶ�����յ���̨�������ַ�r��ΪӦ��������յ�r��send_data_time����Ϊ0
	//ÿ�����ݷ��ͳɹ�send_data_time+1���������6�ζ�û���ܵ��ַ�r��˵���Ͽ����ӣ���Ҫ��������
	if(send_data_time >= 20)
	{
		send_data_time = 0;
		printf("\r\n����20��û���ܵ�Ӧ��\r\n");
		TcpCheeekFlag = 1;
	}
	
	
	if(TcpCheeekFlag) //�������6�ζ�û���ܵ��ַ�r��˵���Ͽ����ӣ���Ҫ��������
	{
		delay_ms(500);

		TcpCheeekFlag = 0;   		     //��ɺ���Ϊ0
		ESP8266_ExitUnvarnishSend(); //�˳�͸��ģʽ(���˳�͸��ģʽ���ܼ��)
		
		Link_Tcp_Again:
		
		printf ( "\r\n���Եȣ����������ȵ�ͷ�����...\r\n" );
		
		while (!ESP8266_JoinAP(User_ESP8266_SSID, User_ESP8266_PWD ) );
		
		while (!ESP8266_Link_Server(enumTCP, User_ESP8266_TCPServer_IP, User_ESP8266_TCPServer_PORT, Single_ID_0 ) );
		
		printf ( "\r\n�����ȵ�ͷ������ɹ�\r\n" );
		delay_ms(100);
		if(ESP8266_UnvarnishSend()!=true||TcpCloseFlag==1)
		{
			printf("\r\nTCPCloseFlagΪ1\r\n");
			TcpCloseFlag = 0;
			goto Link_Tcp_Again;
		}
		printf ( "\r\n����͸���ɹ�\r\n" );
		
	}
	
}	

/*

**********wifiģ����������**********
UTXD  :   PD2
URXD  :   PC12
VCC   :   3.3V
GND   :   GND
CH_PD :   PG7
GOIO16:   PE6
************************************

************������������************
UTXD1  :   PA9
URXD1  :   PA10
************************************

*/


