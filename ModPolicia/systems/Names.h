#pragma once

#include "pch.h"

#include "Skins.h"

struct Name {
    int gxtId;
    SkinGenre genre;
};

class Names {
public:
    static std::vector<Name> m_Names;

    static Name GetRandomName(SkinGenre genre);
};