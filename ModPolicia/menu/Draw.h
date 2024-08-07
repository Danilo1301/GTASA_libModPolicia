#pragma once

#include <vector>

#include "eDrawInfoType.h"

#include "SimpleGTA.h"

enum eDrawType {
	BOX,
	TEXT,
	SPRITE
};

enum eTextAlign_old {
	ALIGN_LEFT_old,
	ALIGN_CENTER_old,
	ALIGN_RIGHT_old
};

class DrawItem {
public:
	eDrawType type = eDrawType::BOX;
	eTextAlign_old textAlign = eTextAlign_old::ALIGN_CENTER_old;
	int gxtId = 0;
	int num1 = 0;
	int num2 = 0;
	CVector2D pos = { 0, 0 };
	CVector2D size = { 0, 0 };
	CRGBA color = { 255, 255, 255, 255 };
	int spriteId = -1;

	DrawItem(eDrawType type)
	{
		this->type = type;
	}

	~DrawItem() {};
};

class Draw {
public:
	static std::vector<DrawItem*> m_DrawItems;

    static void DrawBox(CVector2D pos, CVector2D size, CRGBA color);
	static void DrawGxtText(int gxtId, int num1, int num2, CVector2D pos, CRGBA color, eTextAlign_old align);
	static void DrawGxtText(int gxtId, int num1, int num2, CVector2D pos, CRGBA color);
	static void DrawBoxWithText(int gxtId, int num1, int num2, CVector2D pos, CVector2D boxSize, CRGBA boxColor, CRGBA textColor, eTextAlign_old align);
	static void DrawBoxWithText(int gxtId, int num1, int num2, CVector2D pos, CVector2D boxSize, CRGBA boxColor, CRGBA textColor);
	static void DrawSprite(int spriteId, CVector2D pos, CVector2D size, CRGBA color);
	static void DrawSpriteWithText(int spriteId, int gxtId, int num1, int num2, CVector2D pos, CVector2D size, CRGBA spriteColor, CRGBA textColor, eTextAlign_old align);
};