#include "esp8266_drive.h"
#include "SysTick.h"
#include <stdio.h>  
#include <string.h>  
#include <stdbool.h>
#include "sta_tcpclent_test.h"
#include "esp8266_public.h"

struct STRUCT_USART_Fram ESP8266_Fram_Record_Struct = { 0 };

void ESP8266_Init()
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(ESP8266_RST_Pin_Periph_Clock|ESP8266_CH_PD_Pin_Periph_Clock, ENABLE);

	GPIO_InitStructure.GPIO_Pin = ESP8266_RST_Pin;			   
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 	 //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	 //IO口速度为50MHz
	GPIO_Init(ESP8266_RST_Pin_Port, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = ESP8266_CH_PD_Pin;			   
	  GPIO_Init(ESP8266_CH_PD_Pin_Port, &GPIO_InitStructure);

	ESP8266_RST_Pin_SetH;
	ESP8266_CH_PD_Pin_SetL;	
}


//对ESP8266模块发送AT指令
// cmd：待发送的指令
// ack1，ack2：期待的响应，为NULL表不需响应，两者为或逻辑关系
// time：等待响应的时间
//返回1：发送成功 0：失败
bool ESP8266_Send_AT_Cmd(char *cmd,char *ack1,char *ack2,u32 time)
{
	ESP8266_Fram_Record_Struct .InfBit .FramLength = 0;	//从新开始接收新的数据包
	ESP8266_USART("%s\r\n", cmd);
	if(ack1==0&&ack2==0)	 //不需要接收数据
	{
		return true;
	}
	delay_ms(time);	  //延时time时间

	ESP8266_Fram_Record_Struct.Data_RX_BUF[ESP8266_Fram_Record_Struct.InfBit.FramLength ] = '\0';

	PC_USART("%s", ESP8266_Fram_Record_Struct .Data_RX_BUF);

	if(ack1!=0&&ack2!=0)
	{
		return ( ( bool ) strstr ( ESP8266_Fram_Record_Struct .Data_RX_BUF, ack1 ) || 
						 ( bool ) strstr ( ESP8266_Fram_Record_Struct .Data_RX_BUF, ack2 ) );
	}
	else if( ack1 != 0 )
		return ( ( bool ) strstr ( ESP8266_Fram_Record_Struct .Data_RX_BUF, ack1 ) );
	
	else
		return ( ( bool ) strstr ( ESP8266_Fram_Record_Struct .Data_RX_BUF, ack2 ) );
			
}


//重启WF-ESP8266模块
void ESP8266_Rst(void)
{
	ESP8266_RST_Pin_SetL;
	delay_ms(500); 
	ESP8266_RST_Pin_SetH;
}


//对ESP8266模块进行AT测试启动
void ESP8266_AT_Test(void)
{
	char count=0;
	
	ESP8266_RST_Pin_SetH;
	delay_ms(1000);	
	while(count < 10)
	{
		if(ESP8266_Send_AT_Cmd("AT","OK",NULL,500)) 
			return;
		ESP8266_Rst();
		++ count;
	}
}


//选择ESP8266模块的工作模式
// enumMode：工作模式
//返回1：选择成功 0：选择失败
bool ESP8266_Net_Mode_Choose(ENUM_Net_ModeTypeDef enumMode)
{
	switch ( enumMode )
	{
		case STA:
			return ESP8266_Send_AT_Cmd ( "AT+CWMODE=1", "OK", "no change", 2500 ); 
		
	  	case AP:
		  	return ESP8266_Send_AT_Cmd ( "AT+CWMODE=2", "OK", "no change", 2500 ); 
		
		case STA_AP:
		  	return ESP8266_Send_AT_Cmd ( "AT+CWMODE=3", "OK", "no change", 2500 ); 
		
	  	default:
		  return false;
	}		
}


//ESP8266模块连接外部WiFi
//pSSID：WiFi名称字符串
//pPassWord：WiFi密码字符串
//返回1：连接成功 0：连接失败
bool ESP8266_JoinAP( char * pSSID, char * pPassWord )
{
	char cCmd [120];

	sprintf ( cCmd, "AT+CWJAP=\"%s\",\"%s\"", pSSID, pPassWord );
	
	return ESP8266_Send_AT_Cmd( cCmd, "OK", NULL, 5000 );
	
}

//ESP8266模块创建局域网
//pSSID是路由器的名字
//pPassWord 是路由器的密码
bool ESP8266_BuildAP( char * pSSID, char * pPassWord )
{
	char cCmd [140];

	sprintf ( cCmd, "AT+CWSAP=\"%s\",\"%s\",11,2", pSSID, pPassWord );
	
	return ESP8266_Send_AT_Cmd( cCmd, "OK", NULL, 50000 );
	
}

//将模块配置成服务端

void ESP8266_ServeAP()
{
	char cCmd [140];

	sprintf ( cCmd, "AT+CIPSERVER=1,8090");
	//成功响应后获取IP地址
	while(!ESP8266_Send_AT_Cmd( cCmd, "OK", NULL, 5000 ));
	//获取服务器本机的地址
	sprintf ( cCmd, "AT+CIFSR");
	ESP8266_Send_AT_Cmd( cCmd, "OK", NULL, 5000 );
}
//WIFI模块做服务机，发送数据
//num 表示客户机 0-5
//length 表示一次发送的数据长度
void ESP8266_Server_SendData(int8_t num,int32_t length)
{
	char cCmd [140];

	sprintf ( cCmd, "AT+CIPSEND=%d,%d",num,length);
	
	//成功响应后获取IP地址
	while(!ESP8266_Send_AT_Cmd( cCmd, "OK", NULL, 5000 ));
	
}

//ESP8266模块启动多连接
//enumEnUnvarnishTx：配置是否多连接
//返回1：配置成功 0：配置失败
bool ESP8266_Enable_MultipleId (FunctionalState enumEnUnvarnishTx )
{
	char cStr [20];
	
	sprintf ( cStr, "AT+CIPMUX=%d", ( enumEnUnvarnishTx ? 1 : 0 ) );
	
	return ESP8266_Send_AT_Cmd ( cStr, "OK", 0, 500 );
	
}


//ESP8266模块连接外部服务器
//enumE：网络协议
//ip：服务器IP字符串
//ComNum：服务器端口字符串
//id：模块连接服务器的ID
//返回1：连接成功 0：连接失败
bool ESP8266_Link_Server(ENUM_NetPro_TypeDef enumE, char * ip, char * ComNum, ENUM_ID_NO_TypeDef id)
{
	char cStr [100] = { 0 }, cCmd [120];

  	switch (  enumE )
  	{
		case enumTCP:
		  sprintf ( cStr, "\"%s\",\"%s\",%s", "TCP", ip, ComNum );
		  break;
		
		case enumUDP:
		  sprintf ( cStr, "\"%s\",\"%s\",%s", "UDP", ip, ComNum );
		  break;
		
		default:
			break;
  	}

  	if ( id < 5 )
    	sprintf ( cCmd, "AT+CIPSTART=%d,%s", id, cStr);

  	else
	  	sprintf ( cCmd, "AT+CIPSTART=%s", cStr );

	return ESP8266_Send_AT_Cmd ( cCmd, "OK", "ALREAY CONNECT", 4000 );
	
}


//配置ESP8266模块进入透传发送
//返回1：配置成功 0：配置失败
bool ESP8266_UnvarnishSend ( void )
{
	if (!ESP8266_Send_AT_Cmd ( "AT+CIPMODE=1", "OK", 0, 500 ))
		return false;
	
	return 
	  	ESP8266_Send_AT_Cmd( "AT+CIPSEND", "OK", ">", 500 );
	
}

/*
  ESP8266模块发送字符串
  enumEnUnvarnishTx：声明是否已使能了透传模式
  pStr：要发送的字符串
  ulStrLength：要发送的字符串的字节数
  ucId：哪个ID发送的字符串
  返回1：发送成功   0：发送失败
*/
bool ESP8266_SendString(FunctionalState enumEnUnvarnishTx, char * pStr, u32 ulStrLength, ENUM_ID_NO_TypeDef ucId )
{
	char cStr [20];
	bool bRet = false;
		
	if ( enumEnUnvarnishTx )
	{
		ESP8266_USART ( "%s", pStr );
		
		bRet = true;	
	}

	else
	{
		if ( ucId < 5 )
			sprintf ( cStr, "AT+CIPSEND=%d,%d", ucId, ulStrLength + 2 );

		else
			sprintf ( cStr, "AT+CIPSEND=%d", ulStrLength + 2 );
		
		ESP8266_Send_AT_Cmd ( cStr, "> ", 0, 1000 );

		bRet = ESP8266_Send_AT_Cmd ( pStr, "SEND OK", 0, 1000 );
  }
	
	return bRet;

}


//ESP8266模块退出透传模式
void ESP8266_ExitUnvarnishSend ( void )
{
	delay_ms(1000);
	ESP8266_USART( "+++" );
	delay_ms( 500 ); 	
}


//ESP8266 的连接状态，较适合单端口时使用
//返回0：获取状态失败
//返回2：获得ip
//返回3：建立连接 
//返回4：失去连接 
u8 ESP8266_Get_LinkStatus ( void )
{
	if (ESP8266_Send_AT_Cmd( "AT+CIPSTATUS", "OK", 0, 500 ) )
	{
		if ( strstr ( ESP8266_Fram_Record_Struct .Data_RX_BUF, "STATUS:2\r\n" ) )
			return 2;
		
		else if ( strstr ( ESP8266_Fram_Record_Struct .Data_RX_BUF, "STATUS:3\r\n" ) )
			return 3;
		
		else if ( strstr ( ESP8266_Fram_Record_Struct .Data_RX_BUF, "STATUS:4\r\n" ) )
			return 4;		

	}
	
	return 0;
	
}

