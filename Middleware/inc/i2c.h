#ifndef __I2C_H
#define __I2C_H

#include "stm32f10x_usart.h"

#define I2C_PORT_I2C1

typedef struct I2C_Dev {
	I2C_TypeDef* I2cBus;
	uint8_t ChipAddress;
}I2C_Dev;


int I2c_WriteData_Byte(I2C_Dev *I2cDev, uint8_t ByteData);
int I2c_WriteData_Word(I2C_Dev *I2cDev, uint16_t WordData);
int I2c_WriteData(I2C_Dev *I2cDev, uint8_t *ByteDataBuffer, uint32_t DataLen);
int I2c_SMbus_ReadByte(I2C_Dev *I2cDev, uint8_t Reg);
int I2c_SMbus_WriteByte(I2C_Dev *I2cDev, uint8_t Reg, uint8_t ByteData);

#endif