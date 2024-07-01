#include "Skins.h"

#include "Log.h"

std::vector<SkinData> Skins::m_Skins = {
    {19, SkinGenre::SKIN_MALE, SkinGang::GANG_NONE},
    {7, SkinGenre::SKIN_MALE, SkinGang::GANG_NONE},
    {29, SkinGenre::SKIN_MALE, SkinGang::GANG_NONE},
    {193, SkinGenre::SKIN_FEMALE, SkinGang::GANG_NONE},
    {12, SkinGenre::SKIN_FEMALE, SkinGang::GANG_NONE},
    {13, SkinGenre::SKIN_FEMALE, SkinGang::GANG_NONE},

    {102, SkinGenre::SKIN_MALE, SkinGang::GANG_BALLAS},
    {103, SkinGenre::SKIN_MALE, SkinGang::GANG_BALLAS},
    {104, SkinGenre::SKIN_MALE, SkinGang::GANG_BALLAS},

    {108, SkinGenre::SKIN_MALE, SkinGang::GANG_VAGOS},
    {109, SkinGenre::SKIN_MALE, SkinGang::GANG_VAGOS},
    {110, SkinGenre::SKIN_MALE, SkinGang::GANG_VAGOS},

    {114, SkinGenre::SKIN_MALE, SkinGang::GANG_AZTECAS},
    {115, SkinGenre::SKIN_MALE, SkinGang::GANG_AZTECAS},
    {116, SkinGenre::SKIN_MALE, SkinGang::GANG_AZTECAS}
};

SkinData Skins::GetRandomSkin(SkinGenre genre, SkinGang gang)
{
    std::vector<SkinData> possibleSkins;

    Log::Level(LOG_LEVEL::LOG_BOTH) << "Get random skin genre " << genre << ", gang " << gang << std::endl;

    for(auto skin : m_Skins)
    {
        if(skin.gang != gang) continue;

        if(genre == SkinGenre::BOTH)
        {
            //ok
        } else {
            if(skin.genre != genre) continue;
        }

        Log::Level(LOG_LEVEL::LOG_BOTH) << "Possible skin: " << skin.modelId << " genre " << skin.genre << ", gang " << skin.gang << std::endl;

        possibleSkins.push_back(skin);
    }

    auto skin = possibleSkins[GetRandomNumber(0, possibleSkins.size() - 1)];

    Log::Level(LOG_LEVEL::LOG_BOTH) << "Skin chosed: " << skin.modelId << std::endl;

    return skin;
}