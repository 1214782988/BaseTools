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
 * @brief ͼƬ�ṹ��
 * @note  ͼƬ���ݿ���ʹ�ò����ɶ�LEDȡģ��������(https://led.baud-dance.com)
 */
typedef struct Image_Def {
	uint8_t Width;           // ͼƬ���
	uint8_t Height;           // ͼƬ�߶�
	const uint8_t *data; // ͼƬ����
} Image_Def;

extern const Image_Def BilibiliImage;
#endif
