#include "PoliceDepartment.h"

#include "CleoFunctions.h"
#include "Peds.h"
#include "Locations.h"
#include "Mod.h"
#include "Vehicles.h"
#include "Trunk.h"
#include "Scorch.h"
#include "ModConfig.h"

#include "Log.h"

#include "iniconfig/INIFile.h"

#include "windows/WindowPD_Menu.h"

extern IMenuVSL* menuVSL;

std::vector<Base*> PoliceDepartment::m_Bases;

int PoliceDepartment::m_PartnerWeaponIndex = 0;
int PoliceDepartment::m_PartnerSkinIndex = 0;
std::vector<Ped*> PoliceDepartment::m_Partners;
 
int PoliceDepartment::m_PDMarker = 0;
int PoliceDepartment::m_PDSphere = 0;
CVector PoliceDepartment::m_SphereAndMarkerPosition = CVector(0, 0, 0);

std::vector<Weapon> PoliceDepartment::m_Weapons = {
    {22, 346}, //9mm
    {24, 348}, //desert
    {25, 349}, //shotgun
    {29, 353}, //mp5
    {31, 356}, //m4
};
std::vector<PartnerSkin> PoliceDepartment::m_PartnerSkins = {
    {280}, //LAPD
    {281}, //SFPD
    {282}, //LVPD
    {283}, //Country Sheriff
    {284}, //Motorbike
    {285}, //SWAT
    {286}, //FBI
    {287}, //Army
    {288}, //Desert Sheriff
};
std::vector<int> PoliceDepartment::m_VehicleIds = {
    596,
    597,
    598,
    599,
    523,
    528,
    490,
    601
};

int PoliceDepartment::m_MoneyReward = 350;

CVector PoliceDepartment::m_SpawnParterAtPosition = CVector(0, 0, 0);

int PoliceDepartment::m_PickupDutyId = 1210;
int PoliceDepartment::m_PickupPartnerId = 1314;
int PoliceDepartment::m_PickupEquipmentId = 1242;

void PoliceDepartment::Init()
{
    /*
    TEST BASES:

    //base ls
    Base* base = new Base();
    base->m_PickupDuty->position = CVector(253.4084, 68.5503, 1003.6406);
    m_Bases.push_back(base);

    //base pmesp
    Base* base2 = new Base();
    base2->m_PickupDuty->position = CVector(2550.25, -1791.64, 13.69);
    m_Bases.push_back(base2);
    */
}

void PoliceDepartment::Update(int dt)
{
    UpdateTrunkScorch(dt);

    for(auto base : m_Bases)
    {
        base->m_PickupDuty->Update(dt);
        base->m_PickupEquipment->Update(dt);
        base->m_PickupPartner->Update(dt);

        if(base->m_PickupDuty->playerJustPickedPickup)
        {
            WindowPD_Menu::Create();
        }

        if(base->m_PickupPartner->playerJustPickedPickup)
        {
            m_SpawnParterAtPosition = base->m_PickupPartner->position;
            WindowPD_Menu::CreatePartnerMenu();
        }
    }
}

void PoliceDepartment::UpdateTrunkScorch(int dt)
{
    auto playerActor = GetPlayerActor();
    auto playerPosition = GetPedPosition(playerActor);

    auto prevPDPosition = m_SphereAndMarkerPosition;
    auto closestBase = GetClosestBase();
    auto basePosition = closestBase->scorchSuspectPosition;
    auto distanceFromPD = DistanceBetweenPoints(playerPosition, basePosition);

    auto carryingPeds = false;
    auto vehicleHandle = GetVehiclePedIsUsing(playerActor);
    if(Vehicles::HasVehicleHandle(vehicleHandle))
    {
        auto vehicle = Vehicles::GetVehicleByHandle(vehicleHandle);
        carryingPeds = vehicle->trunk->pedsInTrunk.size() > 0;
    }

    //delect trunk peds if its close to pd
    if(distanceFromPD < 2.0f && carryingPeds)
    {
        auto vehicle = Vehicles::GetVehicleByHandle(vehicleHandle);

        auto peds = vehicle->trunk->pedsInTrunk;

        vehicle->trunk->RemoveAllPedsFromTrunk();

        for(auto pedHandle : peds)
        {
            auto ped = Peds::GetPedByHandle(pedHandle);
            ped->arrested = true;

            CleoFunctions::REMOVE_REFERENCES_TO_ACTOR(pedHandle);
            CleoFunctions::DESTROY_ACTOR(pedHandle);
        }

        //money
        int moneyGained = m_MoneyReward * peds.size();
        CleoFunctions::ADD_MONEY(0, moneyGained);

        menuVSL->ShowMessage(GetLanguageLine("reward", moneyGained), 3000);
    }
    
    //reset if closest dp changes
    if(DistanceBetweenPoints(prevPDPosition, basePosition) > 1.0f)
    {
        m_SphereAndMarkerPosition = basePosition;

        if(m_PDSphere != 0)
        {
            CleoFunctions::DESTROY_SPHERE(m_PDSphere);
            m_PDSphere = 0;
        }

        if(m_PDMarker != 0)
        {
            CleoFunctions::DISABLE_MARKER(m_PDMarker);
            m_PDMarker = 0;
        }
    }

    //sphere
    if(distanceFromPD < 50.0f && carryingPeds)
    {
        if(m_PDSphere == 0)
        {
            m_PDSphere = CleoFunctions::CREATE_SPHERE(basePosition.x, basePosition.y, basePosition.z, 2.0f);
        }
    }

    if(distanceFromPD > 50.0f || !carryingPeds)
    {
        if(m_PDSphere != 0)
        {
            CleoFunctions::DESTROY_SPHERE(m_PDSphere);
            m_PDSphere = 0;
        }
    }

    //marker
    if(carryingPeds && !Trunk::m_PreviewVehicle)
    {
        if(m_PDMarker == 0)
        {
            m_PDMarker = CleoFunctions::CreateMarker(basePosition.x, basePosition.y,basePosition.z, 0, 3, 3);
        }
    } else {
        if(m_PDMarker != 0)
        {
            CleoFunctions::DISABLE_MARKER(m_PDMarker);
            m_PDMarker = 0;
        }
    }
}   

Ped* PoliceDepartment::SpawnPartner()
{
    auto weapon = m_Weapons[m_PartnerWeaponIndex];
    auto skin = m_PartnerSkins[m_PartnerSkinIndex];
    auto spawnPosition = m_SpawnParterAtPosition;

    //type 8 = gang2
    auto hPed = CleoFunctions::CREATE_ACTOR_PEDTYPE(8, skin.pedModelId, spawnPosition.x, spawnPosition.y, spawnPosition.z);

    auto playerActor = CleoFunctions::GET_PLAYER_ACTOR(0);
    auto playerGroup = CleoFunctions::GET_PLAYER_GROUP(0);

    CleoFunctions::PUT_ACTOR_IN_GROUP_AS_LEADER(hPed, playerActor);
    CleoFunctions::PUT_ACTOR_IN_GROUP(playerGroup, hPed);

    CleoFunctions::GIVE_ACTOR_WEAPON(hPed, weapon.weaponId, 10000);
    CleoFunctions::SET_ACTOR_HEALTH(hPed, 500.0f);

    auto partner = Peds::TryCreatePed(hPed);
    
    m_Partners.push_back(partner);

    return partner;
}

void PoliceDepartment::RemoveAllPartners()
{
    for(auto partner : m_Partners)
    {
        if(!CleoFunctions::ACTOR_DEFINED(partner->hPed)) continue;

        CleoFunctions::DESTROY_ACTOR(partner->hPed);
    }
    m_Partners.clear();
}

bool PoliceDepartment::IsPedAPartner(Ped* ped)
{
    auto it = std::find(m_Partners.begin(), m_Partners.end(), ped);
    return it != m_Partners.end();
}

void PoliceDepartment::LoadBases()
{
    Log::Level(LOG_LEVEL::LOG_BOTH) << "PoliceDepartment: LoadBases" << std::endl;

    auto basesPath = ModConfig::GetConfigFolder() + "/data/bases/";

    auto files = ModConfig::GetFilesName(basesPath);
    for(auto file : files)
    {
        Log::Level(LOG_LEVEL::LOG_BOTH) << "Loading base: " << file << std::endl;

        auto filePath = basesPath + "/" + file;

        Base* base = new Base();

        base->m_PickupPartner->pickupModelId = m_PickupPartnerId;
        base->m_PickupDuty->pickupModelId = m_PickupDutyId;
        base->m_PickupEquipment->pickupModelId = m_PickupEquipmentId;

        INIFile iniFile;
        if (iniFile.Read(filePath))
        {   
            if(auto section = iniFile.GetFirstSection("BASE"))
            {
                base->name = section->GetString("name");
            }
            
            if(auto section = iniFile.GetFirstSection("PICKUP_DUTY"))
            {
                section->GetCVector("position", &base->m_PickupDuty->position);
            }

            if(auto section = iniFile.GetFirstSection("PICKUP_EQUIPMENT"))
            {
                section->GetCVector("position", &base->m_PickupEquipment->position);
            }

            if(auto section = iniFile.GetFirstSection("PICKUP_PARTNER"))
            {
                section->GetCVector("position", &base->m_PickupPartner->position);
            }

            if(auto section = iniFile.GetFirstSection("SPAWN_VEHICLE"))
            {
                section->GetCVector("position", &base->spawnVehiclePosition);
                section->GetFloat("angle", &base->spawnVehicleAngle);
            }

            if(auto section = iniFile.GetFirstSection("SCORCH_CRIMINAL"))
            {
                section->GetCVector("position", &base->scorchSuspectPosition);
            }

        } else {
            Log::Level(LOG_LEVEL::LOG_BOTH) << "ModConfig: Error reading " << file << std::endl;
        }

        m_Bases.push_back(base);
    }
}

Base* PoliceDepartment::GetClosestBase()
{
    Base* closest = m_Bases[0];
    double closestDistance = INFINITY;

    for(size_t i = 0; i < m_Bases.size(); i++)
    {
        auto base = m_Bases[i];

        auto playerActor = CleoFunctions::GET_PLAYER_ACTOR(0);

        float playerX = 0.0f, playerY = 0.0f, playerZ = 0.0f;
        CleoFunctions::GET_CHAR_COORDINATES(playerActor, &playerX, &playerY, &playerZ);

        auto distance = DistanceBetweenPoints(base->scorchSuspectPosition, CVector(playerX, playerY, playerZ));

        if(distance < closestDistance)
        {
            closestDistance = distance;
            closest = base;
        }
    }

    return closest;
}