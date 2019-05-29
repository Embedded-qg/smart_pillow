/**********************************************************
										 ��������					 
���ܣ�����AD7190������ͨ�����˲ɼ���Gain = 32������ = 93.75mV(�ο���ѹ3000mV,3000/32=93.75mv)
			��ʾ��λ��uV
�ӿڣ�
					AD7190ģ��		STM32�ܽ�
					DOUT					PC9
					SCK						PC11
					CS						PC12
					DIN						PC10
					GND						GND
ʱ�䣺2015/11/3
�汾��1.0
���ߣ���������
������

������������뵽�Ա��꣬�������ӽ߳�Ϊ������ ^_^
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
* �� �� ��         : main
* ��������		   : ������
* ��    ��         : ��
* ��    ��         : ��
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
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);  //�ж����ȼ����� ��2��
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
//					printf("����ģ�� ��0  ");
//					printf("����ģ�� ��0  ");
//					printf("\n");
//				}
//				else if(tmp4 <= 8)
//				{ 
//					printf("����ģ�� ��%ld  ",17-ad7190Result[0]%2);
//					printf("����ģ�� ��%ld ",62-tmp3-ad7190Result[0]%2-1);
//					printf("\n");
//				}
//				else
//				{ 
//					printf("����ģ�� ��%ld  ",17-ad7190Result[0]%4);
//					printf("����ģ�� ��%ld ",61-tmp3-ad7190Result[0]%4-1);
//					printf("\n");
//				}
//			}
//			else
//			{
//				printf("δ����\n");
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
