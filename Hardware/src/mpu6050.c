#include "mpu6050.h"
#include "module.h"
#include "i2c.h"
#include "gpio.h"
#include "common.h"
#include "log.h"

#define MPU6050_I2CBUS			I2C2
#define MPU6050_I2CADDRESS		0x68

#define	MPU6050_SMPLRT_DIV		0x19
#define	MPU6050_CONFIG			0x1A
#define	MPU6050_GYRO_CONFIG		0x1B
#define	MPU6050_ACCEL_CONFIG	0x1C

#define	MPU6050_ACCEL_XOUT_H	0x3B
#define	MPU6050_ACCEL_XOUT_L	0x3C
#define	MPU6050_ACCEL_YOUT_H	0x3D
#define	MPU6050_ACCEL_YOUT_L	0x3E
#define	MPU6050_ACCEL_ZOUT_H	0x3F
#define	MPU6050_ACCEL_ZOUT_L	0x40
#define	MPU6050_TEMP_OUT_H		0x41
#define	MPU6050_TEMP_OUT_L		0x42
#define	MPU6050_GYRO_XOUT_H		0x43
#define	MPU6050_GYRO_XOUT_L		0x44
#define	MPU6050_GYRO_YOUT_H		0x45
#define	MPU6050_GYRO_YOUT_L		0x46
#define	MPU6050_GYRO_ZOUT_H		0x47
#define	MPU6050_GYRO_ZOUT_L		0x48

#define	MPU6050_PWR_MGMT_1		0x6B
#define	MPU6050_PWR_MGMT_2		0x6C
#define	MPU6050_WHO_AM_I		0x75

I2C_Dev MPU6050_dev = {
	.I2cBus = MPU6050_I2CBUS,
	.ChipAddress = MPU6050_I2CADDRESS,
};

static int MPU6050_I2c_ReadReg(uint8_t Reg)
{
	return I2c_SMbus_ReadByte(&MPU6050_dev, Reg);
}

static int MPU6050_I2c_WriteReg(uint8_t Reg, uint8_t ByteData)
{
	return I2c_SMbus_WriteByte(&MPU6050_dev, Reg, ByteData);
}

uint8_t MPU6050_GetID(void)
{
	return MPU6050_I2c_ReadReg(MPU6050_WHO_AM_I);
}

static void MPU6050_Init(void)
{
	uint8_t GpioIdx;
	/*开启时钟*/
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

	/*GPIO初始化*/
	GPIO_Def I2c_GpioInitStruct[] = {
		{	
			.GpioBank = GPIOB,
			.GpioData = {
				.GPIO_Pin = GPIO_Pin_10,
				.GPIO_Mode = GPIO_Mode_AF_OD,
				.GPIO_Speed = GPIO_Speed_50MHz,
			},
		},
		{
			.GpioBank = GPIOB,
			.GpioData = {
				.GPIO_Pin = GPIO_Pin_11,
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
	I2C_Init(I2C2, &I2c_InitStruct);
	I2C_Cmd(I2C2, ENABLE);
}

Module MPU6050 = {
	.Name = "MPU6050",
	.Init = MPU6050_Init,
};
MODULE_INIT(MPU6050);
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