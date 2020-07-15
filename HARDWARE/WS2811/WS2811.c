#include "WS2811.h"

static void delay(unsigned long int us);

static u8 Gard[3]={0};  // BLUE RED GREEN
static u16 TIM3_count=0;
static u8 Light_blue=0;
static u8 Light_red=0;
static u8 Light_green=0;

//==================================WS2811��ʼ��======================================//
void WS2811_Init(void)
{
		GPIO_InitTypeDef  GPIO_InitStructure;

		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);//ʹ��GPIOAʱ��

		//GPIOA0��ʼ������
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//��ͨ���ģʽ
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�������
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;//������
		GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��
	
    Reset();
	
		TIM3_Int_Init(1000,84);                   //TIM3 1KHZ
}
//==================================ͨ�ö�ʱ��3��ʼ��================================//
//arr���Զ���װֵ�psc��ʱ��Ԥ��Ƶ��     ��ʱ�����ʱ����㷽��:Tout=((arr+1)*(psc+1))/Ft us.
void TIM3_Int_Init(u16 arr,u16 psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);  ///ʹ��TIM3ʱ��
	
  TIM_TimeBaseInitStructure.TIM_Period = arr; 	//�Զ���װ��ֵ
	TIM_TimeBaseInitStructure.TIM_Prescaler=psc;  //��ʱ����Ƶ
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; //���ϼ���ģʽ
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
	
	TIM_TimeBaseInit(TIM3,&TIM_TimeBaseInitStructure);//��ʼ��TIM3
	
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE); //����ʱ��3�����ж�
	TIM_Cmd(TIM3,ENABLE); //ʹ�ܶ�ʱ��3
	
	NVIC_InitStructure.NVIC_IRQChannel=TIM3_IRQn; //��ʱ��3�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x01; //��ռ���ȼ�1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x03; //�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
}

//��ʱ��3�жϷ�����
void TIM3_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM3,TIM_IT_Update)==SET){ //����ж�
			TIM3_count++;
			if((TIM3_count%(LIGHT_TIME*3))==0){
					Light_blue=1;
					Light_red=0;
					Light_green=0;
			}
			else if((TIM3_count%(LIGHT_TIME*3))==(LIGHT_TIME*1)){
					Light_blue=0;
					Light_red=1;
					Light_green=0;
			}
			else if((TIM3_count%(LIGHT_TIME*3))==(LIGHT_TIME*2)){
					Light_blue=0;
					Light_red=0;
					Light_green=1;
			}
			else if(TIM3_count>(LIGHT_TIME*3))
				TIM3_count=0;
	}
	TIM_ClearITPendingBit(TIM3,TIM_IT_Update);  //����жϱ�־λ
}

//===============���ٱ���ģʽBIT0(�ߵ�ƽʱ��:0.5us �͵�ƽʱ��:2us )===============//
void l_dat0(void)
{
		WS2811_CONTROL|=HIGH;
		delay(3);
		WS2811_CONTROL&=~HIGH;
		delay(15);
}

//===============���ٱ���ģʽBIT1(�ߵ�ƽʱ��:5us �͵�ƽʱ��:0.5us)================//
void l_dat1(void)
{
		WS2811_CONTROL|=HIGH;
		delay(16);
		WS2811_CONTROL&=~HIGH;
		delay(2);
}

//===================================RGB��λ====================================//
void Reset(void)
{
		WS2811_CONTROL&=~HIGH;
		delay(2000);
}

//============================����RGB�Ҷ�����===================================//
void send_single_data(const u8 *data) 											//���ݸ�ʽ:R7~R0~G7~G0~B7~B0
{
		s8 i=0,j=0;
		for(i = 0; i < 3; i++){
				for(j = 7; j > -1; j--){
						if((*(data+i)&(HIGH<<j))==(HIGH<<j)) 
							l_dat1(); 
						else 
							l_dat0();
				}
		}
}

void send_string_data(const u8 *data,u8 size) 											
{
		u8 i=0;
		for(i=0;i<size;i++)
		{
			send_single_data(data);	 
		}
    Reset();
}
//==================================RGB����=====================================//
void RGB_open(const u8 *data) //RGB����
{
		send_single_data(data);
}

//=================================RGB��˸========= BLUE RED GREEN ===============//
void RGB_Lighting(void)
{
		if(Light_blue){
				Gard[0]=255;
				Gard[1]=0;
				Gard[2]=0;
				send_string_data(Gard,SEND_TIMES);              //���ʹ�����ƹܳ����й�
			  Light_blue=0;
		}
		else if(Light_red){
			Gard[0]=0;
			Gard[1]=255;
			Gard[2]=0;
			send_string_data(Gard,SEND_TIMES); 
			Light_red=0;
		}
		else if(Light_green){
				Gard[0]=0;
				Gard[1]=0;
				Gard[2]=255;
				send_string_data(Gard,SEND_TIMES); 
				Light_green=0;
		}
}

//=================================��ʱ========= BLUE RED GREEN ===============//
static void delay(unsigned long int us)
{
		unsigned long int j;
		for (j = 5*us; j> 0; j--) ;
}





