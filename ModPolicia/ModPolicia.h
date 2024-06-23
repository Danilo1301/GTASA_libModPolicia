#pragma once

#include "IModPolicia.h"

class ModPolicia : public IModPolicia {
public:
    bool IsRadioOpen();
};

extern IModPolicia* modPolicia;