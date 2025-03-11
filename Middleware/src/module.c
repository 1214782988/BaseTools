#include "module.h"
#include "stdio.h"
#include "log.h"
#include "stdlib.h"
#include "string.h"
#include "stdint.h"



Module ModuleListHead;
static  uint8_t SysInitOver;

static void START(void){}
Start_Init(START);
static void END(void){}
End_Init(END);


void Module_Register(Module *NewModule)
{
	Module *CurList = &ModuleListHead;

	while (CurList->NextModule != NULL) {
		CurList = CurList->NextModule;
	} 
	CurList->NextModule = NewModule;
}

static void Module_AllRegister(void)
{
	__Init *Init = NULL;

	for(Init = &START_Module_Init; Init <= &END_Module_Init; Init++) {
		(*Init)();
	}
}

static void Module_PreInit(void)
{
	Module *CurList = ModuleListHead.NextModule;
	
	while(CurList != NULL) {
		if (SysInitOver)
			LOG_INFO("Module %s Init start!", CurList->Name);

		/* Module GPIO Init */
		if (CurList->Init != NULL) {
			CurList->Init();
		}

		if (CurList->ModuleConfig != NULL) {
			CurList->ModuleConfig();
		}
		if (SysInitOver)
			LOG_INFO("Module %s Init end!", CurList->Name);
		if (strcmp(CurList->Name, "Serial") == 0)
			SysInitOver = 1;
		CurList = CurList->NextModule;
	}
}

void Module_AllInit(void)
{
	Module_AllRegister();
	Module_PreInit();
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