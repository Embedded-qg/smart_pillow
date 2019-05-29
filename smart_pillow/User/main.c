/**********************************************************					 
���ܣ�����AD7190���е�ͨ��ȫ��ֲɼ�
�ӿڣ�
					AD7190ģ��		STM32�ܽ�
					DOUT					PC9
					CS						PC12
					CLK						PC11
					DIN						PC10
					GND						GND
					5V						5V
ʱ�䣺2019/5/28
���ߣ���ʮ
�汾��1.0

**********************************************************/

#include "stm32f10x.h"
#include "led.h"
#include "system.h"
#include "usart.h"
#include "ad7190.h"
#include "delay.h"
#include "sys.h"
#include <stdlib.h>
#include <time.h>
#include "bsp_TiMbase.h" 

#define a 0.001
#define MON_CYCLE 100
long int ad7190Result[4];
long int data[1010];
long int dataY[1010];
int i = 0;

/*******************************************************************************
* �� �� ��         : main
* ��������		   : ������
* ��    ��         : ��
* ��    ��         : ��
*******************************************************************************/

void Low_pass_filter(long int *data, long int *dataY)
{
	int y;
	int x;
	y=data[0];
	for(i=0; i<MON_CYCLE; i++)
	{
			dataY[i]=a*data[i]+(1-a)*y;
			y=dataY[i];
	}
}

int Peak_num(long int *data)
{
	int peakNum=0;
	int nextData=0;
	int flag=0;
	int lastData=data[0];
	for(i=0; i<MON_CYCLE; i++)
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

int main()
{		
	

	
	
	int tmp3 = 0;
	int tmp4 = 0;
	unsigned char tmp = 0x50;
  unsigned char tmp2[3];
	SystemInit();
	delay_init(72);
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);  //�ж����ȼ����� ��2��
	NVIC_Configuration();
	USART1_Init(115200);
	ad7190_Init();
	delay_us(20);
	ad7190_Init();
	delay_us(20);
	TIM2_Configuration();
	TIM2_NVIC_Configuration();
	while(1)
	{
		for(i=0; i<MON_CYCLE; i++)
		{
			AdcGroupRead(ad7190Result);
			ad7190Result[0]=((double)ad7190Result[0])/1/16777216*3000000;
			data[i]=ad7190Result[0];
			//printf("%ld\n", ad7190Result[0]);
		}
//		for(i=0; i<MON_CYCLE; i++)
//		{
//			AdcGroupRead(ad7190Result);
//			data[i] = ((double)ad7190Result[i])/1/16777216*3000000;
//			
//		}
		Low_pass_filter(data, dataY);
		printf("%d\n",Peak_num(dataY));
	}
}
