#include "log.h"
#include "stm32f10x_usart.h"

int fputc(int ch,FILE* fp)
{
	USART_SendData(LOG_TO_USARTX,ch);
	while(USART_GetFlagStatus(LOG_TO_USARTX, USART_FLAG_TC) != SET);
	return ch;
}
