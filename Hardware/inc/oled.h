#ifndef __OLED_H
#define __OLED_H

#include "stdint.h"
#include "font.h"


#define OLED_GRAM_LAYER0	0
#define OLED_GRAM_LAYER1	1
#define OLED_GRAM_LAYER2	2

typedef enum Oled_Color {
	OLED_COLOR_NORMAL = 0,
	OLED_COLOR_INVERSE = 1,
}Oled_Color;

void Oled_ShowFrame(void);
void Oled_ClearFrame(void);
void Oled_GramLayers_Mix(uint8_t Layer1, uint8_t Layer2, uint8_t BlendMode);
void Oled_Set_GramLayers(uint8_t Layers);
void Oled_DrawChar(uint8_t X, uint8_t Y, char Char, const ASCIIFont_Def *Font, Oled_Color Color);
void Oled_DrawLine(uint8_t X1, uint8_t Y1, uint8_t X2, uint8_t Y2, Oled_Color Color);
void Oled_DrawCircle(uint8_t X, uint8_t Y, uint8_t R, Oled_Color color);
void OLED_DrawFilledCircle(uint8_t x, uint8_t y, uint8_t r, Oled_Color color);
void Oled_DrawPixel(uint8_t X, uint8_t Y, Oled_Color Color);
void Oled_DrawFilledRectangle(uint8_t x, uint8_t y, uint8_t w, uint8_t h, Oled_Color color);

void Oled_DrawImage(uint8_t x, uint8_t y, const Image_Def *img, Oled_Color color);
void Oled_DrawString(uint8_t X, uint8_t Y, char *String, const ASCIIFont_Def *Font, Oled_Color Color);

#endif