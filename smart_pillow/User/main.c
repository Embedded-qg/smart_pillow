/**********************************************************					 
功能：利用AD7190进行单通道全查分采集
接口：
					AD7190模块		STM32管脚
					DOUT					PC9
					CS						PC13
					CLK						PC11
					DIN						PC10
					GND						GND
					5V						5V
时间：2019/5/28
作者：川十
版本：1.0

**********************************************************/

#include "stm32f10x.h"
#include "led.h"
#include "system.h"
#include "usart.h"
#include "ad7190.h"
#include "delay.h"
#include "sys.h"
#include <stdlib.h>
#include "bsp_TiMbase.h" 
#include "bsp_adc.h"
#include <math.h>
#include "esp8266_drive.h"
#include "sta_tcpclent_test.h"

#define BREATH_PARA 0.01
#define HEARTBEAT_PARA 0.3

long int ad7190Result[4];
long int data[1010];
long int dataY[1010];
int i = 0;
volatile u32 time = 0;

extern __IO uint16_t ADC_ConvertedValue;
float ADC_ConvertedValueLocal;        

/*******************************************************************************
* 函 数 名         : main
* 函数功能		   : 主函数
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/

//低通滤波算法
void Low_pass_filter(long int *data, long int *dataY, int n, double a)
{
	int y;
	y=data[0];
	for(i=0; i<n; i++)
	{
			dataY[i]=a*data[i]+(1-a)*y;
			y=dataY[i];
	}
}

int Peak_num(long int *data, int n)
{
	int peakNum=0;
	int nextData=0;
	int flag=0;
	int lastData=data[0];
	for(i=0; i<n; i++)
	{
			nextData=data[i+1];
			if(nextData>lastData)
					flag=1;
			if(nextData<lastData&&flag==1)
			{
					peakNum++;
					flag=0;
					//printf("%d ",i);
			}
			lastData=nextData;

	}
	return peakNum;
}

int Beat_num(long int *data, int num)
{
	int n=0;
	int beatNum=0;
	for(i=1; i<num-1; i++)
  {
		if(n<0) n=0;
		if(abs(data[i]-data[i-1]+data[i]-data[i+1])>60 && n==0)
		{
			if(abs(data[i]-data[i-1])+50 >20)
				beatNum++;
			n=3;
		}
		n--;
	}
	return beatNum;
}


int main()
{		
	char str[400]="{\"blueteeth\":1,\"wifi\": 1,\"rateList\":[{\"deviceId\":1,\"time\":\"2017-01-01 20:20:20\",\"signal\":0,\"heartAvg\":32,\"breathAvg\":32,\"envAvg\":32},{\"deviceId\":1,\"time\":\"2017-01-01 20:20:20\",\"signal\":0,\"heartAvg\":32,\"breathAvg\":32,\"envAvg\":32}]}";
	SystemInit();
	delay_init(72);
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);  //中断优先级分组 分2组
	NVIC_Configuration();
	USART1_Init(115200);
	ADC1_Init();
	ad7190_Init();
	delay_us(20);
	ad7190_Init();
	delay_us(20);
	TIM2_Configuration();
	TIM2_NVIC_Configuration();
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2 , ENABLE);
	UART5_Init (115200);
	WIFI_ESP8266_Init();
	printf("HelloWorld!\n");
	
	
//	while(1)
//	{
		//delay_ms(10);
		//ADC_ConvertedValueLocal =abs((float) ADC_ConvertedValue);
   	//printf("%8f\r\n",ADC_ConvertedValueLocal); 
//		int breath_num;
//		int heartbeat_num;
//		int n=0;
		//while(time<1000)
		//{
			//AdcGroupRead(ad7190Result);
			//ad7190Result[0]=((double)ad7190Result[0])/1/16777216*3000000;
			//printf("%ld\n",ad7190Result[0]);
//			data[n++]=ad7190Result[0];
//			//printf("%ld\n", data[n-1]);
		//}
////		for(i=0; i<n; i++)
////			printf("%ld ",data[i]);
//		Low_pass_filter(data, dataY, n, BREATH_PARA);
//		breath_num=Peak_num(dataY, n);
//		Low_pass_filter(data, dataY, n, HEARTBEAT_PARA);
//		heartbeat_num=Beat_num(dataY, n);
//		printf("%d ",breath_num);
//		printf("%d ",heartbeat_num);
//		printf("\n%d\n",n);
//		n=0;
		//time=0;
		delay_ms(5000);
//		
    WIFI_ESP8266_Send_Data(str);    //运用该函数，输入字符串即可发送字符串到指定服务器
//		
	//}
}
