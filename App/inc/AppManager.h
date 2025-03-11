#ifndef __APPMANAGER_H
#define __APPMANAGER_H

#include "font.h"

typedef struct APP {
	char *Name;
	uint8_t AppId;
	const Image_Def *Icon;
	void (*Func)(void);
	struct APP *NextApp;
	struct APP *PreApp;
}APP;

typedef void (*__Install)(void);

#define __INIT__ATTRIBUTE(id) 	__attribute__ ((section ("app."#id))) __attribute__((used))

#define __App_Install(fn, id) \
static __INIT__ATTRIBUTE(id) __Install fn##_App_Install = fn

#define Start_Install(fn)	__App_Install(fn, 0)
#define App_Install(fn)		__App_Install(fn, 1)
#define End_Install(fn)		__App_Install(fn, 2)

#define APP_INSTALL(app) \
static void app##_App_Install(void) \
{ \
	App_install(&app); \
} \
App_Install(app##_App_Install)

void App_install(APP *NewApp);
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