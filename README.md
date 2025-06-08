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
	.Icon = &App_ServoContrl_Icon,
	.Func = ServoContrl_AppStart,
};
APP_INSTALL(ServoContrl);
```
The(Func)function will be called when you entering the App!
The App will be install in System, then the Icon & Name will appear in AppManager like this:

![System UI](Images\system_ui.jpg)

You can use two keys to select APP, and a key to enter, I used a rotary encoder to control the cursor here, you can see a GIF:

![System UI GIF](./Images/system_ui.gif)

## Next Tasks
- [x] Uart Data Trans with Windows Tool
- [ ] SPI Flash Driver
- [ ] Update *.bin by Uart use the Windows Tool, and save the firwarm in SPI Flash
- [ ] Use the SPI Flash and USB slave as a SD card
- [ ] 
