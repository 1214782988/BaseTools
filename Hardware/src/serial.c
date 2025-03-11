#include "log.h"
#include "module.h"
#include "serial.h"
#include "stm32f10x_rcc.h"
#include "gpio.h"
#include "common.h"

static void Serial_Init(void)
{
	uint8_t GpioIdx;
	/*����ʱ��*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);	//����USART1��ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	//����GPIOA��ʱ��
	
	/*GPIO��ʼ��*/
	GPIO_Def Serial_GpioInitStruct[] = {
		{	
			.GpioBank = GPIOA,
			.GpioData = {
				.GPIO_Pin = GPIO_Pin_9,
				.GPIO_Mode = GPIO_Mode_AF_PP,
				.GPIO_Speed = GPIO_Speed_50MHz,
			},
		},
		{
			.GpioBank = GPIOA,
			.GpioData = {
				.GPIO_Pin = GPIO_Pin_10,
				.GPIO_Mode = GPIO_Mode_IPU,
				.GPIO_Speed = GPIO_Speed_50MHz,
			}
		}
	};

//	for (GpioIdx = 0; GpioIdx < ARRAY_SIZE(Serial_GpioInitStruct); GpioIdx++) {
//		GPIO_Init(Serial_GpioInitStruct[GpioIdx].GpioBank, &Serial_GpioInitStruct[GpioIdx].GpioData);
//	}
	GPIO_Init(GPIOA, &Serial_GpioInitStruct[0].GpioData);
	GPIO_Init(GPIOA, &Serial_GpioInitStruct[1].GpioData);
	

	/*USART��ʼ��*/
	USART_InitTypeDef USART1_InitStruct = {
		.USART_BaudRate = 115200,										//������
		.USART_HardwareFlowControl = USART_HardwareFlowControl_None,	//Ӳ�������ƣ�����Ҫ
		.USART_Mode = USART_Mode_Tx,									//ģʽ��ѡ��Ϊ����ģʽ
		.USART_Parity = USART_Parity_No,								//��żУ�飬����Ҫ
		.USART_StopBits = USART_StopBits_1,								//ֹͣλ��ѡ��1λ
		.USART_WordLength = USART_WordLength_8b,						//�ֳ���ѡ��8λ
	};
	USART_Init(USART1, &USART1_InitStruct);							//���ṹ���������USART_Init������USART1
	USART_Cmd(USART1, ENABLE);
}

static Module Serial ={
	.Name = "Serial",
	.Init = Serial_Init,
};

CONSOLE_INIT(Serial);
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