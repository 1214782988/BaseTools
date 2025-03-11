#include "oled.h"
#include "font.h"
#include "stdlib.h"
#include "AppManager.h"
#include "log.h"
#include "key.h"
#include "AppIcon.h"
#include "encoder.h"
#include "string.h"

static void START(void){}
Start_Install(START);
static void END(void){}
End_Install(END);

#define SCREEN_W	127
#define SCREEN_H	63
	
#define ICON_W		30
#define ICON_H		30
#define ICON_GAP	10
#define ICON_X	9
#define ICON_Y	12

#define CURSOR_GAP	2
#define CURSOR_X1 (ICON_X + ICON_GAP + ICON_W - CURSOR_GAP)
#define CURSOR_Y1 (ICON_Y - CURSOR_GAP)
#define CURSOR_X2 (ICON_X + ICON_GAP + ICON_W*2 + CURSOR_GAP)
#define CURSOR_Y2 (ICON_Y + ICON_H + CURSOR_GAP)
#define CURSOR_LEN	3

#define DOTTED_LINE_X 0
#define DOTTED_LINE_Y ICON_Y + ICON_H + 6
#define DOTTED_LINE_W 9
#define DOTTED_LINE_H 2

#define ARROW_X1	0
#define ARROW_X2	(SCREEN_W - Page_ArrowRight.Height)
#define ARROW_Y		DOTTED_LINE_Y + 4

#define NAME_X	30
#define NAME_Y1	65
#define NAME_Y2	DOTTED_LINE_Y + 6

enum Key_Eevent{
	Key_Up,
	Key_Down,
	Key_Enter,
	Key_Max,
};

struct User_Page {
	APP *CurApp;
	uint16_t Keys[Key_Max];
	float Icon_CurrX;
	float Icon_DestX;
	float Cursor_CurrX;
	float Cursor_DestX;
	float AppIdx_CurrX;
	float AppIdx_DestX;
	float Name_CurrY;
	float Name_DestY;
	float Arrow_CurrX1;
	float Arrow_DestX1;
	float Arrow_CurrX2;
	float Arrow_DestX2;
	uint8_t AppNum;
};

struct APP *AppListHead;
uint8_t Apps = 0;

void App_install(APP *NewApp)
{
	APP *CurAppList = AppListHead;
	
	if (AppListHead == NULL) {
		AppListHead = NewApp;
		AppListHead->AppId = 1;
	} else {
		while (CurAppList->NextApp != NULL) {
				CurAppList = CurAppList->NextApp;
		} 
		CurAppList->NextApp = NewApp;
		NewApp->PreApp = CurAppList;
		NewApp->AppId = CurAppList->AppId + 1;
	}
	Apps++;
	
}

static enum Key_Eevent App_CheckKeyEvent(struct User_Page *User_Page)
{
	uint8_t Key_Eevent;

	for(Key_Eevent = 0; Key_Eevent < Key_Max; Key_Eevent++) {
		if (Is_KeyPressed(User_Page->Keys[Key_Eevent])) {
			return Key_Eevent;
		}
	}

	return Key_Max;
}

static void Show_PositionCursor(struct User_Page *UserPage)
{
	UserPage->Cursor_CurrX += (UserPage->Cursor_DestX - UserPage->Cursor_CurrX)/10;
	Oled_DrawLine(CURSOR_X1 + UserPage->Cursor_CurrX, CURSOR_Y1, CURSOR_X1 + CURSOR_LEN + UserPage->Cursor_CurrX, 			   CURSOR_Y1, OLED_COLOR_NORMAL);
	Oled_DrawLine(CURSOR_X1 + UserPage->Cursor_CurrX, CURSOR_Y1, 			  CURSOR_X1 + UserPage->Cursor_CurrX, CURSOR_Y1 + CURSOR_LEN, OLED_COLOR_NORMAL);
	Oled_DrawLine(CURSOR_X1 + UserPage->Cursor_CurrX, CURSOR_Y2, CURSOR_X1 + CURSOR_LEN + UserPage->Cursor_CurrX, 			   CURSOR_Y2, OLED_COLOR_NORMAL);
	Oled_DrawLine(CURSOR_X1 + UserPage->Cursor_CurrX, CURSOR_Y2, 			  CURSOR_X1 + UserPage->Cursor_CurrX, CURSOR_Y2 - CURSOR_LEN, OLED_COLOR_NORMAL);
	Oled_DrawLine(CURSOR_X2 + UserPage->Cursor_CurrX, CURSOR_Y1, CURSOR_X2 - CURSOR_LEN + UserPage->Cursor_CurrX,			   CURSOR_Y1, OLED_COLOR_NORMAL);
	Oled_DrawLine(CURSOR_X2 + UserPage->Cursor_CurrX, CURSOR_Y1, 			  CURSOR_X2 + UserPage->Cursor_CurrX, CURSOR_Y1 + CURSOR_LEN, OLED_COLOR_NORMAL);
	Oled_DrawLine(CURSOR_X2 + UserPage->Cursor_CurrX, CURSOR_Y2, CURSOR_X2 - CURSOR_LEN + UserPage->Cursor_CurrX, 			   CURSOR_Y2, OLED_COLOR_NORMAL);
	Oled_DrawLine(CURSOR_X2 + UserPage->Cursor_CurrX, CURSOR_Y2, 			  CURSOR_X2 + UserPage->Cursor_CurrX, CURSOR_Y2 - CURSOR_LEN, OLED_COLOR_NORMAL);
}



static void Show_AppNums(struct User_Page *UserPage)
{
	UserPage->AppIdx_CurrX += (UserPage->AppIdx_DestX - UserPage->AppIdx_CurrX)/10;
	UserPage->AppIdx_DestX = SCREEN_W / UserPage->AppNum * UserPage->CurApp->AppId;
	Oled_DrawFilledRectangle(0,0,UserPage->AppIdx_CurrX,3,OLED_COLOR_NORMAL);
	Oled_DrawPixel(UserPage->AppIdx_CurrX + 1, 1, OLED_COLOR_NORMAL);
}

static void Show_AppName(struct User_Page *UserPage)
{
	UserPage->Name_CurrY += (UserPage->Name_DestY - UserPage->Name_CurrY)/10;
	Oled_DrawString((SCREEN_W - strlen(UserPage->CurApp->Name)*OLED_Font8x6.Width)/2, UserPage->Name_CurrY ,UserPage->CurApp->Name, &OLED_Font8x6,OLED_COLOR_NORMAL);
}
                                                                                              
static void Show_Dott(struct User_Page *UserPage)
{
	uint8_t Idx;
	for (Idx = 0; Idx < SCREEN_W; Idx += DOTTED_LINE_W + 6)
		Oled_DrawFilledRectangle(DOTTED_LINE_X + Idx, DOTTED_LINE_Y, DOTTED_LINE_W, DOTTED_LINE_H, OLED_COLOR_NORMAL);
}

static void Show_Arrow(struct User_Page *UserPage)
{
	UserPage->Arrow_CurrX1 += (UserPage->Arrow_DestX1 - UserPage->Arrow_CurrX1)/10;
	UserPage->Arrow_CurrX2 += (UserPage->Arrow_DestX2 - UserPage->Arrow_CurrX2)/10;
	Oled_DrawImage(UserPage->Arrow_CurrX1, ARROW_Y, &Page_ArrowLeft,OLED_COLOR_NORMAL);
	Oled_DrawImage(UserPage->Arrow_CurrX2, ARROW_Y, &Page_ArrowRight,OLED_COLOR_NORMAL);
}

static void Show_App(struct User_Page *UserPage)
{
	for (APP *AppIdx = AppListHead; AppIdx != NULL; AppIdx = AppIdx->NextApp) {
		if (((ICON_X + UserPage->Icon_CurrX + (AppIdx->AppId - 1)* (ICON_W + ICON_GAP)) > SCREEN_W) || 
			((ICON_X + UserPage->Icon_CurrX + (AppIdx->AppId - 1)* (ICON_W + ICON_GAP)) < 0 - ICON_W)) {
			continue;	
		}
		Oled_DrawImage((uint8_t)(ICON_X + UserPage->Icon_CurrX + (AppIdx->AppId - 1) * (ICON_W + ICON_GAP)), ICON_Y, AppIdx->Icon, OLED_COLOR_NORMAL);
	}
}


static void Show_UI(struct User_Page *UserPage)
{
	APP *AppPointer = UserPage->CurApp;
	int EncoderValue = 0;
	switch (App_CheckKeyEvent(UserPage)) {
		case Key_Up:
			EncoderValue = ICON_W + ICON_GAP;
			if (AppPointer->PreApp != NULL)
				AppPointer = AppPointer->PreApp;
			break;
		case Key_Down:
			EncoderValue = -(ICON_W + ICON_GAP);
			if (AppPointer->NextApp != NULL)
				AppPointer = AppPointer->NextApp;
			break;
		case Key_Enter:
			UserPage->CurApp->Func();
			KeyCode_Init();
			break;
		default:
			break;
	}
	UserPage->Icon_CurrX += (UserPage->Icon_DestX - UserPage->Icon_CurrX)/10;
	if ((AppPointer->NextApp == NULL && EncoderValue < 0 ) || (AppPointer->PreApp == NULL && EncoderValue > 0));
	else {
		UserPage->Icon_DestX += EncoderValue;
		UserPage->Name_DestY = NAME_Y2;
	}

	if (AppPointer->PreApp == NULL) {
		UserPage->Cursor_DestX = -(ICON_W + ICON_GAP);
	} else if (UserPage->Icon_DestX > -41 && UserPage->Icon_DestX < 0) {
		if (UserPage->Cursor_DestX < 0) {
			UserPage->Cursor_DestX = 0;
			UserPage->Icon_DestX = 0;
		}
	}

	if (AppPointer->NextApp == NULL) {
		UserPage->Cursor_DestX = ICON_W + ICON_GAP;
	} else if (UserPage->CurApp->NextApp == NULL && EncoderValue > 0) {
		UserPage->Cursor_DestX = 0;
		UserPage->Icon_DestX -= EncoderValue;
	}
	if (EncoderValue > 0) {
		if (UserPage->CurApp->PreApp != NULL) {
			UserPage->CurApp = UserPage->CurApp->PreApp;
			UserPage->Name_CurrY = NAME_Y1;
			UserPage->Arrow_CurrX1 = ARROW_X1 + 5;
		}
	} else if (EncoderValue < 0){
		if (UserPage->CurApp->NextApp != NULL) {
			UserPage->CurApp = UserPage->CurApp->NextApp;
			UserPage->Name_CurrY = NAME_Y1;
			UserPage->Arrow_CurrX2 = ARROW_X2 - 5;
		}
	}

	Oled_ClearFrame();

	Show_Dott(UserPage);
	Show_AppNums(UserPage);
	Show_AppName(UserPage);
	Show_Arrow(UserPage);
	Show_App(UserPage);
	Show_PositionCursor(UserPage);

	Oled_ShowFrame();
}

static void App_AllInstall(void)
{
	__Install *Install = NULL;

	for(Install = &START_App_Install; Install<=&END_App_Install; Install++) {
		(*Install)();
	}
}


void MainSystem(void)
{
	enum Key_Eevent CurKeyEevent;

	App_AllInstall();
	
	struct User_Page UserPage = {
		.CurApp = AppListHead->NextApp,
		.AppNum = Apps,
		.Keys = {
			KEY_UP, KEY_DOWN, KEY_ENTER,
		},
		.Name_CurrY = NAME_Y1,
		.Arrow_DestX1 = ARROW_X1,
		.Arrow_CurrX2 = ARROW_X2,
		.Arrow_DestX2 = ARROW_X2,
	};

	while (1) {
		Show_UI(&UserPage);
	}

}

//APP WeGame = {
//	.Name = "WeGame",
//	.Icon = &App_WeGame_Icon,
//	.Func = NULL,
//};

//APP_INSTALL(WeGame);

APP BaiDuYun = {
	.Name = "BaiDuYun",
	.Icon = &App_BaiDuYun_Icon,
	.Func = NULL,
};

APP_INSTALL(BaiDuYun);

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