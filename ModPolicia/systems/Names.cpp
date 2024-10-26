#include "Names.h"

#include "ModConfig.h"

std::vector<Name> Names::m_Names = {
    {"Corzeus Dias", SkinGenre::SKIN_MALE},
    {"Cleverson Ryan", SkinGenre::SKIN_MALE},
    {"Tamires Pereira", SkinGenre::SKIN_FEMALE}
};

std::map<int, std::string> Names::m_WeaponNames = {
    {22, "9mm"},
    {24, "Desert"},
    {25, "Shotgun"},
    {29, "MP5"},
    {31, "M4"}
};

std::map<int, std::string> Names::m_SkinNames = {
    {280, "LAPD"},
    {281, "SFPD"},
    {282, "LVPD"},
    {283, "Country Sheriff"},
    {284, "Motorbike"},
    {285, "SWAT"},
    {286, "FBI"},
    {287, "Army"},
    {288, "Desert Sheriff"}
};

void Names::Load()
{
    std::string namesFolder = ModConfig::GetConfigFolder() + "/data/names/";

    auto names = ReadNamesFile(namesFolder + "/male.txt");
    for(auto name : names) m_Names.push_back({name, SkinGenre::SKIN_MALE});

    names = ReadNamesFile(namesFolder + "/female.txt");
    for(auto name : names) m_Names.push_back({name, SkinGenre::SKIN_FEMALE});
}

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

std::string Names::GetWeaponName(int weaponId)
{
    if(m_WeaponNames.find(weaponId) == m_WeaponNames.end())
    {
        return std::to_string(weaponId);
    }
    return m_WeaponNames[weaponId];
}

std::string Names::GetSkinName(int skinId)
{
    if(m_SkinNames.find(skinId) == m_SkinNames.end())
    {
        return std::to_string(skinId);
    }
    return m_SkinNames[skinId];
}

std::string Names::GenerateRandomRG()
{
    auto digits1 = GetRandomNumber(10, 99);
    auto digits2 = GetRandomNumber(100, 999);
    auto digits3 = GetRandomNumber(100, 999);
    auto digits4 = GetRandomNumber(1, 9);

    std::stringstream str;
    str << digits1 << '.' << digits2 << '.' << digits3 << '-' << digits4;
    
    return str.str();
}

std::string Names::GenerateRandomCPF()
{
    auto digits1 = GetRandomNumber(100, 999);
    auto digits2 = GetRandomNumber(100, 999);
    auto digits3 = GetRandomNumber(100, 999);
    auto digits4 = GetRandomNumber(10, 99);

    std::stringstream str;
    str << digits1 << '.' << digits2 << '.' << digits3 << '-' << digits4;
    
    return str.str();
}

std::vector<std::string> Names::ReadNamesFile(std::string path)
{
    std::ifstream infile(path);

    std::vector<std::string> names;

    if (!infile.good()) {
        infile.close();
        return names;
    }

    std::string line;
    while (std::getline(infile, line))
    {
        //Log::file << "Line: " << line << std::endl;
        names.push_back(line);
    }

    infile.close();

    return names;
}