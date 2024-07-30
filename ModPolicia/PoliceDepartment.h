#pragma once

#include "pch.h"

#include "systems/Base.h"
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
    static std::vector<Base*> m_Bases;

    static int m_PartnerWeaponIndex;
    static int m_PartnerSkinIndex;
    static std::vector<Ped*> m_Partners;

    static int m_PDMarker;
    static int m_PDSphere;
    static CVector m_SphereAndMarkerPosition;

    static std::vector<Weapon> m_Weapons;
    static std::vector<PartnerSkin> m_PartnerSkins;
    static std::vector<int> m_VehicleIds;

    static int m_MoneyReward;
    
	static CVector m_SpawnParterAtPosition;

    static int m_PickupDutyId;
    static int m_PickupPartnerId;
    static int m_PickupEquipmentId;

    static void Init();
    static void Update(int dt);
    static void UpdateTrunkScorch(int dt);

    static Ped* SpawnPartner();
    static void RemoveAllPartners();
    static bool IsPedAPartner(Ped* ped);

    static void LoadBases();
    static Base* GetClosestBase();
};