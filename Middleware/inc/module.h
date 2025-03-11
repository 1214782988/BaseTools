#ifndef __MODULE_H
#define __MODULE_H

typedef struct Module {
	const char* Name;
	void (*Init)(void);
	void (*ModuleConfig)(void);
	struct Module *NextModule;
}Module;

typedef void (*__Init)(void);

#define __INIT__ATTRIBUTE(id) 	__attribute__ ((section ("module."#id))) __attribute__((used))

#define __Device_Init(fn, id) \
static __INIT__ATTRIBUTE(id) __Init fn##_Module_Init = fn

#define Start_Init(fn)		__Device_Init(fn, 0)
#define Console_Init(fn)	__Device_Init(fn, 1)
#define System_Init(fn)		__Device_Init(fn, 2)
#define Module_Init(fn)		__Device_Init(fn, 3)
#define End_Init(fn)		__Device_Init(fn, 4)

#define REGISTER_INIT(module) \
static void module##_Module_Init(void) \
{ \
	Module_Register(&module); \
}

#define CONSOLE_INIT(module) \
REGISTER_INIT(module)\
Console_Init(module##_Module_Init)

#define SYSTEM_INIT(module) \
REGISTER_INIT(module) \
System_Init(module##_Module_Init)

#define MODULE_INIT(module) \
REGISTER_INIT(module)\
Module_Init(module##_Module_Init)


void Module_AllInit(void);
void Module_Register(Module *NewModule);
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