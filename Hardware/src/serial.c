#include "log.h"
#include "module.h"
#include "serial.h"
#include "stm32f10x_rcc.h"
#include "gpio.h"
#include "common.h"

static void Serial_Init(void)
{
	uint8_t GpioIdx;
	/*开启时钟*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);	//开启USART1的时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	//开启GPIOA的时钟
	
	/*GPIO初始化*/
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
	

	/*USART初始化*/
	USART_InitTypeDef USART1_InitStruct = {
		.USART_BaudRate = 115200,										//波特率
		.USART_HardwareFlowControl = USART_HardwareFlowControl_None,	//硬件流控制，不需要
		.USART_Mode = USART_Mode_Tx,									//模式，选择为发送模式
		.USART_Parity = USART_Parity_No,								//奇偶校验，不需要
		.USART_StopBits = USART_StopBits_1,								//停止位，选择1位
		.USART_WordLength = USART_WordLength_8b,						//字长，选择8位
	};
	USART_Init(USART1, &USART1_InitStruct);							//将结构体变量交给USART_Init，配置USART1
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