#pragma once

#include "IModPolicia.h"

class ModPolicia : public IModPolicia {
public:
    bool IsRadioOpen();
    bool IsMenuOpen();
};

extern IModPolicia* modPolicia;