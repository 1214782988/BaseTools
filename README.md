## Useage:

The hardware init struct like this:
```c
static Module Key = {
	.Name = "Key",
	.Init = Key_Init,
};
MODULE_INIT(Key);
```
The(Init)function will be called automatic when System start!

And the App init struct like this:
```c
APP ServoContrl = {
	.Name = "ServoContrl",
	.Icon = &App_WeGame_Icon,
	.Func = ServoContrl_AppStart,
};
APP_INSTALL(ServoContrl);
```
The(Func)function will be called when you entering the App!
The App whll be install in System, then the Icon & Name will appear in AppManager like this:

## Next Tasks
- [x] Uart Data Trans with Windows Tool
- [x] SPI Flash Driver
- [ ] Update *.bin by Uart use the Windows Tool, and save the firwarm in SPI Flash
