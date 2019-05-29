/**********************************************************
										 康威电子					 
功能：利用AD7190进行四通道单端采集，Gain = 32，量程 = 93.75mV(参考电压3000mV,3000/32=93.75mv)
			显示单位：uV
接口：
					AD7190模块		STM32管脚
					DOUT					PC9
					SCK						PC11
					CS						PC12
					DIN						PC10
					GND						GND
时间：2015/11/3
版本：1.0
作者：康威电子
其他：

更多电子需求，请到淘宝店，康威电子竭诚为您服务 ^_^
https://kvdz.taobao.com/

**********************************************************/

#include "stm32f10x.h"
#include "led.h"
#include "system.h"
#include "SysTick.h"
#include "usart.h"
#include "ad7190.h"
#include "delay.h"
#include "sys.h"
#include <stdlib.h>
#include <time.h>

long int ad7190Result[4];

/*******************************************************************************
* 函 数 名         : main
* 函数功能		   : 主函数
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/

int main()
{		
	int tmp3 = 0;
	int tmp4 = 0;
	int i = 0;
	unsigned char tmp = 0x50;
  unsigned char tmp2[3];
	SystemInit();
	delay_init(72);
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);  //中断优先级分组 分2组
	NVIC_Configuration();
	USART1_Init(9600);
	ad7190_Init();
	delay_us(20);
	ad7190_Init();
	delay_us(20);
//	while(1)
//	{
//		printf("1\n");
//		printf("1\n");
//		printf("1\n");
//		printf("1\n");
//	}
//	while(1)
//	{
//		printf("%d \n",PCin(9));
//////	}
//	while(1)
//	{
//		WriteToAD7190(1,&tmp);
//		delay_us(1);
//		ReadFromAD7190(3,tmp2);
//		printf("%x ", tmp2[0]);
//		printf("%x ", tmp2[1]);
//		printf("%x \n", tmp2[2]);
//		delay_us(20);
//		ad7190_Init();
//		delay_us(20);
//	}
//		
		

	
	while(1)
	{
		int time = 1000;

		//AveWeightDataGet();
		AdcGroupRead(ad7190Result);
		{
			int sleepflag = 0;
			int tmp = ad7190Result[0]%5;
			srand(PCin(10));
			for(i = 0; i < 4; i++)
				ad7190Result[i] = ((double)ad7190Result[i])/1/16777216*3000000;
			if(ad7190Result[0]>60000)
				sleepflag = 1;
//			if(sleepflag == 1) 
//			{
//				tmp4++;
//				if(tmp4<=4)
//				{
//					printf("呼吸模拟 ：0  ");
//					printf("心率模拟 ：0  ");
//					printf("\n");
//				}
//				else if(tmp4 <= 8)
//				{ 
//					printf("呼吸模拟 ：%ld  ",17-ad7190Result[0]%2);
//					printf("心率模拟 ：%ld ",62-tmp3-ad7190Result[0]%2-1);
//					printf("\n");
//				}
//				else
//				{ 
//					printf("呼吸模拟 ：%ld  ",17-ad7190Result[0]%4);
//					printf("心率模拟 ：%ld ",61-tmp3-ad7190Result[0]%4-1);
//					printf("\n");
//				}
//			}
//			else
//			{
//				printf("未入枕\n");
//				tmp4=0;
//			}
			printf("%8ld\n",ad7190Result[0]);
			delay_us(1000);
//			printf("CH1:%8lduV  ",ad7190Result[1]);
//			printf("CH2:%8lduV  ",ad7190Result[2]);
//			printf("CH3:%8lduV  ",ad7190Result[3]);
//			printf("123\n");
//		printf("\n");
//			printf("CH0:%ld\tCH1:%ld\tCH2:%ld\tCH3:%ld\t\r\n",ad7190Result[0], ad7190Result[1], ad7190Result[2], ad7190Result[3]);
			tmp3 = ad7190Result[0]%5;
		}
	}
}
