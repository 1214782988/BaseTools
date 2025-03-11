#include "oled.h"
#include "stdlib.h"
#include "stdio.h"
#include "stm32f10x_gpio.h"
#include "log.h"
#include "string.h"
#include "AppManager.h"
#include "AppIcon.h"
#include "key.h"

#define DINOSAUR_INIT_X		10
#define DINOSAUR_INIT_Y		63 - 23

#define BASE_LINE_X 0
#define BASE_LINE_Y 63
#define BASE_LINE_X1 127

// Dino crashed
const uint8_t dino_crashedData[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xfe, 0xff, 0xf1, 0xf5, 0xf1, 0xff, 0xff, 0xff, 0xff, 0x7f, 0x7e, 0x7f, 0xfc, 0xf8, 0xe0, 0xe0, 0xf8, 0xfc, 0xfc, 0xfe, 0xff, 0xff, 0xff, 0xff, 0xff, 0x7f, 0x04, 0x1c, 0x00, 0x00, 0x00, 0x00, 0x80, 0x80, 0x81, 0x83, 0x87, 0xff, 0xdf, 0x87, 0x87, 0x87, 0xff, 0xc3, 0xc3, 0x81, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
};
const Image_Def Dinosaur_Crashed_Image = {21, 23, dino_crashedData};
// Dino lifts front leg
const uint8_t dino_front_legData[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xfe, 0xff, 0xff, 0xfb, 0xff, 0xff, 0xbf, 0xbf, 0xbf, 0x3f, 0x3e, 0x3f, 0xfc, 0xf8, 0xf0, 0xf0, 0xf8, 0xfc, 0xfc, 0xfe, 0xff, 0xff, 0xff, 0xff, 0xff, 0x7f, 0x04, 0x0c, 0x00, 0x00, 0x00, 0x00, 0x80, 0x80, 0x81, 0x83, 0x87, 0xff, 0xdf, 0x8f, 0x87, 0x87, 0x8f, 0x8b, 0x8b, 0x81, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
};
const Image_Def Dinosaur_FrontLeg_Image = {21, 23, dino_front_legData};
// Dino lifts back leg
const uint8_t dino_back_legData[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xfe, 0xff, 0xff, 0xfb, 0xff, 0xff, 0xbf, 0xbf, 0xbf, 0x3f, 0x3e, 0x3f, 0xfc, 0xf8, 0xf0, 0xf0, 0xf8, 0xfc, 0xfc, 0xfe, 0xff, 0xff, 0xff, 0xff, 0xff, 0x7f, 0x04, 0x0c, 0x00, 0x00, 0x00, 0x00, 0x80, 0x80, 0x81, 0x83, 0x87, 0x8f, 0x9f, 0x97, 0x87, 0x8f, 0xff, 0xc3, 0xc3, 0x81, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
};
const Image_Def Dinosaur_BackLeg_Image = {21, 23, dino_back_legData};
// Dino jumps
const uint8_t dino_jumpsData[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xfe, 0xff, 0xff, 0xfb, 0xff, 0xff, 0xbf, 0xbf, 0xbf, 0x3f, 0x3e, 0x3f, 0xfc, 0xf8, 0xf0, 0xf0, 0xf8, 0xfc, 0xfc, 0xfe, 0xff, 0xff, 0xff, 0xff, 0xff, 0x7f, 0x04, 0x0c, 0x00, 0x00, 0x00, 0x00, 0x80, 0x80, 0x81, 0x83, 0x87, 0xff, 0xdf, 0x8f, 0x87, 0x8f, 0xff, 0xc3, 0xc3, 0x81, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
};
const Image_Def Dinosaur_Jump_Image = {21, 23, dino_jumpsData};


// Small tree
const uint8_t tree_smallData[] = {
   0xf8, 0x00, 0xff, 0xff, 0x00, 0xfc, 0xf0, 0x00
};
const Image_Def Tree_Small_Image = {8, 8, tree_smallData};
// Big tree
const uint8_t tree_bigData[] = {
    0x80, 0xc0, 0x80, 0x00, 0xff, 0xff, 0xff, 0xff, 0x00, 0xe0, 0xe0, 0xe0, 0x3f, 0x7f, 0x7f, 0x60, 0xff, 0xff, 0xff, 0xff, 0x60, 0x7f, 0x3f, 0x1f, 0x80, 0x80, 0x80, 0x80, 0xff, 0xff, 0xff, 0xff, 0x80, 0x80, 0x80, 0x80,
};
const Image_Def Tree_Big_Image = {12, 23, tree_bigData};

/* Jump Time-Distance Table */
const uint8_t Time_Distance[38] = {0, 4, 7, 11, 14, 17, 20, 23, 25, 28, 30, 31, 33, 34, 35, 36, 37, 37, 38, 38, 37, 37, 36, 35, 34, 33, 31, 30, 28, 25, 23, 20, 17, 14, 11, 7, 4, 0};


	
enum Dinosaur_Actions {
	Dinosaur_FrontLeg,
	Dinosaur_BackLeg,
	Dinosaur_Jump,
	Dinosaur_Crashed,
};

enum Tree_Type {
	Tree_Small,
	Tree_Big,
};

enum Key_Type {
	Key_Jump,
	Key_Pause,
	Key_Start_Exit,
	Key_Max,
};

uint8_t Dinosaur_KeyCode[] = {
	[Key_Jump] = KEY_UP,
//	[Key_Pause] = KEY_UP,
	[Key_Start_Exit] = KEY_ENTER,
};

enum Score_Type {
	OnGaming,
	HighScore,
	GameOver,
};

struct Tree {
	int16_t Tree_Xcoordinate;
	int16_t Tree_Ycoordinate;
	const Image_Def *Tree_Image[4];
	enum Tree_Type TreeType;
};

struct Dinosaur {
	int16_t Dinosaur_Xcoordinate;
	int16_t Dinosaur_Ycoordinate;
	const Image_Def *Dinosaur_Image[4];
	enum Dinosaur_Actions Action;
};

static uint32_t High_Score = 0;

void Draw_MenuInfo(void)
{
	uint8_t R;
	for (R = 0; R < 80; R += 2) {
		Oled_Set_GramLayers(OLED_GRAM_LAYER2);
		Oled_ClearFrame();
		OLED_DrawFilledCircle(64, 32, R, OLED_COLOR_NORMAL);
		Oled_GramLayers_Mix(OLED_GRAM_LAYER0, OLED_GRAM_LAYER2, 2);
		Oled_Set_GramLayers(OLED_GRAM_LAYER1);
		Oled_ClearFrame();
		Oled_DrawString((BASE_LINE_X1 - sizeof("Dinosaur Game")*8) / 2, 8, "Dinosaur Game", &OLED_Font8x16, OLED_COLOR_NORMAL);
		Oled_DrawImage(DINOSAUR_INIT_X, DINOSAUR_INIT_Y, &Dinosaur_Jump_Image, OLED_COLOR_NORMAL);
		Oled_DrawString((BASE_LINE_X1 - 32) / 2, 43, "Start", &OLED_Font8x16, OLED_COLOR_NORMAL);
		Oled_GramLayers_Mix(OLED_GRAM_LAYER1, OLED_GRAM_LAYER2, 0);
		Oled_GramLayers_Mix(OLED_GRAM_LAYER0, OLED_GRAM_LAYER1, 1);
		Oled_Set_GramLayers(OLED_GRAM_LAYER0);
		Oled_ShowFrame();
	}


}


static void Draw_Dinosaur(struct Dinosaur *Dinosaur)
{
	Oled_DrawImage(Dinosaur->Dinosaur_Xcoordinate, Dinosaur->Dinosaur_Ycoordinate, Dinosaur->Dinosaur_Image[Dinosaur->Action], OLED_COLOR_NORMAL);
}

static void Draw_Tree(struct Tree *Tree)
{
	if (Tree->Tree_Xcoordinate > BASE_LINE_X1 - Tree->Tree_Image[Tree->TreeType]->Width) return;

	Oled_DrawImage(Tree->Tree_Xcoordinate, Tree->Tree_Ycoordinate - Tree->Tree_Image[Tree->TreeType]->Width, Tree->Tree_Image[Tree->TreeType], OLED_COLOR_NORMAL);
}

static void Draw_Score(int Score, enum Score_Type ScoreType)
{
	char ScoreString[15];
	char GameOverString[] = "Game Over";

	switch (ScoreType) {
		case GameOver:
			Oled_DrawString(BASE_LINE_X1 - strlen(GameOverString) * 8, 40, GameOverString, &OLED_Font8x16, OLED_COLOR_NORMAL);
			break;
		case OnGaming:
			sprintf(ScoreString, "Score:%d", Score);
			Oled_DrawString(BASE_LINE_X1 - strlen(ScoreString) * 8, 3, ScoreString, &OLED_Font8x16, OLED_COLOR_NORMAL);
			break;
		case HighScore:
			sprintf(ScoreString, "HighScore:%d", Score);
			Oled_DrawString(BASE_LINE_X1 - strlen(ScoreString) * 8, 10, ScoreString, &OLED_Font8x16, OLED_COLOR_NORMAL);
			break;
		default:
			break;
	}
}

static uint8_t Check_IsCrashed(struct Dinosaur *Dinosaur, struct Tree *Tree, int16_t dino_right, int jump)
{
	if (Tree->Tree_Xcoordinate <= (dino_right - ((jump < 5) ? jump + 5 : 10)) &&
		(Tree->Tree_Xcoordinate > Dinosaur->Dinosaur_Xcoordinate + 1) &&
		(Dinosaur->Dinosaur_Ycoordinate + Dinosaur_BackLeg_Image.Height) >= (Tree->Tree_Ycoordinate - Tree->Tree_Image[Tree->TreeType]->Height) + (Tree->TreeType == Tree_Small ? 5 : 7) ||
		(Tree->Tree_Xcoordinate + Tree->Tree_Image[Tree->TreeType]->Width <= (dino_right - ((jump < 5) ? jump + 5 : 10)) && 
		(Tree->Tree_Xcoordinate + Tree->Tree_Image[Tree->TreeType]->Width > Dinosaur->Dinosaur_Xcoordinate + 1) &&
		((Dinosaur->Dinosaur_Ycoordinate + Dinosaur_BackLeg_Image.Height) >= (Tree->Tree_Ycoordinate - Tree->Tree_Image[Tree->TreeType]->Height) + (Tree->TreeType == Tree_Small ? 5 : 7)))) {
		return 1;
	}

	return 0;
}

static uint8_t Check_KeyEvent(uint8_t IsCheckPress)
{
	uint8_t Key_Eevent;

	if (IsCheckPress) {
		for(Key_Eevent = 0; Key_Eevent < Key_Max; Key_Eevent++) {
			if (Is_KeyPressed(Dinosaur_KeyCode[Key_Eevent])) {
				return Key_Eevent;
			}
		}
	} else {
		for(Key_Eevent = 0; Key_Eevent < Key_Max; Key_Eevent++) {
			if (Is_KeyRealsed(Dinosaur_KeyCode[Key_Eevent])) {
				return Key_Eevent;
			}
		}
	}

	return Key_Max;
}

static uint16_t Get_RandomNum(uint16_t NumMax)
{
	static int RandNum =  12345;

	srand(RandNum);

	RandNum = rand();

	return RandNum % NumMax;
}

static void Dinosaur_GameStart(void)
{	
	uint16_t Score = 0;
	uint16_t Score_temp = 0;
	uint8_t jump = 0;
	uint8_t Leg = 0;
	uint8_t Level = 2;
	int Distance;

	int16_t dino_right = DINOSAUR_INIT_X + Dinosaur_FrontLeg_Image.Width;
	
	struct Dinosaur Dinosaur = {
		.Dinosaur_Xcoordinate = DINOSAUR_INIT_X,
		.Dinosaur_Ycoordinate = DINOSAUR_INIT_Y,
		.Dinosaur_Image = {
			&Dinosaur_FrontLeg_Image,
			&Dinosaur_BackLeg_Image,
			&Dinosaur_Jump_Image,
			&Dinosaur_Crashed_Image,
		}
	};
	struct Tree Tree1 = {
		.Tree_Xcoordinate = 250 + Get_RandomNum(200),
		.Tree_Ycoordinate = BASE_LINE_Y,
		.TreeType = !!Get_RandomNum(2),
		.Tree_Image = {
			&Tree_Small_Image,
			&Tree_Big_Image,
		},
	};
	struct Tree Tree2 = {
		.Tree_Xcoordinate = 400 + Get_RandomNum(200),
		.Tree_Ycoordinate = BASE_LINE_Y,
		.TreeType = !!Get_RandomNum(2),
		.Tree_Image = {
			&Tree_Small_Image,
			&Tree_Big_Image,
		},
	};

	while (1) {
		Oled_ClearFrame();
		
		if (Check_KeyEvent(1) == Key_Jump) {
			if (jump == 0)
					jump = 1;
		} else if (Check_KeyEvent(1) == Key_Start_Exit) {
			return;
		}
		
		if (Check_IsCrashed(&Dinosaur, &Tree1, dino_right, jump) || Check_IsCrashed(&Dinosaur, &Tree2, dino_right, jump)) {
			Dinosaur.Action = Dinosaur_Crashed;
			Draw_Dinosaur(&Dinosaur);
			Draw_Tree(&Tree1);
			Draw_Tree(&Tree2);
			Oled_DrawLine(BASE_LINE_X, BASE_LINE_Y, BASE_LINE_X1, BASE_LINE_Y, OLED_COLOR_NORMAL);
			if (Score > High_Score) {
				High_Score = Score;
				Draw_Score(Score, HighScore);
			} else {
				Draw_Score(Score, OnGaming);
			}
			Draw_Score(Score, GameOver);
			Oled_ShowFrame();
			break;
		}

		Score_temp++;
		Score = Score_temp/5;
		if (!(Score + 1 % 501))
			Level++;
		Draw_Score(Score, OnGaming);

		if (jump > 0) {
			Dinosaur.Dinosaur_Ycoordinate = DINOSAUR_INIT_Y - Time_Distance[jump - 1];
		if (++jump > 38) jump = 0;
		}

		if (jump != 0) {
			Dinosaur.Action = Dinosaur_Jump;
		} else {
			if (Leg++ > 9) {
				Leg = 0;
			} else {
				Dinosaur.Action = (Leg > 4 ? Dinosaur_BackLeg : Dinosaur_FrontLeg);
			}
		}

		Draw_Dinosaur(&Dinosaur);
		
		Draw_Tree(&Tree1);
		Draw_Tree(&Tree2);
		Oled_DrawLine(BASE_LINE_X, BASE_LINE_Y, BASE_LINE_X1, BASE_LINE_Y, OLED_COLOR_NORMAL);
		
		if (Tree1.Tree_Xcoordinate < 0) {
			Tree1.Tree_Xcoordinate = 250 + Get_RandomNum(200);
			Tree1.TreeType = !!Get_RandomNum(2);
		} else {
			Tree1.Tree_Xcoordinate -= Level;
		}
		if (Tree2.Tree_Xcoordinate < 0) {
			Tree2.Tree_Xcoordinate = 500 + Get_RandomNum(200);
			Tree2.TreeType = !!Get_RandomNum(2);
		} else {
			Tree2.Tree_Xcoordinate -= Level;
		}

		Distance = Tree1.Tree_Xcoordinate - Tree2.Tree_Xcoordinate;
		if (0 < Distance && Distance <= 100)
			Tree1.Tree_Xcoordinate += Distance;
		else if (-100 < Distance && Distance <= 0)
			Tree2.Tree_Xcoordinate += Distance;

		Oled_ShowFrame();
	}
}

static void Dinosaur_AppStart(void)
{
	KeyCode_Init();

	Draw_MenuInfo();

	while (Check_KeyEvent(1) != Key_Jump) {
		if (Check_KeyEvent(1) == Key_Start_Exit)
			break;
	}
	while (1) {
		Dinosaur_GameStart();
		while (Check_KeyEvent(1) != Key_Jump) {
			if (Check_KeyEvent(1) == Key_Start_Exit)
				return;
		}
	}
}

APP DINO = {
	.Name = "JUMP",
	.Icon = &App_Dinosaur_Icon,
	.Func = Dinosaur_AppStart,
};

APP_INSTALL(DINO);
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
