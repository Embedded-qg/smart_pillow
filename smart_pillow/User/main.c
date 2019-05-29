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
#include "bsp_TiMbase.h" 

#define a 0.01

long int ad7190Result[4];
long int data[1010];
long int dataY[1010];
int i = 0;
volatile u32 time = 0;
/*******************************************************************************
* �� �� ��         : main
* ��������		   : ������
* ��    ��         : ��
* ��    ��         : ��
*******************************************************************************/

void Low_pass_filter(long int *data, long int *dataY, int n)
{
	int y;
	int x;
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
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2 , ENABLE);
	while(1)
	{
		int n=0;
		while(time<10000)
		{
			AdcGroupRead(ad7190Result);
			ad7190Result[0]=((double)ad7190Result[0])/1/16777216*3000000;
			data[n++]=ad7190Result[0];
			//printf("%ld\n", data[n-1]);
		}
//		for(i=0; i<n; i++)
//			printf("%ld ",data[i]);
		Low_pass_filter(data, dataY, n);
		printf("\n%d\n",Peak_num(dataY, n));
		printf("\n%d\n",n);
		n=0;
		time=0;
	}
}
