#include "Draw.h"

#include "Log.h"

std::vector<DrawItem*> Draw::m_DrawItems;

void Draw::DrawBox(CVector2D pos, CVector2D size, CRGBA color)
{
    DrawItem* item = new DrawItem(eDrawType::BOX);
	//item->type = eDrawType::BOX;
	item->pos = pos;
	item->color = color;
	item->size = size;

	//Log::file << "Draw: DrawBox at (" << pos.x << ", " << pos.y << "), size " << size.x << ", " << size.y << ")" << std::endl;

	m_DrawItems.push_back(item);
}

void Draw::DrawBoxWithText(int gxtId, int num1, int num2, CVector2D pos, CVector2D boxSize, CRGBA boxColor, CRGBA textColor, eTextAlign_old align)
{
	Draw::DrawBox(pos, boxSize, boxColor);

	CVector2D textPos = pos;
	textPos.y += boxSize.y / 2;

	if (align == eTextAlign_old::ALIGN_CENTER_old)
	{
		textPos.x += boxSize.x / 2;
	}

	if (align == eTextAlign_old::ALIGN_RIGHT_old)
	{
		textPos.x += boxSize.x;
		
	}

	Draw::DrawGxtText(gxtId, num1, num2, textPos, textColor, align);
}

void Draw::DrawBoxWithText(int gxtId, int num1, int num2, CVector2D pos, CVector2D boxSize, CRGBA boxColor, CRGBA textColor)
{
	Draw::DrawBoxWithText(gxtId, num1, num2, pos, boxSize, boxColor, textColor, eTextAlign_old::ALIGN_CENTER_old);
}

void Draw::DrawGxtText(int gxtId, int num1, int num2, CVector2D pos, CRGBA color, eTextAlign_old align)
{
	DrawItem* item = new DrawItem(eDrawType::TEXT);
	//item->type = eDrawType::TEXT;
	item->textAlign = align;
	item->gxtId = gxtId;
	item->num1 = num1;
	item->num2 = num2;
	item->pos = pos;
	item->color = color;

	//Log::file << "Draw: DrawGxtText " << gxtId << ", nums " << num1 << " and " << num2 << ", at (" << pos.x << ", " << pos.y << ")" << std::endl;

	m_DrawItems.push_back(item);
}

void Draw::DrawGxtText(int gxtId, int num1, int num2, CVector2D pos, CRGBA color)
{
	DrawGxtText(gxtId, num1, num2, pos, color, eTextAlign_old::ALIGN_CENTER_old);
}

void Draw::DrawSprite(int spriteId, CVector2D pos, CVector2D size, CRGBA color)
{
	DrawItem* item = new DrawItem(eDrawType::SPRITE);
	//item->type = eDrawType::SPRITE;
	item->spriteId = spriteId;
	item->pos = pos;
	item->size = size;
	item->color = color;

    //Log::file << "Draw: DrawSprite " << spriteId << " at (" << pos.x << ", " << pos.y << ")" << std::endl;

	m_DrawItems.push_back(item);
}

void Draw::DrawSpriteWithText(int spriteId, int gxtId, int num1, int num2, CVector2D pos, CVector2D size, CRGBA spriteColor, CRGBA textColor, eTextAlign_old align)
{
	Draw::DrawSprite(spriteId, pos, size, spriteColor);

	CVector2D textPos = pos;
	textPos.y += size.y / 2;

	if (align == eTextAlign_old::ALIGN_CENTER_old)
	{
		textPos.x += size.x / 2;
	}

	if (align == eTextAlign_old::ALIGN_RIGHT_old)
	{
		textPos.x += size.x;

	}

	Draw::DrawGxtText(gxtId, num1, num2, textPos, textColor, align);
}