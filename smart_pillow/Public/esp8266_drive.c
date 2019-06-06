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
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 	 //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	 //IO���ٶ�Ϊ50MHz
	GPIO_Init(ESP8266_RST_Pin_Port, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = ESP8266_CH_PD_Pin;			   
	  GPIO_Init(ESP8266_CH_PD_Pin_Port, &GPIO_InitStructure);

	ESP8266_RST_Pin_SetH;
	ESP8266_CH_PD_Pin_SetL;	
}


//��ESP8266ģ�鷢��ATָ��
// cmd�������͵�ָ��
// ack1��ack2���ڴ�����Ӧ��ΪNULL������Ӧ������Ϊ���߼���ϵ
// time���ȴ���Ӧ��ʱ��
//����1�����ͳɹ� 0��ʧ��
bool ESP8266_Send_AT_Cmd(char *cmd,char *ack1,char *ack2,u32 time)
{
	ESP8266_Fram_Record_Struct .InfBit .FramLength = 0;	//���¿�ʼ�����µ����ݰ�
	ESP8266_USART("%s\r\n", cmd);
	if(ack1==0&&ack2==0)	 //����Ҫ��������
	{
		return true;
	}
	delay_ms(time);	  //��ʱtimeʱ��

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


//����WF-ESP8266ģ��
void ESP8266_Rst(void)
{
	ESP8266_RST_Pin_SetL;
	delay_ms(500); 
	ESP8266_RST_Pin_SetH;
}


//��ESP8266ģ�����AT��������
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


//ѡ��ESP8266ģ��Ĺ���ģʽ
// enumMode������ģʽ
//����1��ѡ��ɹ� 0��ѡ��ʧ��
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


//ESP8266ģ�������ⲿWiFi
//pSSID��WiFi�����ַ���
//pPassWord��WiFi�����ַ���
//����1�����ӳɹ� 0������ʧ��
bool ESP8266_JoinAP( char * pSSID, char * pPassWord )
{
	char cCmd [120];

	sprintf ( cCmd, "AT+CWJAP=\"%s\",\"%s\"", pSSID, pPassWord );
	
	return ESP8266_Send_AT_Cmd( cCmd, "OK", NULL, 5000 );
	
}

//ESP8266ģ�鴴��������
//pSSID��·����������
//pPassWord ��·����������
bool ESP8266_BuildAP( char * pSSID, char * pPassWord )
{
	char cCmd [140];

	sprintf ( cCmd, "AT+CWSAP=\"%s\",\"%s\",11,2", pSSID, pPassWord );
	
	return ESP8266_Send_AT_Cmd( cCmd, "OK", NULL, 50000 );
	
}

//��ģ�����óɷ����

void ESP8266_ServeAP()
{
	char cCmd [140];

	sprintf ( cCmd, "AT+CIPSERVER=1,8090");
	//�ɹ���Ӧ���ȡIP��ַ
	while(!ESP8266_Send_AT_Cmd( cCmd, "OK", NULL, 5000 ));
	//��ȡ�����������ĵ�ַ
	sprintf ( cCmd, "AT+CIFSR");
	ESP8266_Send_AT_Cmd( cCmd, "OK", NULL, 5000 );
}
//WIFIģ�������������������
//num ��ʾ�ͻ��� 0-5
//length ��ʾһ�η��͵����ݳ���
void ESP8266_Server_SendData(int8_t num,int32_t length)
{
	char cCmd [140];

	sprintf ( cCmd, "AT+CIPSEND=%d,%d",num,length);
	
	//�ɹ���Ӧ���ȡIP��ַ
	while(!ESP8266_Send_AT_Cmd( cCmd, "OK", NULL, 5000 ));
	
}

//ESP8266ģ������������
//enumEnUnvarnishTx�������Ƿ������
//����1�����óɹ� 0������ʧ��
bool ESP8266_Enable_MultipleId (FunctionalState enumEnUnvarnishTx )
{
	char cStr [20];
	
	sprintf ( cStr, "AT+CIPMUX=%d", ( enumEnUnvarnishTx ? 1 : 0 ) );
	
	return ESP8266_Send_AT_Cmd ( cStr, "OK", 0, 500 );
	
}


//ESP8266ģ�������ⲿ������
//enumE������Э��
//ip��������IP�ַ���
//ComNum���������˿��ַ���
//id��ģ�����ӷ�������ID
//����1�����ӳɹ� 0������ʧ��
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


//����ESP8266ģ�����͸������
//����1�����óɹ� 0������ʧ��
bool ESP8266_UnvarnishSend ( void )
{
	if (!ESP8266_Send_AT_Cmd ( "AT+CIPMODE=1", "OK", 0, 500 ))
		return false;
	
	return 
	  	ESP8266_Send_AT_Cmd( "AT+CIPSEND", "OK", ">", 500 );
	
}

/*
  ESP8266ģ�鷢���ַ���
  enumEnUnvarnishTx�������Ƿ���ʹ����͸��ģʽ
  pStr��Ҫ���͵��ַ���
  ulStrLength��Ҫ���͵��ַ������ֽ���
  ucId���ĸ�ID���͵��ַ���
  ����1�����ͳɹ�   0������ʧ��
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


//ESP8266ģ���˳�͸��ģʽ
void ESP8266_ExitUnvarnishSend ( void )
{
	delay_ms(1000);
	ESP8266_USART( "+++" );
	delay_ms( 500 ); 	
}


//ESP8266 ������״̬�����ʺϵ��˿�ʱʹ��
//����0����ȡ״̬ʧ��
//����2�����ip
//����3���������� 
//����4��ʧȥ���� 
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

