#pragma once

#include "pch.h"

enum SkinGenre {
    SKIN_MALE,
    SKIN_FEMALE,
    BOTH
};

enum SkinGang {
    GANG_NONE,
    GANG_BALLAS,
    GANG_VAGOS,
    GANG_AZTECAS
};

/*
int modelId;
SkinGenre genre;
SkinGang gang;
*/
struct SkinData {
    int modelId;
    SkinGenre genre;
    SkinGang gang;
};

class Skins {
public:
    static std::vector<SkinData> m_Skins;

    static SkinData GetRandomSkin(SkinGenre genre, SkinGang gang);
};