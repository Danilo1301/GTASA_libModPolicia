#pragma once

#include "../pch.h"

#include "../menu/Menu.h"

class WindowBackup {
public:
    static Window* m_Window;
    
	static void Create();
	static void Remove();
};