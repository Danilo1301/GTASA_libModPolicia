#pragma once

#include "menu/Menu.h"

class WindowTrunk {
public:
	static Window* m_Window;

	static void Create();
	static void Remove();

	static void CreateEditTrunk(int trunkIndex);

};