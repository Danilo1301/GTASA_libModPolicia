#pragma once

#include "menu/Menu.h"

#include "Chase.h"

class WindowPD_Menu {
public:
	static Window* m_Window;

	static void Create();
	static void Remove();

	static void CreatePartnerMenu();
	static void CreateEditBarrier(BarrierModel* barrierModel);
	static void CreateEditCamera();
};