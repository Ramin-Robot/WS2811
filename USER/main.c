#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "WS2811.h"

int main(void)
{ 
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//����ϵͳ�ж����ȼ�����2
	delay_init(168);  //��ʼ����ʱ����

	WS2811_Init();    //�ƹܳ�ʼ��
	while(1)
	{
			RGB_Lighting();
	}
}
