#ifndef __KEY_H
#define __KEY_H

#include "gpio.h"
#include "keycode.h"
#include "common.h"

typedef struct Key_Def {
	GPIO_TypeDef *GpioBank;
	uint16_t Pin;
	uint8_t KeyCode;
	uint8_t KeyStatus;
	uint8_t IsEncoder;
	uint16_t EncoderValue;
}Key_Def;


#define KEY_PRESSED		BIT(0)
#define KEY_RELEASE		BIT(1)

void KeyCode_Init(void);
uint8_t Is_KeyPressed(uint32_t KeyCode);
uint8_t Is_KeyRealsed(uint32_t KeyCode);
#endif
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