#include "Names.h"

std::vector<Name> Names::m_Names = {
    {194, SkinGenre::SKIN_MALE}, //custom
    {195, SkinGenre::SKIN_MALE}, //custom
    {196, SkinGenre::SKIN_MALE}, //custom
    {197, SkinGenre::SKIN_MALE},
    {198, SkinGenre::SKIN_MALE},
    {199, SkinGenre::SKIN_MALE},
    {200, SkinGenre::SKIN_MALE},
    {201, SkinGenre::SKIN_MALE},
    {202, SkinGenre::SKIN_MALE},
    {203, SkinGenre::SKIN_MALE},

    {204, SkinGenre::SKIN_FEMALE}, //custom
    {205, SkinGenre::SKIN_FEMALE},
    {206, SkinGenre::SKIN_FEMALE},
    {207, SkinGenre::SKIN_FEMALE},
    {208, SkinGenre::SKIN_FEMALE},
    {209, SkinGenre::SKIN_FEMALE},
    {210, SkinGenre::SKIN_FEMALE},
    {211, SkinGenre::SKIN_FEMALE},
    {212, SkinGenre::SKIN_FEMALE},
    {213, SkinGenre::SKIN_FEMALE}
};

Name Names::GetRandomName(SkinGenre genre)
{
    std::vector<Name> possibleNames;

    for(auto name : m_Names)
    {
        if(name.genre != genre) continue;

        possibleNames.push_back(name);
    }

    return possibleNames[GetRandomNumber(0, possibleNames.size() - 1)];
}