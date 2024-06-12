#include "WindowPD_Menu.h"

#include "ModConfig.h"
#include "Mod.h"

Window* WindowPD_Menu::m_Window = NULL;

void WindowPD_Menu::Create()
{
	if (m_Window) return;

    auto window = m_Window = Menu::AddWindow(6);
    window->showPageControls = true;

    if(Mod::m_Enabled)
    {
        auto button_leaveDuty = window->AddButton(142);
        button_leaveDuty->onClick = []()
        {
            Remove();
            Mod::ToggleMod(false);
        };
    } else {
        auto button_enterDuty = window->AddButton(141);
        button_enterDuty->onClick = []()
        {
            Remove();
            Mod::ToggleMod(true);
        };
    }

    window->AddCheckbox(139, &ModConfig::CreateTestOptionsInRadioMenu);

    auto button_close = window->AddButton(7, CRGBA(170, 70, 70));
    button_close->onClick = []()
    {
        Remove();
    };
}

void WindowPD_Menu::Remove()
{
    m_Window->RemoveThisWindow();
    m_Window = NULL;
}