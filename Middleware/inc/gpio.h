#ifndef __GPIO_H
#define __GPIO_H

#include "stm32f10x_gpio.h"

typedef struct GPIO_Def {
	GPIO_TypeDef *GpioBank;
	GPIO_InitTypeDef GpioData;	
}GPIO_Def;

//#define GPIO_INIT(Bank, Pin, Speed, Mode) \
//		GPIO_InitTypeDef Bank##Pin##GpioInitStruct = { \
//			.GPIO_Pin = Pin, \
//			.GPIO_Mode = Mode, \
//			.GPIO_Speed = Speed, \
//		}; \
//		GPIO_Init(Bank, &Bank##Pin##GpioInitStruct)

#endif