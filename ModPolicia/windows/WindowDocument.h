#pragma once

#include "../pch.h"

#include "../Ped.h"

#include "../menu/Menu.h"

enum DOC_TYPE {
	RG,
	CNH
};

class WindowDocument {
public:
    static Window* m_Window;
    static DOC_TYPE m_DocumentType;
    static Ped* m_Ped;
	static std::function<void()> m_OnClose;

    static CVector2D m_TestPosition;

private:
	static void Create();
	static void Remove();

public:
	static void ToggleDocuments(DOC_TYPE documentType, Ped* ped);
	static void Draw();
};