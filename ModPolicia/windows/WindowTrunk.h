#pragma once

#include "menu/IMenuVSL.h"

class WindowTrunk {
public:
	static IWindow* m_Window;

	static void Create();
	static void Remove();

	static void CreateEditTrunk(int trunkIndex);

};