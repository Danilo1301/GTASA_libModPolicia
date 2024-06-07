#include "WindowTest.h"

#include "Log.h"
#include "CleoFunctions.h"
#include "Pullover.h"
#include "Peds.h"
#include "Chase.h"
#include "Mod.h"
#include "Ambulance.h"

#include "WindowDocument.h"

Window* WindowTest::m_Window = NULL;

void WindowTest::Create()
{
	if (m_Window) return;

    auto window = m_Window = Menu::AddWindow(6);
    window->position = CVector2D(200, 200); //80, 200
    window->showPageControls = true;

    auto button_position = window->AddButton(9, 1, 0);
    button_position->onClick = [window]() {
        Menu::AddPosition2DWindow(window, &WindowDocument::m_TestPosition, -1000.0f, 1000.0f, 0.5f, []() {});
    };

    window->AddCheckbox(29, &Menu::m_DrawCursor);

    auto button_close = window->AddButton(7, CRGBA(170, 70, 70));
    button_close->onClick = []()
    {
        WindowTest::Remove();
    };
}

void WindowTest::Remove()
{
    m_Window->RemoveThisWindow();
    m_Window = NULL;
}