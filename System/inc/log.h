#ifndef __LOG_H
#define __LOG_H

#include <stdio.h>

// ≈‰÷√
#define LOG_TO_USARTX		USART1
#define LOG_ON
#define LOG_WITH_COLOR
//#define LOG_WITH_FILENAME
#define LOG_WITH_END
#define LOG_END_CRLF
////////////////////////

#ifdef LOG_WITH_END
#ifdef  LOG_END_CRLF
#define LOG_END		"\r\n"
#else
#define LOG_END      "\n"
#endif
#else
#define LOG_END		""
#endif


#ifdef LOG_WITH_FILENAME
#define LOG_BASE_FILENAME \
    (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : \
    strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)
#else
#define LOG_BASE_FILENAME	" "
#endif

#ifdef LOG_WITH_COLOR
#define LOG_COLOR_RED       "\033[31m"
#define LOG_COLOR_GREEN     "\033[32m"
#define LOG_COLOR_YELLOW    "\033[33m"
#else
#define LOG_COLOR_RED
#define LOG_COLOR_GREEN
#define LOG_COLOR_YELLOW
#endif

#ifdef LOG_ON
#define LOG_INFO(fmt, ...)		printf(LOG_COLOR_GREEN "[INFO]:%s%s %d: " fmt LOG_END, \
												LOG_BASE_FILENAME, __func__, __LINE__,##__VA_ARGS__)
#define LOG_DEBUG(fmt, ...)		printf(LOG_COLOR_YELLOW "[DEBUG]:%s%s %d: " fmt LOG_END, \
												LOG_BASE_FILENAME, __func__, __LINE__,##__VA_ARGS__)
#define LOG_ERROR(fmt, ...)		printf(LOG_COLOR_RED "[ERROR]:%s%s %d: " fmt LOG_END, \
												LOG_BASE_FILENAME, __func__, __LINE__,##__VA_ARGS__)
#else
#define LOG_INFO(fmt, ...)
#define LOG_DEBUG(fmt, ...)
#define LOG_ERROR(fmt, ...)
#endif

#endif
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