#ifndef _sta_tcpclent_test_H
#define _sta_tcpclent_test_H


#include "stm32f10x.h"


#define User_ESP8266_SSID	  "GDUT04"	      //Ҫ���ӵ��ȵ������
#define User_ESP8266_PWD	  "linxu123456"	  //Ҫ���ӵ��ȵ������


/*���Եķ������˿ں�IP*/
#define User_ESP8266_TCPServer_IP	     "192.168.31.248"	  //Ҫ���ӵķ�������IP
#define User_ESP8266_TCPServer_PORT    "8090"	  //Ҫ���ӵķ������Ķ˿�

void WIFI_ESP8266_Init(void);
void WIFI_ESP8266_Send_Data(char *content);


#endif
