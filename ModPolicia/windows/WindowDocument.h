#pragma once

#include "../pch.h"

#include "../Ped.h"

#include "../menu/Menu.h"

#include "menu/IMenuVSL.h"

enum DOC_TYPE {
	RG,
	CNH
};

class WindowDocument {
public:
    static IWindow* m_Window;
    static DOC_TYPE m_DocumentType;
    static Ped* m_Ped;
	static std::function<void()> m_OnClose;

    static CVector2D m_TestPosition;

	static CSprite2d m_RG_sprite;
	static CSprite2d m_CNH_sprite;

private:
	static void Create();
	static void Remove();

public:
	static void ToggleDocuments(DOC_TYPE documentType, Ped* ped);
	static void Draw();
};