#pragma once

#include "../pch.h"

#include "Vehicle.h"

#include "../menu/Menu.h"
#include "menu/IMenuVSL.h"

class WindowCarMenu {
public:
    static IWindow* m_Window;
	static Vehicle* m_Vehicle;

	static void Create(Vehicle* vehicle);
	static void Remove();
};