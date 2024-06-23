#include "ModPolicia.h"

#include "Log.h"

#include "windows/WindowRadio.h"

bool ModPolicia::IsRadioOpen()
{
    return WindowRadio::m_Enabled;
}

static ModPolicia modPoliciaLocal;
IModPolicia* modPolicia = &modPoliciaLocal;