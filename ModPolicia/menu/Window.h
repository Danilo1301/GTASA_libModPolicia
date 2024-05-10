#pragma once

#include "Item.h"

class Window {
public:
	static CRGBA defaultButtonColor;

	std::vector<Item*> items;
	std::vector<Item*> floatingItems;

	float width = 320.0f;
	float titleHeight = 20.0f;

	int titleGtxId = 0;
	CVector2D position = { 0, 0 };
	CRGBA titleBoxColor = CRGBA(23, 30, 53);
	CRGBA backgroundColor = CRGBA(54, 70, 124);

	Window* parentWindow = NULL;

	Item* btnLeft = NULL;
	Item* btnRight = NULL;
	Item* btnBack = NULL;

	bool visible = true;

	int maxItemsPerPage = 5;
	int page = 0;
	bool showPageControls = false;
	bool showTitle = true;

	Window();

	Item* AddButton(int gxtId, int num1, int num2, CRGBA color);
	Item* AddButton(int gxtId);
	Item* AddButton(int gxtId, int num1, int num2);
	Item* AddButton(int gxtId, CRGBA color);
	Item* AddFloatingButton(int gxtId, int num1, int num2, CVector2D position, CVector2D size, CRGBA color);
	Item* AddFloatingButton(int gxtId, int num1, int num2, CVector2D position, CVector2D size);
	Item* AddCheckbox(int gxtId, bool* value);
	Item* AddText(int gxtId, CRGBA color);
	Item* AddOptions(int gxtId);
	Item* AddFloatRange(int gxtId, float* value, float min, float max, float addBy);
	Item* AddIntRange(int gxtId, int* value, int min, int max, int addBy);

	void Update();
	void Draw();
	void Destroy();

	std::vector<Item*> GetItemsToDraw();

	void GoToPrevWindow();
	void RemoveThisWindow();

	int GetMaxPages();
};