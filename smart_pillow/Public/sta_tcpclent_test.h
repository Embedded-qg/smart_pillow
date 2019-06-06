#ifndef _sta_tcpclent_test_H
#define _sta_tcpclent_test_H


#include "stm32f10x.h"


#define User_ESP8266_SSID	  "GDUT04"	      //要连接的热点的名称
#define User_ESP8266_PWD	  "linxu123456"	  //要连接的热点的密码


/*测试的服务器端口和IP*/
#define User_ESP8266_TCPServer_IP	     "192.168.31.248"	  //要连接的服务器的IP
#define User_ESP8266_TCPServer_PORT    "8090"	  //要连接的服务器的端口

void WIFI_ESP8266_Init(void);
void WIFI_ESP8266_Send_Data(char *content);


#endif
