#pragma once

#include "pch.h"

#include "Pickup.h"
#include "Ped.h"

struct Weapon {
    int gxtId;
    int weaponId;
    int weaponModelId;
};

struct PartnerSkin {
    int gxtId;
    int pedModelId;
};

class PoliceDepartment {
public:
    static Pickup* m_PickupMenu;
    static Pickup* m_PickupEquipment;
    static Pickup* m_PickupPartner;

    static int m_PartnerWeaponIndex;
    static int m_PartnerSkinIndex;
    static std::vector<Ped*> m_Partners;

    static std::vector<Weapon> m_Weapons;
    static std::vector<PartnerSkin> m_PartnerSkins;

    static void Init();
    static void Update(int dt);

    static Ped* SpawnPartner();
    static void RemoveAllPartners();
};