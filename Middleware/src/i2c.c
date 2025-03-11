#include "stm32f10x_i2c.h"
#include "module.h"
#include "i2c.h"
#include "error.h"
#include "log.h"

static int I2c_WaitEvent(I2C_TypeDef* I2Cx, uint32_t I2C_EVENT)
{
	uint32_t TimeOut = 10000;

	while (I2C_CheckEvent(I2Cx, I2C_EVENT) != SUCCESS) {
		if (TimeOut-- == 0) {
			LOG_ERROR("I2C_EVENT:%X",I2C_EVENT);
			return RetTimeout;
		}
	}

	return RetOk;
}

int I2c_WriteData(I2C_Dev *I2cDev, uint8_t *ByteDataBuffer, uint32_t DataLen)
{
	uint32_t BufferIdx = 0;

	I2C_GenerateSTART(I2cDev->I2cBus, ENABLE);
	if (I2c_WaitEvent(I2cDev->I2cBus, I2C_EVENT_MASTER_MODE_SELECT)) {
		goto I2c_error;
	}
	I2C_Send7bitAddress(I2cDev->I2cBus, I2cDev->ChipAddress, I2C_Direction_Transmitter);
	if (I2c_WaitEvent(I2cDev->I2cBus, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)) {
		goto I2c_error;
	}
	for (BufferIdx = 0; BufferIdx < DataLen - 1; BufferIdx++) {
		I2C_SendData(I2cDev->I2cBus, ByteDataBuffer[BufferIdx]);
		if (I2c_WaitEvent(I2cDev->I2cBus, I2C_EVENT_MASTER_BYTE_TRANSMITTING)) {
			goto I2c_error;
		}
	}
	I2C_SendData(I2cDev->I2cBus, ByteDataBuffer[BufferIdx]);
	if (I2c_WaitEvent(I2cDev->I2cBus, I2C_EVENT_MASTER_BYTE_TRANSMITTED)) {
		goto I2c_error;
	}
	I2C_GenerateSTOP(I2cDev->I2cBus, ENABLE);

	return RetOk;

I2c_error:
	LOG_ERROR("I2c Write Timeout!");
	return RetTimeout;
}

int I2c_WriteData_Byte(I2C_Dev *I2cDev, uint8_t ByteData)
{
	I2C_GenerateSTART(I2cDev->I2cBus, ENABLE);
	if (I2c_WaitEvent(I2cDev->I2cBus, I2C_EVENT_MASTER_MODE_SELECT)) {
		goto I2c_error;
	}
	I2C_Send7bitAddress(I2cDev->I2cBus, I2cDev->ChipAddress, I2C_Direction_Transmitter);
	if (I2c_WaitEvent(I2cDev->I2cBus, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)) {
		goto I2c_error;
	}
	I2C_SendData(I2cDev->I2cBus, ByteData);
	if (I2c_WaitEvent(I2cDev->I2cBus, I2C_EVENT_MASTER_BYTE_TRANSMITTED)) {
		goto I2c_error;
	}
	I2C_GenerateSTOP(I2cDev->I2cBus, ENABLE);

	return RetOk;

I2c_error:
	LOG_ERROR("I2c Write Timeout!");
	return RetTimeout;
}

int I2c_ReadData_Byte(I2C_Dev *I2cDev)
{
	uint8_t ByteData;

	I2C_GenerateSTART(I2cDev->I2cBus, ENABLE);
	if (I2c_WaitEvent(I2cDev->I2cBus, I2C_EVENT_MASTER_MODE_SELECT)) {
		goto I2c_error;
	}
	I2C_Send7bitAddress(I2cDev->I2cBus, I2cDev->ChipAddress, I2C_Direction_Receiver);
	if (I2c_WaitEvent(I2cDev->I2cBus, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED)) {
		goto I2c_error;
	}
	if (I2c_WaitEvent(I2cDev->I2cBus, I2C_EVENT_MASTER_BYTE_RECEIVED)) {
		I2C_SendData(I2cDev->I2cBus, ByteData);
		I2C_GenerateSTOP(I2cDev->I2cBus, ENABLE);
	} else {
		goto I2c_error;
	}

	return ByteData;

I2c_error:
	LOG_ERROR("I2c Write Timeout!");
	return RetTimeout;
}

int I2c_WriteData_Word(I2C_Dev *I2cDev, uint16_t WordData)
{
	uint32_t I2cState;
	uint8_t ByteData[2];
	
	ByteData[0] = WordData >> 8;
	ByteData[1] = WordData;

	I2C_GenerateSTART(I2cDev->I2cBus, ENABLE);
	if (I2c_WaitEvent(I2cDev->I2cBus, I2C_EVENT_MASTER_MODE_SELECT)) {
		goto I2c_error;
	}
	I2C_Send7bitAddress(I2cDev->I2cBus, I2cDev->ChipAddress, I2C_Direction_Transmitter);
	if (I2c_WaitEvent(I2cDev->I2cBus, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)) {
		goto I2c_error;
	}
	I2C_SendData(I2cDev->I2cBus, ByteData[0]);
	if (I2c_WaitEvent(I2cDev->I2cBus, I2C_EVENT_MASTER_BYTE_TRANSMITTING)) {
		goto I2c_error;
	}
	I2C_SendData(I2cDev->I2cBus, ByteData[0]);
	if (I2c_WaitEvent(I2cDev->I2cBus, I2C_EVENT_MASTER_BYTE_TRANSMITTED)) {
		goto I2c_error;
	}
	I2C_GenerateSTOP(I2cDev->I2cBus, ENABLE);

	return RetOk;

I2c_error:
	LOG_ERROR("I2c Write Timeout!");
	return RetTimeout;
}

int I2c_SMbus_ReadByte(I2C_Dev *I2cDev, uint8_t Reg)
{
	int Ret;

	Ret = I2c_WriteData_Byte(I2cDev, Reg);
	if (Ret)
		goto I2c_error;
	Ret = I2c_ReadData_Byte(I2cDev);
	if(Ret < 0)
		goto I2c_error;

	return Ret;

I2c_error:
	return RetTimeout;
}

int I2c_SMbus_WriteByte(I2C_Dev *I2cDev, uint8_t Reg, uint8_t ByteData)
{
	int Ret;
	uint16_t WordData = Reg << 8 | ByteData;

	Ret = I2c_WriteData_Word(I2cDev, WordData);
	if (Ret)
		goto I2c_error;

I2c_error:
	return RetTimeout;
}
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