#include "log.h"
#include "i2c.h"
#include "module.h"
#include "stdlib.h"
#include "Delay.h"
#include "oled.h"
#include "string.h"
#include "stdarg.h"
#include "gpio.h"
#include "common.h"

#define OLED_I2CBUS	I2C1
#define OLED_I2CADDRESS	0x78
#define OLED_PAGE 8            // OLEDҳ��
#define OLED_ROW 8 * OLED_PAGE // OLED����
#define OLED_COLUMN 128        // OLED����



uint8_t OLEN_GRAM0[OLED_PAGE][OLED_COLUMN];
uint8_t OLEN_GRAM1[OLED_PAGE][OLED_COLUMN];
uint8_t OLEN_GRAM2[OLED_PAGE][OLED_COLUMN];
uint8_t (*OLEN_GRAM)[OLED_COLUMN] = OLEN_GRAM0;

I2C_Dev Oled_dev = {
	.I2cBus = OLED_I2CBUS,
	.ChipAddress = OLED_I2CADDRESS,
};

enum OLED_Reg {
	WRITECOMMAND	= 0x00,
	READSTATUS		= 0x10,
	WRITEDATA		= 0x40,
	READDATA		= 0x3,
};

static int Oled_I2cWrite_Cmd(uint8_t Cmd)
{
	uint8_t DataBuffer[2];

	DataBuffer[0] = WRITECOMMAND;
	DataBuffer[1] = Cmd;

	return I2c_WriteData(&Oled_dev, DataBuffer, sizeof(DataBuffer));
}

static int Oled_I2cWrite_Data(uint8_t *Data, uint32_t DataLen)
{
	uint8_t DataBuffer[DataLen + 1];

	DataBuffer[0] = WRITEDATA;
	memcpy(DataBuffer + 1, Data, DataLen);

	return I2c_WriteData(&Oled_dev, DataBuffer, sizeof(DataBuffer));
}


// ========================== �Դ�������� ==========================

void Oled_ShowFrame(void)
{
	int Page;

	for (Page = 0; Page < OLED_PAGE; Page++) {
		Oled_I2cWrite_Cmd(0xB0 + Page);
		Oled_I2cWrite_Cmd(0x10);
		Oled_I2cWrite_Cmd(0x00);
		Oled_I2cWrite_Data(OLEN_GRAM[Page], OLED_COLUMN);
	}
}

void Oled_ClearFrame(void)
{
	memset(OLEN_GRAM, 0, sizeof(OLEN_GRAM1));
}

void Oled_Set_GramLayers(uint8_t Layers)
{
	if (Layers == 0) {
		OLEN_GRAM = OLEN_GRAM0;
	} else if (Layers == 1) {
		OLEN_GRAM = OLEN_GRAM1;
	} else if (Layers == 2) {
		OLEN_GRAM = OLEN_GRAM2;
	}
}

void Oled_GramLayers_Mix(uint8_t Layer1, uint8_t Layer2, uint8_t BlendMode)
{
	uint8_t Page = 0;
	uint8_t Column = 0;
	uint8_t (*OLEN_GRAM_1)[OLED_COLUMN];
	uint8_t (*OLEN_GRAM_2)[OLED_COLUMN];

	if (Layer1 == 0) {
		OLEN_GRAM_1 = OLEN_GRAM0;
	} else if (Layer1 == 1) {
		OLEN_GRAM_1 = OLEN_GRAM1;
	} else if (Layer1 == 2) {
		OLEN_GRAM_1 = OLEN_GRAM2;
	}
	if (Layer2 == 0) {
		OLEN_GRAM_2 = OLEN_GRAM0;
	} else if (Layer2 == 1) {
		OLEN_GRAM_2 = OLEN_GRAM1;
	} else if (Layer2 == 2) {
		OLEN_GRAM_2 = OLEN_GRAM2;
	}

	for(; Page < OLED_PAGE; Page++) {
		for(Column = 0; Column < OLED_COLUMN; Column++) {
			if (BlendMode == 0)
				OLEN_GRAM_1[Page][Column] &= OLEN_GRAM_2[Page][Column];
			else if (BlendMode == 1)
				OLEN_GRAM_1[Page][Column] |= OLEN_GRAM_2[Page][Column];
			else if (BlendMode == 2)
				OLEN_GRAM_1[Page][Column] &= ~OLEN_GRAM_2[Page][Column];
		}
	}
	
}

/**
 * @brief �����Դ���һ�ֽ����ݵ�ĳ��λ
 * @param page ҳ��ַ
 * @param column �е�ַ
 * @param data ����
 * @param start ��ʼλ
 * @param end ����λ
 * @param color ��ɫ
 * @note �˺������Դ��е�ĳһ�ֽڵĵ�startλ����endλ����Ϊ��data��ͬ
 * @note start��end�ķ�ΧΪ0-7, start����С�ڵ���end
 * @note �˺�����Oled_SetByte_Fine���������ڴ˺���ֻ�������Դ��е�ĳһ��ʵ�ֽ�
 */
void Oled_SetByte_Fine(uint8_t page, uint8_t column, uint8_t data, uint8_t start, uint8_t end, Oled_Color color) {
  static uint8_t temp;
  if (page >= OLED_COLUMN || column >= OLED_COLUMN) return;
  if (color) data = ~data;

  temp = data | (0xff << (end + 1)) | (0xff >> (8 - start));
  OLEN_GRAM[page][column] &= temp;
  temp = data & ~(0xff << (end + 1)) & ~(0xff >> (8 - start));
  OLEN_GRAM[page][column] |= temp;

}

/**
 * @brief �����Դ��е�һ�ֽ�����
 * @param page ҳ��ַ
 * @param column �е�ַ
 * @param data ����
 * @param color ��ɫ
 * @note �˺������Դ��е�ĳһ�ֽ�����Ϊdata��ֵ
 */
void Oled_SetByte(uint8_t page, uint8_t column, uint8_t data, Oled_Color color) {
  if (page >= OLED_PAGE || column >= OLED_COLUMN) return;
  if (color) data = ~data;
  OLEN_GRAM[page][column] = data;
}

/**
 * @brief �����Դ��е�һ�ֽ����ݵ�ĳ��λ
 * @param x ������
 * @param y ������
 * @param data ����
 * @param len λ��
 * @param color ��ɫ
 * @note �˺������Դ��д�(x,y)��ʼ������lenλ����Ϊ��data��ͬ
 * @note len�ķ�ΧΪ1-8
 * @note �˺�����Oled_SetByte_Fine���������ڴ˺����ĺ��������������������Ϊ��λ��, ���ܳ��ֿ�������ʵ�ֽڵ����(��ҳ)
 */
void Oled_SetBits_Fine(uint8_t x, uint8_t y, uint8_t data, uint8_t len, Oled_Color color) {
  uint8_t page = y / 8;
  uint8_t bit = y % 8;
  if (bit + len > 8) {
    Oled_SetByte_Fine(page, x, data << bit, bit, 7, color);
    Oled_SetByte_Fine(page + 1, x, data >> (8 - bit), 0, len + bit - 1 - 8, color);
  } else {
    Oled_SetByte_Fine(page, x, data << bit, bit, bit + len - 1, color);
  }
}

/**
 * @brief �����Դ���һ�ֽڳ��ȵ�����
 * @param X ������
 * @param Y ������
 * @param Data ����
 * @param Color ��ɫ
 * @note �˺������Դ��д�(X,Y)��ʼ������8λ����Ϊ��Data��ͬ
 * @note �˺�����Oled_SetByte���������ڴ˺����ĺ��������������������Ϊ��λ��, ���ܳ��ֿ�������ʵ�ֽڵ����(��ҳ)
 */
void Oled_SetBits(uint8_t X, uint8_t Y, uint8_t Data, Oled_Color Color) {
  uint8_t Page = Y / 8;
  uint8_t Bit = Y % 8;
  Oled_SetByte_Fine(Page, X, Data << Bit, Bit, 7, Color);
  if (Bit) {
    Oled_SetByte_Fine(Page + 1, X, Data >> (8 - Bit), 0, Bit - 1, Color);
  }
}

void Oled_SetBlock(uint8_t X, uint8_t Y, const uint8_t *Data, uint8_t Width, uint8_t Height, Oled_Color Color)
{
	uint8_t fullRow = Height / 8; // ����������
	uint8_t partBit = Height % 8; // ���������ֽ��е���Чλ��

	for (uint8_t i = 0; i < Width; i++) {
		for (uint8_t j = 0; j < fullRow; j++) {
			Oled_SetBits(X + i, Y + j * 8, Data[i + j * Width], Color);
		}
	}
	if (partBit) {
		uint16_t fullNum = Width * fullRow; // �������ֽ���
		for (uint8_t i = 0; i < Width; i++) {
			Oled_SetBits_Fine(X + i, Y + (fullRow * 8), Data[fullNum + i], partBit, Color);
		}
	}
}

// ========================== ͼ�λ��ƺ��� ==========================
void Oled_DrawPixel(uint8_t X, uint8_t Y, Oled_Color Color)
{
	if (X >= OLED_COLUMN || Y >= OLED_ROW) return;
	if (!Color)
		OLEN_GRAM[Y/8][X] |= 0x01 << (Y % 8);
	else
		OLEN_GRAM[Y/8][X] |= ~(0x01 << (Y % 8));
}

/**
 * @brief ����һ���߶�
 * @param X1 ��ʼ�������
 * @param Y1 ��ʼ��������
 * @param X2 ��ֹ�������
 * @param Y2 ��ֹ��������
 * @param Color ��ɫ
 * @note �˺���ʹ��Bresenham�㷨�����߶�
 */
void Oled_DrawLine(uint8_t X1, uint8_t Y1, uint8_t X2, uint8_t Y2, Oled_Color Color)
{
	static uint8_t temp = 0;
	if (X1 == X2) {
		if (Y1 > Y2) {
			temp = Y1;
			Y1 = Y2;
			Y2 = temp;
		}
		for (uint8_t y = Y1; y <= Y2; y++) {
			Oled_DrawPixel(X1, y, Color);
		}
	} else if (Y1 == Y2) {
		if (X1 > X2) {
			temp = X1;
			X1 = X2;
			X2 = temp;
		}
		for (uint8_t x = X1; x <= X2; x++) {
			Oled_DrawPixel(x, Y1, Color);
		}
	} else {
		// Bresenhamֱ���㷨
		int16_t dx = X2 - X1;
		int16_t dy = Y2 - Y1;
		int16_t ux = ((dx > 0) << 1) - 1;
		int16_t uy = ((dy > 0) << 1) - 1;
		int16_t x = X1, y = Y1, eps = 0;
		dx = abs(dx);
		dy = abs(dy);
		if (dx > dy) {
			for (x = X1; x != X2; x += ux) {
				Oled_DrawPixel(x, y, Color);
				eps += dy;
				if ((eps << 1) >= dx) {
					y += uy;
					eps -= dx;
				}
			}
		} else {
			for (y = Y1; y != Y2; y += uy) {
			Oled_DrawPixel(x, y, Color);
			eps += dx;
			if ((eps << 1) >= dy) {
				x += ux;
				eps -= dy;
			}
			}
		}
	}
}

/**
 * @brief ����һ��������
 * @param x ��ʼ�������
 * @param y ��ʼ��������
 * @param w ���ο��
 * @param h ���θ߶�
 * @param color ��ɫ
 */
void Oled_DrawFilledRectangle(uint8_t x, uint8_t y, uint8_t w, uint8_t h, Oled_Color color)
{
  for (uint8_t i = 0; i < h; i++) {
    Oled_DrawLine(x, y + i, x + w, y + i, color);
  }
}

void Oled_DrawCircle(uint8_t X, uint8_t Y, uint8_t R, Oled_Color Color)
{
	int16_t a = 0, b = R, di = 3 - (R << 1);

	while (a <= b) {
		Oled_DrawPixel(X - b, Y - a, Color);
		Oled_DrawPixel(X - b, Y + a, Color);
		Oled_DrawPixel(X + b, Y - a, Color);
		Oled_DrawPixel(X + b, Y + a, Color);
		Oled_DrawPixel(X - a, Y + b, Color);
		Oled_DrawPixel(X - a, Y - b, Color);
		Oled_DrawPixel(X + a, Y - b, Color);
		Oled_DrawPixel(X + a, Y + b, Color);
		a++;
		if (di < 0) {
			di += 4 * a + 6;
		} else {
			di += 10 + 4 * (a - b);
			b--;
		}
		Oled_DrawPixel(X + a, Y + b, Color);
	}
}

/**
 * @brief ����һ�����Բ
 * @param x Բ�ĺ�����
 * @param y Բ��������
 * @param r Բ�뾶
 * @param color ��ɫ
 * @note �˺���ʹ��Bresenham�㷨����Բ
 */
void OLED_DrawFilledCircle(uint8_t x, uint8_t y, uint8_t r, Oled_Color color)
{
  int16_t a = 0, b = r, di = 3 - (r << 1);
  while (a <= b) {
    for (int16_t i = x - b; i <= x + b; i++) {
      Oled_DrawPixel(i, y + a, color);
      Oled_DrawPixel(i, y - a, color);
    }
    for (int16_t i = x - a; i <= x + a; i++) {
      Oled_DrawPixel(i, y + b, color);
      Oled_DrawPixel(i, y - b, color);
    }
    a++;
    if (di < 0) {
      di += 4 * a + 6;
    } else {
      di += 10 + 4 * (a - b);
      b--;
    }
  }
}

/**
 * @brief ����һ��ͼƬ
 * @param x ��ʼ�������
 * @param y ��ʼ��������
 * @param img ͼƬ
 * @param color ��ɫ
 */
void Oled_DrawImage(uint8_t x, uint8_t y, const Image_Def *img, Oled_Color color)
{
  Oled_SetBlock(x, y, img->data, img->Width, img->Height, color);
}

// ================================ ���ֻ��� ================================
void Oled_DrawChar(uint8_t X, uint8_t Y, char Char, const ASCIIFont_Def *Font, Oled_Color Color)
{
	Oled_SetBlock(X, Y, Font->Chars + (Char - ' ') * (((Font->Height + 7) / 8) * Font->Width), Font->Width, Font->Height, Color);
}

void Oled_DrawString(uint8_t X, uint8_t Y, char *String, const ASCIIFont_Def *Font, Oled_Color Color)
{
	while(*String) {
		Oled_DrawChar(X, Y, *String, Font, Color);
		X += Font->Width;
		String++;
	}
}
/********* Function List *************/
static void Oled_I2cInit(void)
{
	uint8_t GpioIdx;
	/*����ʱ��*/
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	/*GPIO��ʼ��*/
	GPIO_Def I2c_GpioInitStruct[] = {
		{	
			.GpioBank = GPIOB,
			.GpioData = {
				.GPIO_Pin = GPIO_Pin_6,
				.GPIO_Mode = GPIO_Mode_AF_OD,
				.GPIO_Speed = GPIO_Speed_50MHz,
			},
		},
		{
			.GpioBank = GPIOB,
			.GpioData = {
				.GPIO_Pin = GPIO_Pin_7,
				.GPIO_Mode = GPIO_Mode_AF_OD,
				.GPIO_Speed = GPIO_Speed_50MHz,
			}
		}
	};

	for (GpioIdx = 0; GpioIdx < ARRAY_SIZE(I2c_GpioInitStruct); GpioIdx++) {
		GPIO_Init(I2c_GpioInitStruct[GpioIdx].GpioBank, &I2c_GpioInitStruct[GpioIdx].GpioData);
	}
	
	I2C_InitTypeDef I2c_InitStruct = {
		.I2C_ClockSpeed = 400000,
		.I2C_Mode = I2C_Mode_I2C,
		.I2C_Ack = I2C_Ack_Enable,
		.I2C_DutyCycle = I2C_DutyCycle_2,
		.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit,
		.I2C_OwnAddress1 = 0x00,
	};
	I2C_Init(I2C1, &I2c_InitStruct);
	I2C_Cmd(I2C1, ENABLE);	
}

static void Oled_Init(void)
{
	Delay_ms(20);

	Oled_I2cInit();			//�˿ڳ�ʼ��
	
	Oled_I2cWrite_Cmd(0xAE);	//�ر���ʾ
	
	Oled_I2cWrite_Cmd(0xD5);	//������ʾʱ�ӷ�Ƶ��/����Ƶ��
	Oled_I2cWrite_Cmd(0x80);
	
	Oled_I2cWrite_Cmd(0xA8);	//���ö�·������
	Oled_I2cWrite_Cmd(0x3F);
	
	Oled_I2cWrite_Cmd(0xD3);	//������ʾƫ��
	Oled_I2cWrite_Cmd(0x00);
	
	Oled_I2cWrite_Cmd(0x40);	//������ʾ��ʼ��
	
	Oled_I2cWrite_Cmd(0xA1);	//�������ҷ���0xA1���� 0xA0���ҷ���
	
	Oled_I2cWrite_Cmd(0xC8);	//�������·���0xC8���� 0xC0���·���

	Oled_I2cWrite_Cmd(0xDA);	//����COM����Ӳ������
	Oled_I2cWrite_Cmd(0x12);
	
	Oled_I2cWrite_Cmd(0x81);	//���öԱȶȿ���
	Oled_I2cWrite_Cmd(0xCF);

	Oled_I2cWrite_Cmd(0xD9);	//����Ԥ�������
	Oled_I2cWrite_Cmd(0xF1);

	Oled_I2cWrite_Cmd(0xDB);	//����VCOMHȡ��ѡ�񼶱�
	Oled_I2cWrite_Cmd(0x30);

	Oled_I2cWrite_Cmd(0xA4);	//����������ʾ��/�ر�

	Oled_I2cWrite_Cmd(0xA6);	//��������/��ת��ʾ

	Oled_I2cWrite_Cmd(0x8D);	//���ó���
	Oled_I2cWrite_Cmd(0x14);

	Oled_I2cWrite_Cmd(0xAF);	//������ʾ
		
	Oled_ShowFrame();			//OLED����
	
}

static Module Oled ={
	.Name = "Oled",
	.Init = Oled_Init,
};


MODULE_INIT(Oled);
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