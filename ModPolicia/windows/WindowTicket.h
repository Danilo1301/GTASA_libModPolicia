#pragma once

#include "../pch.h"

#include "../menu/Menu.h"
#include "menu/IMenuVSL.h"

class WindowTicket {
public:
    static IWindow* m_Window;

	static void Create();
	static void Remove();

	static void IssueTicket(int price);
};
