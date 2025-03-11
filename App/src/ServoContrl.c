#include "Servo.h"
#include "AppManager.h"
#include "AppIcon.h"
#include "key.h"
#include "oled.h"
#include "stdio.h"
#include "string.h"
#include "log.h"
#include "encoder.h"

enum Key_Eevent{
	Key_Right,
	Key_Left,
	Key_Max,
};

struct Servo {
	enum Key_Eevent Keys[Key_Max];
};



static enum Key_Eevent Servo_CheckKeyEvent(struct Servo *Servo)
{
	uint8_t Key_Eevent;

	for(Key_Eevent = 0; Key_Eevent < Key_Max; Key_Eevent++) {
		if (Is_KeyPressed(Servo->Keys[Key_Eevent])) {
			return Key_Eevent;
		}
	}

	return Key_Max;
}

static void Show_UI(float Angle)
{
	char String[32];
	
	sprintf(String, "Angle: %.2f", Angle);
	Oled_ClearFrame();
	Oled_DrawString(40, 32, String, &OLED_Font8x6, OLED_COLOR_NORMAL);
	Oled_ShowFrame();
}

static void ServoContrl_AppStart(void)
{
	float Angle = 0;
	struct Servo Servo = {
		.Keys = {
			KEY_UP, KEY_DOWN,
		},
	};
	
	Oled_Set_GramLayers(OLED_GRAM_LAYER0);
	Oled_ClearFrame();
	while(1) {
		switch(Servo_CheckKeyEvent(&Servo)) {
			case Key_Right:
				if (Angle < 180)
					Angle += 18;
				else
					Angle = 180;
				break;
			case Key_Left:
				if (Angle > 0)
					Angle -= 18;
				else
					Angle = 0;
				break;
			default:
				break;
		};
//		Angle = (float)Encoder_GetCount() * 1.8;
		Servo_SetAngle(Angle);
		Show_UI(Angle);
	}
}



APP ServoContrl = {
	.Name = "ServoContrl",
	.Icon = &App_WeGame_Icon,
	.Func = ServoContrl_AppStart,
};

APP_INSTALL(ServoContrl);