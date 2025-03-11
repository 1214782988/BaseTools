#include "gpio.h"
#include "module.h"
#include "common.h"
#include "encoder.h"
#include "log.h"

int Encoder_GetCount(void)
{
	uint16_t Temp;

	Temp = TIM_GetCounter(TIM3);
//	TIM_SetCounter(TIM3, 0);

	return Temp;
}

void Encoder_SetCount(void)
{
	TIM_SetCounter(TIM3, 0);
}

static void Encoder_Init(void)
{
	uint8_t GpioIdx;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	//����GPIOB��ʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	
	/*GPIO��ʼ��*/
	GPIO_Def Encoder_GpioInitStruct[] = {
		{	
			.GpioBank = GPIOA,
			.GpioData = {
				.GPIO_Pin = GPIO_Pin_6,
				.GPIO_Mode = GPIO_Mode_IPU,
				.GPIO_Speed = GPIO_Speed_50MHz,
			},
		},
		{	
			.GpioBank = GPIOA,
			.GpioData = {
				.GPIO_Pin = GPIO_Pin_7,
				.GPIO_Mode = GPIO_Mode_IPU,
				.GPIO_Speed = GPIO_Speed_50MHz,
			},
		},
	};

	for (GpioIdx = 0; GpioIdx < ARRAY_SIZE(Encoder_GpioInitStruct); GpioIdx++) {
		GPIO_Init(Encoder_GpioInitStruct[GpioIdx].GpioBank, &Encoder_GpioInitStruct[GpioIdx].GpioData);
	}

	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure = {			//����ṹ�����
		.TIM_ClockDivision = TIM_CKD_DIV1,     						//ʱ�ӷ�Ƶ��ѡ�񲻷�Ƶ���˲������������˲���ʱ�ӣ���Ӱ��ʱ����Ԫ����
		.TIM_CounterMode = TIM_CounterMode_Up, 						//������ģʽ��ѡ�����ϼ���
		.TIM_Period = 100 - 1,              						 //�������ڣ���ARR��ֵ
		.TIM_Prescaler = 1 - 1,               						 //Ԥ��Ƶ������PSC��ֵ
		.TIM_RepetitionCounter = 0,      						      //�ظ����������߼���ʱ���Ż��õ�
	};
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStructure);             //���ṹ���������TIM_TimeBaseInit������TIM3��ʱ����Ԫ

	TIM_ICInitTypeDef TIM_ICInitStructure;							//����ṹ�����
	TIM_ICStructInit(&TIM_ICInitStructure);							//�ṹ���ʼ�������ṹ��û��������ֵ

	TIM_ICInitStructure.TIM_Channel = TIM_Channel_1;				//ѡ�����ö�ʱ��ͨ��1
	TIM_ICInitStructure.TIM_ICFilter = 0xF;							//�����˲������������Թ����źŶ���
	TIM_ICInit(TIM3, &TIM_ICInitStructure);							//���ṹ���������TIM_ICInit������TIM3�����벶��ͨ��
	TIM_ICInitStructure.TIM_Channel = TIM_Channel_2;				//ѡ�����ö�ʱ��ͨ��2
	TIM_ICInitStructure.TIM_ICFilter = 0xF;							//�����˲������������Թ����źŶ���
	TIM_ICInit(TIM3, &TIM_ICInitStructure);							//���ṹ���������TIM_ICInit������TIM3�����벶��ͨ��
	
	/*�������ӿ�����*/
	TIM_EncoderInterfaceConfig(TIM3, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);
	/*TIMʹ��*/
	TIM_Cmd(TIM3, ENABLE);			//ʹ��TIM3����ʱ����ʼ����
}

Module Encoder = {
	.Name = "Encoder",
	.Init = Encoder_Init,
};

MODULE_INIT(Encoder);
/*
 *
 *
 *
 *
 *
 *
 *
 *
 */