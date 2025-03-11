STM32 base tools with a system init like Linux.

The hardware init struct like this:
static Module Key ={
	.Name = "Key",
	.Init = Key_Init,
};
MODULE_INIT(Key);

the(Init)function will be called automatic when System start!
