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
//		Angle += 30;				//�Ƕȱ�������30
//		if (Angle > 180)			//�Ƕȱ�������180��
//		{
//			Angle = 0;				//�Ƕȱ�������
//		}
//		Servo_SetAngle(Angle);			//���ö���ĽǶ�Ϊ�Ƕȱ���
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
