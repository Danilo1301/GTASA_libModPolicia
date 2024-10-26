#pragma once

#include "pch.h"

#include "Skins.h"

struct Name {
    std::string name;
    SkinGenre genre;
};

class Names {
public:
    static std::vector<Name> m_Names;
    
    static std::map<int, std::string> m_WeaponNames;
    static std::map<int, std::string> m_SkinNames;

    static void Load();
    
    static Name GetRandomName(SkinGenre genre);
    static std::string GetWeaponName(int weaponId);
    static std::string GetSkinName(int skinId);

    static std::string GenerateRandomRG();
    static std::string GenerateRandomCPF();

    static std::vector<std::string> ReadNamesFile(std::string path);
};