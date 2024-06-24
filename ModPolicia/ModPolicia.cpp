#include "ModPolicia.h"

#include "Log.h"

#include "windows/WindowRadio.h"
#include "menu/Menu.h"

bool ModPolicia::IsRadioOpen()
{
    return WindowRadio::m_Enabled;
}

bool ModPolicia::IsMenuOpen()
{
    for(auto window : Menu::m_Windows)
    {
        if(window == Menu::m_MainWindow) continue;
        return true;
    }
    return false;
}

static ModPolicia modPoliciaLocal;
IModPolicia* modPolicia = &modPoliciaLocal;