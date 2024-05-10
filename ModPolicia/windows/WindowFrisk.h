#pragma once

#include "../pch.h"

#include "../menu/Menu.h"

#include "../InventoryItem.h"

class WindowFrisk {
public:
    static Window* m_Window;
    static Window* m_WindowItemActions;
    
	static void Create();
	static void Remove();

	static void CreateItemActions(InventoryItem* item);
	static void RemoveItemActions();
};