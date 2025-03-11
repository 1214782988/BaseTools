#include "module.h"
#include "gpio.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_exti.h"
#include "key.h"
#include "log.h"
#include "Delay.h"
#include "encoder.h"

uint8_t Key_State[16];
#define ENCODER_SENSITIVITY  	12

Key_Def KEY_BOARD[] = {
	{
		.GpioBank = GPIOA,
		.Pin = GPIO_Pin_11,
		.KeyCode = KEY_ENTER,
	},
	{
		.KeyCode = KEY_UP,
		.IsEncoder = SET,
		.EncoderValue = 85,
	},
	{
		.KeyCode = KEY_DOWN,
		.IsEncoder = SET,
		.EncoderValue = 15,
	},
};

void KeyCode_Init(void)
{
	uint32_t KeyIdx;
	for(KeyIdx = 0; KeyIdx < ARRAY_SIZE(KEY_BOARD); KeyIdx++) {
		KEY_BOARD[KeyIdx].KeyStatus = 0;
	}
}


uint8_t Is_KeyPressed(uint32_t KeyCode)
{
	uint32_t KeyIdx, KeyStatus = 0;

	for(KeyIdx = 0; KeyIdx < ARRAY_SIZE(KEY_BOARD); KeyIdx++) {
		if (KEY_BOARD[KeyIdx].IsEncoder == SET && KeyCode == KEY_BOARD[KeyIdx].KeyCode) {
			if (Encoder_GetCount() >= KEY_BOARD[KeyIdx].EncoderValue - ENCODER_SENSITIVITY && Encoder_GetCount() <= KEY_BOARD[KeyIdx].EncoderValue + ENCODER_SENSITIVITY) {
				KeyStatus = KEY_PRESSED;
				Encoder_SetCount();
			}
		}
		if (KeyCode == KEY_BOARD[KeyIdx].KeyCode) {
			KeyStatus |= KEY_BOARD[KeyIdx].KeyStatus & KEY_PRESSED;
		}
	}

	return KeyStatus;
}

uint8_t Is_KeyRealsed(uint32_t KeyCode)
{
	uint32_t KeyIdx, KeyStatus = 0;

	for(KeyIdx = 0; KeyIdx < ARRAY_SIZE(KEY_BOARD); KeyIdx++) {
		if (KeyCode == KEY_BOARD[KeyIdx].KeyCode) {
			KeyStatus = KEY_BOARD[KeyIdx].KeyStatus & KEY_RELEASE;
			KEY_BOARD[KeyIdx].KeyStatus = KEY_BOARD[KeyIdx].KeyStatus & ~KEY_RELEASE;
		}
	}

	return KeyStatus;
}

static void Key_SetKeyCode(uint32_t Key)
{
	int KeyIdx;

	for(KeyIdx = 0; KeyIdx < ARRAY_SIZE(KEY_BOARD); KeyIdx++) {
		if (Key == KEY_BOARD[KeyIdx].Pin) {
			if (GPIO_ReadInputDataBit(KEY_BOARD[KeyIdx].GpioBank, KEY_BOARD[KeyIdx].Pin)) {
				KEY_BOARD[KeyIdx].KeyStatus |= KEY_RELEASE;
				KEY_BOARD[KeyIdx].KeyStatus &= ~KEY_PRESSED;
			} else {
				KEY_BOARD[KeyIdx].KeyStatus |= KEY_PRESSED;
			}
		}
	}
}


void EXTI15_10_IRQHandler(void)
{
	int Status;

	Delay_ms(10);
	if (EXTI_GetITStatus(EXTI_Line11) == SET) {
		EXTI_ClearITPendingBit(EXTI_Line11);
		Key_SetKeyCode(EXTI_Line11);
	}
}

static void Key_Init(void)
{
	uint8_t GpioIdx;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	//开启GPIOB的时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	
	/*GPIO初始化*/
	GPIO_Def Key_GpioInitStruct[] = {
		{	
			.GpioBank = GPIOA,
			.GpioData = {
				.GPIO_Pin = GPIO_Pin_11,
				.GPIO_Mode = GPIO_Mode_IPU,
				.GPIO_Speed = GPIO_Speed_50MHz,
			},
		},
	};

	for (GpioIdx = 0; GpioIdx < ARRAY_SIZE(Key_GpioInitStruct); GpioIdx++) {
		GPIO_Init(Key_GpioInitStruct[GpioIdx].GpioBank, &Key_GpioInitStruct[GpioIdx].GpioData);
	}
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource11);
	EXTI_InitTypeDef Key_EXTIInitStruct = {
		.EXTI_Line = EXTI_Line11,
		.EXTI_LineCmd = ENABLE,
		.EXTI_Trigger = EXTI_Trigger_Rising_Falling,
		.EXTI_Mode = EXTI_Mode_Interrupt,
	};

	EXTI_Init(&Key_EXTIInitStruct);

	
	NVIC_InitTypeDef Key_NVIC_InitStruct = {
		.NVIC_IRQChannel = EXTI15_10_IRQn,
		.NVIC_IRQChannelCmd = ENABLE,
		.NVIC_IRQChannelPreemptionPriority = 2,
		.NVIC_IRQChannelSubPriority = 2,
	};
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	NVIC_Init(&Key_NVIC_InitStruct);
}

static Module Key ={
	.Name = "Key",
	.Init = Key_Init,
};

MODULE_INIT(Key);
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