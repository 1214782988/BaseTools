#ifndef __OLED_FONT_H
#define __OLED_FONT_H

#include "stdint.h"

typedef struct ASCIIFont_Def {
	uint8_t Width;
	uint8_t Height;
	uint8_t *Chars;
}ASCIIFont_Def;

extern const ASCIIFont_Def OLED_Font8x16;
extern const ASCIIFont_Def OLED_Font8x6;
/**
 * @brief 图片结构体
 * @note  图片数据可以使用波特律动LED取模助手生成(https://led.baud-dance.com)
 */
typedef struct Image_Def {
	uint8_t Width;           // 图片宽度
	uint8_t Height;           // 图片高度
	const uint8_t *data; // 图片数据
} Image_Def;

extern const Image_Def BilibiliImage;
#endif
