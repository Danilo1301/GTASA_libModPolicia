#pragma once

#include "../pch.h"

#include "../menu/Menu.h"

class WindowTicket {
public:
    static Window* m_Window;

	static void Create();
	static void Remove();

	static void IssueTicket(int price);
};
