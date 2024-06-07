#pragma once

#include "../menu/Menu.h"

class WindowRadio {
public:
	static Window* m_Window;

	static void Create();
	static void Remove();

	static void CreateTestOptions();
};