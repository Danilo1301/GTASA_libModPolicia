#include "PoliceDepartment.h"

#include "CleoFunctions.h"
#include "Peds.h"
#include "Locations.h"
#include "Mod.h"
#include "Vehicles.h"
#include "Trunk.h"
#include "Scorch.h"

#include "windows/WindowPD_Menu.h"

Pickup* PoliceDepartment::m_PickupMenu = new Pickup(CVector(253.4084, 68.5503, 1003.6406), 1210);
Pickup* PoliceDepartment::m_PickupEquipment = new Pickup(CVector(253.7876, 76.5393, 1003.6406), 1242);
Pickup* PoliceDepartment::m_PickupPartner = new Pickup(CVector(1541.6024, -1657.0557, 13.5595), 1314);

int PoliceDepartment::m_PartnerWeaponIndex = 0;
int PoliceDepartment::m_PartnerSkinIndex = 0;
std::vector<Ped*> PoliceDepartment::m_Partners;
 
int PoliceDepartment::m_PDMarker = 0;
int PoliceDepartment::m_PDSphere = 0;
CVector PoliceDepartment::m_SphereAndMarkerPosition = CVector(0, 0, 0);

std::vector<Weapon> PoliceDepartment::m_Weapons = {
    {143, 22, 346}, //9mm
    {144, 24, 348}, //desert
    {145, 25, 349}, //shotgun
    {146, 29, 353}, //mp5
    {147, 31, 356}, //m4
};
std::vector<PartnerSkin> PoliceDepartment::m_PartnerSkins = {
    {150, 280}, //LAPD
    {151, 281}, //SFPD
    {152, 282}, //LVPD
    {153, 283}, //Country Sheriff
    {154, 284}, //Motorbike
    {155, 285}, //SWAT
    {156, 286}, //FBI
    {157, 287}, //Army
    {158, 288}, //Desert Sheriff
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

void PoliceDepartment::Init()
{
    
}

void PoliceDepartment::Update(int dt)
{
    UpdateTrunkScorch(dt);

    m_PickupMenu->Update(dt);
    m_PickupEquipment->Update(dt);
    m_PickupPartner->Update(dt);

    if(m_PickupMenu->playerJustPickedPickup)
    {
        WindowPD_Menu::Create();
    }

    if(m_PickupEquipment->playerJustPickedPickup)
    {
        
    }

    if(m_PickupPartner->playerJustPickedPickup)
    {
        WindowPD_Menu::CreatePartnerMenu();
    }
}

void PoliceDepartment::UpdateTrunkScorch(int dt)
{
    auto playerActor = Mod::GetPlayerActor();
    auto playerPosition = Mod::GetPedPosition(playerActor);

    auto prevPDPosition = m_SphereAndMarkerPosition;
    auto closestPD = Locations::GetClosestPoliceDepartment();
    auto distanceFromPD = DistanceBetweenPoints(playerPosition, closestPD.position);

    auto carryingPeds = false;
    auto vehicleHandle = Mod::GetVehiclePedIsUsing(playerActor);
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
        CleoFunctions::SHOW_TEXT_3NUMBERS("MPFX168", moneyGained, 0, 0, 3000, 1); //resultado
    }
    
    //reset if closest dp changes
    if(DistanceBetweenPoints(prevPDPosition, closestPD.position) > 1.0f)
    {
        m_SphereAndMarkerPosition = closestPD.position;

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
            m_PDSphere = CleoFunctions::CREATE_SPHERE(closestPD.position.x, closestPD.position.y, closestPD.position.z, 2.0f);
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
            m_PDMarker = CleoFunctions::CreateMarker(closestPD.position.x, closestPD.position.y, closestPD.position.z, 0, 3, 3);
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
    auto spawnPosition = m_PickupPartner->position;

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