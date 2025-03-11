#include "gpio.h"
#include "module.h"
#include "common.h"
#include "encoder.h"
#include "log.h"

int Encoder_GetCount(void)
{
	uint16_t Temp;

	Temp = TIM_GetCounter(TIM3);
//	TIM_SetCounter(TIM3, 0);

	return Temp;
}

void Encoder_SetCount(void)
{
	TIM_SetCounter(TIM3, 0);
}

static void Encoder_Init(void)
{
	uint8_t GpioIdx;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	//开启GPIOB的时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	
	/*GPIO初始化*/
	GPIO_Def Encoder_GpioInitStruct[] = {
		{	
			.GpioBank = GPIOA,
			.GpioData = {
				.GPIO_Pin = GPIO_Pin_6,
				.GPIO_Mode = GPIO_Mode_IPU,
				.GPIO_Speed = GPIO_Speed_50MHz,
			},
		},
		{	
			.GpioBank = GPIOA,
			.GpioData = {
				.GPIO_Pin = GPIO_Pin_7,
				.GPIO_Mode = GPIO_Mode_IPU,
				.GPIO_Speed = GPIO_Speed_50MHz,
			},
		},
	};

	for (GpioIdx = 0; GpioIdx < ARRAY_SIZE(Encoder_GpioInitStruct); GpioIdx++) {
		GPIO_Init(Encoder_GpioInitStruct[GpioIdx].GpioBank, &Encoder_GpioInitStruct[GpioIdx].GpioData);
	}

	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure = {			//定义结构体变量
		.TIM_ClockDivision = TIM_CKD_DIV1,     						//时钟分频，选择不分频，此参数用于配置滤波器时钟，不影响时基单元功能
		.TIM_CounterMode = TIM_CounterMode_Up, 						//计数器模式，选择向上计数
		.TIM_Period = 100 - 1,              						 //计数周期，即ARR的值
		.TIM_Prescaler = 1 - 1,               						 //预分频器，即PSC的值
		.TIM_RepetitionCounter = 0,      						      //重复计数器，高级定时器才会用到
	};
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStructure);             //将结构体变量交给TIM_TimeBaseInit，配置TIM3的时基单元

	TIM_ICInitTypeDef TIM_ICInitStructure;							//定义结构体变量
	TIM_ICStructInit(&TIM_ICInitStructure);							//结构体初始化，若结构体没有完整赋值

	TIM_ICInitStructure.TIM_Channel = TIM_Channel_1;				//选择配置定时器通道1
	TIM_ICInitStructure.TIM_ICFilter = 0xF;							//输入滤波器参数，可以过滤信号抖动
	TIM_ICInit(TIM3, &TIM_ICInitStructure);							//将结构体变量交给TIM_ICInit，配置TIM3的输入捕获通道
	TIM_ICInitStructure.TIM_Channel = TIM_Channel_2;				//选择配置定时器通道2
	TIM_ICInitStructure.TIM_ICFilter = 0xF;							//输入滤波器参数，可以过滤信号抖动
	TIM_ICInit(TIM3, &TIM_ICInitStructure);							//将结构体变量交给TIM_ICInit，配置TIM3的输入捕获通道
	
	/*编码器接口配置*/
	TIM_EncoderInterfaceConfig(TIM3, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);
	/*TIM使能*/
	TIM_Cmd(TIM3, ENABLE);			//使能TIM3，定时器开始运行
}

Module Encoder = {
	.Name = "Encoder",
	.Init = Encoder_Init,
};

MODULE_INIT(Encoder);
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