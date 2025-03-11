#include "module.h"
#include "log.h"
#include "oled.h"
#include "string.h"
#include "Delay.h"
#include "Servo.h"

extern void MainSystem(void);
int main(void)
{
	int Angle = 0;
	/* Hareware Init */
	Module_AllInit();
	LOG_INFO("System Start!\n");
	/* Main System Start */
	while (1) {
		MainSystem();
//		Angle += 30;				//角度变量自增30
//		if (Angle > 180)			//角度变量超过180后
//		{
//			Angle = 0;				//角度变量归零
//		}
//		Servo_SetAngle(Angle);			//设置舵机的角度为角度变量
	}
	return 0;
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
