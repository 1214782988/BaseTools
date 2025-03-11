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
#define OLED_PAGE 8            // OLED页数
#define OLED_ROW 8 * OLED_PAGE // OLED行数
#define OLED_COLUMN 128        // OLED列数



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


// ========================== 显存操作函数 ==========================

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
 * @brief 设置显存中一字节数据的某几位
 * @param page 页地址
 * @param column 列地址
 * @param data 数据
 * @param start 起始位
 * @param end 结束位
 * @param color 颜色
 * @note 此函数将显存中的某一字节的第start位到第end位设置为与data相同
 * @note start和end的范围为0-7, start必须小于等于end
 * @note 此函数与Oled_SetByte_Fine的区别在于此函数只能设置显存中的某一真实字节
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
 * @brief 设置显存中的一字节数据
 * @param page 页地址
 * @param column 列地址
 * @param data 数据
 * @param color 颜色
 * @note 此函数将显存中的某一字节设置为data的值
 */
void Oled_SetByte(uint8_t page, uint8_t column, uint8_t data, Oled_Color color) {
  if (page >= OLED_PAGE || column >= OLED_COLUMN) return;
  if (color) data = ~data;
  OLEN_GRAM[page][column] = data;
}

/**
 * @brief 设置显存中的一字节数据的某几位
 * @param x 横坐标
 * @param y 纵坐标
 * @param data 数据
 * @param len 位数
 * @param color 颜色
 * @note 此函数将显存中从(x,y)开始向下数len位设置为与data相同
 * @note len的范围为1-8
 * @note 此函数与Oled_SetByte_Fine的区别在于此函数的横坐标和纵坐标是以像素为单位的, 可能出现跨两个真实字节的情况(跨页)
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
 * @brief 设置显存中一字节长度的数据
 * @param X 横坐标
 * @param Y 纵坐标
 * @param Data 数据
 * @param Color 颜色
 * @note 此函数将显存中从(X,Y)开始向下数8位设置为与Data相同
 * @note 此函数与Oled_SetByte的区别在于此函数的横坐标和纵坐标是以像素为单位的, 可能出现跨两个真实字节的情况(跨页)
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
	uint8_t fullRow = Height / 8; // 完整的行数
	uint8_t partBit = Height % 8; // 不完整的字节中的有效位数

	for (uint8_t i = 0; i < Width; i++) {
		for (uint8_t j = 0; j < fullRow; j++) {
			Oled_SetBits(X + i, Y + j * 8, Data[i + j * Width], Color);
		}
	}
	if (partBit) {
		uint16_t fullNum = Width * fullRow; // 完整的字节数
		for (uint8_t i = 0; i < Width; i++) {
			Oled_SetBits_Fine(X + i, Y + (fullRow * 8), Data[fullNum + i], partBit, Color);
		}
	}
}

// ========================== 图形绘制函数 ==========================
void Oled_DrawPixel(uint8_t X, uint8_t Y, Oled_Color Color)
{
	if (X >= OLED_COLUMN || Y >= OLED_ROW) return;
	if (!Color)
		OLEN_GRAM[Y/8][X] |= 0x01 << (Y % 8);
	else
		OLEN_GRAM[Y/8][X] |= ~(0x01 << (Y % 8));
}

/**
 * @brief 绘制一条线段
 * @param X1 起始点横坐标
 * @param Y1 起始点纵坐标
 * @param X2 终止点横坐标
 * @param Y2 终止点纵坐标
 * @param Color 颜色
 * @note 此函数使用Bresenham算法绘制线段
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
		// Bresenham直线算法
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
 * @brief 绘制一个填充矩形
 * @param x 起始点横坐标
 * @param y 起始点纵坐标
 * @param w 矩形宽度
 * @param h 矩形高度
 * @param color 颜色
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
 * @brief 绘制一个填充圆
 * @param x 圆心横坐标
 * @param y 圆心纵坐标
 * @param r 圆半径
 * @param color 颜色
 * @note 此函数使用Bresenham算法绘制圆
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
 * @brief 绘制一张图片
 * @param x 起始点横坐标
 * @param y 起始点纵坐标
 * @param img 图片
 * @param color 颜色
 */
void Oled_DrawImage(uint8_t x, uint8_t y, const Image_Def *img, Oled_Color color)
{
  Oled_SetBlock(x, y, img->data, img->Width, img->Height, color);
}

// ================================ 文字绘制 ================================
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
	/*开启时钟*/
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	/*GPIO初始化*/
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

	Oled_I2cInit();			//端口初始化
	
	Oled_I2cWrite_Cmd(0xAE);	//关闭显示
	
	Oled_I2cWrite_Cmd(0xD5);	//设置显示时钟分频比/振荡器频率
	Oled_I2cWrite_Cmd(0x80);
	
	Oled_I2cWrite_Cmd(0xA8);	//设置多路复用率
	Oled_I2cWrite_Cmd(0x3F);
	
	Oled_I2cWrite_Cmd(0xD3);	//设置显示偏移
	Oled_I2cWrite_Cmd(0x00);
	
	Oled_I2cWrite_Cmd(0x40);	//设置显示开始行
	
	Oled_I2cWrite_Cmd(0xA1);	//设置左右方向，0xA1正常 0xA0左右反置
	
	Oled_I2cWrite_Cmd(0xC8);	//设置上下方向，0xC8正常 0xC0上下反置

	Oled_I2cWrite_Cmd(0xDA);	//设置COM引脚硬件配置
	Oled_I2cWrite_Cmd(0x12);
	
	Oled_I2cWrite_Cmd(0x81);	//设置对比度控制
	Oled_I2cWrite_Cmd(0xCF);

	Oled_I2cWrite_Cmd(0xD9);	//设置预充电周期
	Oled_I2cWrite_Cmd(0xF1);

	Oled_I2cWrite_Cmd(0xDB);	//设置VCOMH取消选择级别
	Oled_I2cWrite_Cmd(0x30);

	Oled_I2cWrite_Cmd(0xA4);	//设置整个显示打开/关闭

	Oled_I2cWrite_Cmd(0xA6);	//设置正常/倒转显示

	Oled_I2cWrite_Cmd(0x8D);	//设置充电泵
	Oled_I2cWrite_Cmd(0x14);

	Oled_I2cWrite_Cmd(0xAF);	//开启显示
		
	Oled_ShowFrame();			//OLED清屏
	
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