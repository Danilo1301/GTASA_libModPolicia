#pragma once

#include "pch.h"

#include "menu/IMenuVSL.h"

#include "InventoryItem.h"
#include "Vehicle.h"

class WindowFrisk {
public:
    static IWindow* m_Window;
    static IWindow* m_WindowItemActions;
    
	static void Create();
	static void Remove();

	static void CreateItemActions(InventoryItem* item, std::function<void()> onClose);
	static void RemoveItemActions();

	static void CreateFriskCar(Vehicle* vehicle);
};