#pragma once

#include "../pch.h"

#include "Vehicle.h"

#include "../menu/Menu.h"

class WindowCarMenu {
public:
    static Window* m_Window;
	static Vehicle* m_Vehicle;

	static void Create(Vehicle* vehicle);
	static void Remove();
};