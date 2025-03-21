#pragma once

#include "../pch.h"

#include "../menu/Menu.h"
#include "menu/IMenuVSL.h"

class WindowPullover {
public:
    static IWindow* m_Window;
    
	static void CreatePullingPed();
	static void CreatePullingCar();
	static void CreateFriskCar();
	static void CreateScorchWindow();
	static void CreateDialogWindow();
	static void Remove();
};