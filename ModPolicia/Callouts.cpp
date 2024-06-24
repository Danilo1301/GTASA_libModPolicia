#include "Callouts.h"

#include "Mod.h"
#include "CleoFunctions.h"
#include "Widgets.h"
#include "Peds.h"
#include "Log.h"
#include "Pullover.h"
#include "Chase.h"
#include "Scorch.h"
#include "Vehicles.h"
#include "SoundSystem.h"
#include "Locations.h"
#include "Backup.h"

float Callouts::CALLOUT_DISTANCE = 400.0f;
int Callouts::m_TimeBetweenCallouts = 50000;
int Callouts::m_TimeToCallout = 0;
std::vector<Callout> Callouts::m_Callouts = {
    {CALLOUT_TYPE::CALLOUT_ASSAULT,             81, 1.0f, "callouts/CALLOUT_ASSAULT.wav"},
    {CALLOUT_TYPE::CALLOUT_GANG_SHOTS_FIRED,    89, 1.0f, "callouts/CALLOUT_GANG_SHOTS_FIRED.wav"},
    {CALLOUT_TYPE::CALLOUT_STOLEN_VEHICLE,      97, 1.0f, "callouts/CALLOUT_STOLEN_VEHICLE.wav"},
    {CALLOUT_TYPE::CALLOUT_HOUSE_INVASION,      114, 1.0f, "callouts/CALLOUT_HOUSE_INVASION.wav"},
    {CALLOUT_TYPE::CALLOUT_CHASE,               160, 1.0f, "callouts/CALLOUT_CHASE.wav"}
};
CALLOUT_TYPE Callouts::m_CurrentCalloutIndex = CALLOUT_TYPE::CALLOUT_NONE;
CALLOUT_TYPE Callouts::m_ModulatingCalloutIndex = CALLOUT_TYPE::CALLOUT_NONE;
std::vector<Ped*> Callouts::m_Criminals;
bool Callouts::m_AproachingCallout = false;
bool Callouts::m_AbortedCallout = false;
bool Callouts::m_WaitingToPlayAcceptCalloutAudio = false;
CAudioStream* Callouts::m_ModulatingCalloutAudio = NULL;

std::vector<SkinData> Callouts::m_Skins = {
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

std::vector<int> Callouts::m_StolenVehicleIds = {
    400, 445, 461, 479, 507, 522
};
std::vector<int> Callouts::m_StolenTruckIds {
    413, 414, 440, 456, 482, 498, 499
};

void Callouts::Update(int dt)
{ 
    if(!IsModulatingCallout())
    {
        if(!Pullover::m_PullingPed && !Chase::m_ChasingPed && Mod::m_Enabled)
        {
            if(m_CurrentCalloutIndex == CALLOUT_TYPE::CALLOUT_NONE)
            {
                m_TimeToCallout += dt;
            }
            
            if(m_TimeToCallout >= m_TimeBetweenCallouts)
            {
                m_TimeToCallout = 0;

                m_ModulatingCalloutIndex = GetRandomCallout();

                auto callout = m_Callouts[m_ModulatingCalloutIndex];

                Log::Level(LOG_LEVEL::LOG_BOTH) << "Modulating callout " << m_ModulatingCalloutIndex << std::endl;

                char buffer[256];
                sprintf(buffer, "MPFX%i", callout.gxtId);
                CleoFunctions::SHOW_TEXT_3NUMBERS(buffer, 0, 0, 0, 5000, 1);

                SoundSystem::PlayHTAudio();
                m_ModulatingCalloutAudio = SoundSystem::PlayStreamFromAudiosFolder(callout.audio, false);
            }
        }
    } else {
        m_TimeToCallout += dt;
        if(m_TimeToCallout >= 5000)
        {
            m_TimeToCallout = 0;
            m_ModulatingCalloutIndex = CALLOUT_TYPE::CALLOUT_NONE;
        }
    }

    if(IsModulatingCallout())
    {
        if(m_CurrentCalloutIndex == -1)
        {
            if(Widgets::IsWidgetJustPressed(37)) //green button
            {
                m_CurrentCalloutIndex = m_ModulatingCalloutIndex;
                m_ModulatingCalloutIndex = CALLOUT_TYPE::CALLOUT_NONE;

                m_WaitingToPlayAcceptCalloutAudio = true;

                Log::Level(LOG_LEVEL::LOG_BOTH) << "Accepting callout " << m_CurrentCalloutIndex << std::endl;

                CleoFunctions::SHOW_TEXT_3NUMBERS("MPFX82", 0, 0, 0, 3000, 1);
                    
                m_AproachingCallout = true;
                Log::Level(LOG_LEVEL::LOG_BOTH) << "m_AproachingCallout = " << m_AproachingCallout << std::endl;

                if(m_CurrentCalloutIndex == CALLOUT_TYPE::CALLOUT_ASSAULT) StartAssaultCallout();
                else if(m_CurrentCalloutIndex == CALLOUT_TYPE::CALLOUT_GANG_SHOTS_FIRED) StartGangShotsFiredCallout();
                else if(m_CurrentCalloutIndex == CALLOUT_TYPE::CALLOUT_STOLEN_VEHICLE) StartStolenVehicleCallout();
                else if(m_CurrentCalloutIndex == CALLOUT_TYPE::CALLOUT_HOUSE_INVASION) StartHouseInvasionCallout();
                else if(m_CurrentCalloutIndex == CALLOUT_TYPE::CALLOUT_CHASE) StartChaseCallout();
            }
        }
    }

    if(m_ModulatingCalloutAudio != NULL)
    {
        auto state = m_ModulatingCalloutAudio->GetState();

        if(state != 1)
        {
            m_ModulatingCalloutAudio = NULL;

            if(m_WaitingToPlayAcceptCalloutAudio)
            {
                m_WaitingToPlayAcceptCalloutAudio = false;

                CleoFunctions::WAIT(500, []() {
                    SoundSystem::PlayStreamFromAudiosFolder("voices/ACCEPT_CALLOUT.wav", false);
                });
            }
        }
    }

    UpdateCriminals(dt);
}

void Callouts::UpdateCriminals(int dt)
{
    std::vector<Ped*> removeCriminals;
    for(auto criminal : m_Criminals)
    {   
        if(!CleoFunctions::ACTOR_DEFINED(criminal->hPed))
        {
            Log::Level(LOG_LEVEL::LOG_BOTH) << "Criminal is not defined anymore" << std::endl;
            removeCriminals.push_back(criminal);
            continue;
        }

        if(CleoFunctions::ACTOR_DEAD(criminal->hPed))
        {
            Log::Level(LOG_LEVEL::LOG_BOTH) << "Criminal is dead" << std::endl;
            criminal->RemoveBlip();
            removeCriminals.push_back(criminal);
            continue;
        }

        if(Scorch::IsPedBeeingScorched(criminal->hPed))
        {
            Log::Level(LOG_LEVEL::LOG_BOTH) << "Criminal is beeing scorched" << std::endl;
            removeCriminals.push_back(criminal);
            continue;
        }
    }

    //if(m_CurrentCalloutIndex == 0 || m_CurrentCalloutIndex == 1)
    if(m_CurrentCalloutIndex > 0)
    {
        auto playerActor = CleoFunctions::GET_PLAYER_ACTOR(0);
        if(CleoFunctions::ACTOR_DEAD(playerActor))
        {
            for(auto criminal : m_Criminals)
            {
                CleoFunctions::DESTROY_ACTOR(criminal->hPed);

                removeCriminals.push_back(criminal);
            }
        }
    }

    for(auto ped : removeCriminals)
    {
        auto it = std::find(m_Criminals.begin(), m_Criminals.end(), ped);
        m_Criminals.erase(it);
    }

    if(m_CurrentCalloutIndex != -1 && !m_AproachingCallout)
    {
        if(m_Criminals.size() == 0)
        {
            Log::Level(LOG_LEVEL::LOG_BOTH) << "Criminal number is 0, clearing callout" << std::endl;
            m_CurrentCalloutIndex = CALLOUT_TYPE::CALLOUT_NONE;

            CleoFunctions::SHOW_TEXT_3NUMBERS("MPFX90", 0, 0, 0, 3000, 1);
        }
    }
}

CALLOUT_TYPE Callouts::GetRandomCallout()
{
    std::vector<int> ids;
    for(int calloutIndex = 0; calloutIndex < m_Callouts.size(); calloutIndex++)
    {
        auto callout = m_Callouts[calloutIndex];
        int chance = (int)(callout.chance * 100.0f);
        for(int i = 0; i < chance; i++) {
            ids.push_back(calloutIndex);
        }
        Log::Level(LOG_LEVEL::LOG_BOTH) << "callout " << calloutIndex << " chance " << chance << " total " << ids.size() << std::endl;
    }
    
    int randomCalloutIndex = ids[Mod::GetRandomNumber(0, ids.size() - 1)];

    ids.clear();

    return (CALLOUT_TYPE)randomCalloutIndex;
}

void Callouts::AddPedToCriminalList(Ped* ped)
{
    Log::Level(LOG_LEVEL::LOG_BOTH) << "AddPedToCriminalList " << ped->hPed << std::endl;

    if(IsPedOnCriminalList(ped))
    {
        Log::Level(LOG_LEVEL::LOG_BOTH) << "Ped is already on criminal list" << std::endl;
        return;
    }

    m_Criminals.push_back(ped);
}

void Callouts::RemovePedFromCriminalList(Ped* ped)
{
    Log::Level(LOG_LEVEL::LOG_BOTH) << "RemovePedFromCriminalList " << ped->hPed << std::endl;

    if(!IsPedOnCriminalList(ped))
    {
        Log::Level(LOG_LEVEL::LOG_BOTH) << "Ped is not on criminal list" << std::endl;
        return;
    }

    auto it = std::find(m_Criminals.begin(), m_Criminals.end(), ped);
    m_Criminals.erase(it);
}

bool Callouts::IsPedOnCriminalList(Ped* ped)
{
    auto it = std::find(m_Criminals.begin(), m_Criminals.end(), ped);
    return it != m_Criminals.end();
}

std::vector<Ped*> Callouts::GetCriminals()
{
    return m_Criminals;
}

void Callouts::AbortCallout()
{
    Log::Level(LOG_LEVEL::LOG_BOTH) << "Callouts: AbortCallout" << std::endl;
    
    if(!m_AproachingCallout) return;

    m_AbortedCallout = true;
}

bool Callouts::IsModulatingCallout()
{
    return m_ModulatingCalloutIndex != -1;
}

bool Callouts::IsOnCallout()
{
    return m_CurrentCalloutIndex != -1;
}

Callout Callouts::GetCurrentCallout()
{
    if(IsModulatingCallout())
    {
        return m_Callouts[m_ModulatingCalloutIndex];
    }

    return m_Callouts[m_CurrentCalloutIndex];
}

void Callouts::StartAssaultCallout()
{
    auto playerActor = CleoFunctions::GET_PLAYER_ACTOR(0);

    AproachCalloutPedPath(100.0f, [] (CVector pedPathNodePosition) {
        auto criminalSkin = GetRandomSkin(SkinGenre::SKIN_MALE, SkinGang::GANG_NONE);
        int criminal = CleoFunctions::CREATE_ACTOR_PEDTYPE(4, criminalSkin.modelId, pedPathNodePosition.x, pedPathNodePosition.y, pedPathNodePosition.z);

        auto criminalPed = Peds::TryCreatePed(criminal);
        m_Criminals.push_back(criminalPed);

        criminalPed->AddBlip();

        CleoFunctions::SET_ACTOR_WEAPON_AND_AMMO(criminal, 4, 1);

        float findX = 0, findY = 0, findZ = 0;
        CleoFunctions::STORE_COORDS_FROM_ACTOR_WITH_OFFSET(criminal, -2.0f, -2.0f, 0.0f, &findX, &findY, &findZ);

        float pedX = 0, pedY = 0, pedZ = 0;
        CleoFunctions::STORE_PED_PATH_COORDS_CLOSEST_TO(findX, findY, findZ, &pedX, &pedY, &pedZ);

        auto pedSkin = GetRandomSkin(SkinGenre::SKIN_FEMALE, SkinGang::GANG_NONE);
        int ped = CleoFunctions::CREATE_ACTOR_PEDTYPE(5, pedSkin.modelId, pedX, pedY, pedZ);

        CleoFunctions::FLEE_FROM_ACTOR(ped, criminal, 1000.0f, -1);

        CleoFunctions::KILL_ACTOR(criminal, ped);

        //CleoFunctions::SET_ACTOR_WANTED_BY_POLICE(criminal, true); //this makes the criminal forget about the victim and chase the police nearby
    });
}

void Callouts::StartGangShotsFiredCallout()
{
    AproachCalloutPedPath(100.0f,[] (CVector pedPathNodePosition) {
        auto playerActor = CleoFunctions::GET_PLAYER_ACTOR(0);
        int gang = Mod::GetRandomNumber(1, 3);

        for(int i = 0; i < 7; i++)
        {
            auto criminalSkin = GetRandomSkin(SkinGenre::SKIN_MALE, (SkinGang)gang);
            auto criminal = SpawnPedInRandomPedPathLocation(4, criminalSkin.modelId, pedPathNodePosition, 10.0f);
            criminal->AddBlip();
            criminal->willShootAtCops = true;

            m_Criminals.push_back(criminal);

            CleoFunctions::SET_ACTOR_HEALTH(criminal->hPed, 300.0f);

            if(i < 4)
                CleoFunctions::GIVE_ACTOR_WEAPON(criminal->hPed, 22, 10000);

            CleoFunctions::KILL_ACTOR(criminal->hPed, playerActor);
        }
    });
}

void Callouts::StartStolenVehicleCallout()
{
    AproachCalloutCarPath(150.0f, [] (CVector calloutPosition) {

        std::vector<int> vehicleIds;
        for(auto id : m_StolenVehicleIds) vehicleIds.push_back(id);
        for(auto id : m_StolenTruckIds) vehicleIds.push_back(id);

        int vehicleModel = vehicleIds[Mod::GetRandomNumber(0, vehicleIds.size() -1)];
        
        Log::Level(LOG_LEVEL::LOG_BOTH) << "creating car " << vehicleModel << std::endl;

        int carHandle = CleoFunctions::CREATE_CAR_AT(vehicleModel, calloutPosition.x, calloutPosition.y, calloutPosition.z);

        auto vehicle = Vehicles::TryCreateVehicle(carHandle);
        vehicle->isStolen = true;
        vehicle->AddBlip();

        Log::Level(LOG_LEVEL::LOG_BOTH) << "create driver" << std::endl;

        auto pedSkin = GetRandomSkin(SkinGenre::SKIN_MALE, SkinGang::GANG_NONE);

        int driver = CleoFunctions::CREATE_ACTOR_PEDTYPE_IN_CAR_DRIVERSEAT(carHandle, 4, pedSkin.modelId);
        auto pedDriver = Peds::TryCreatePed(driver);
        pedDriver->AddBlip();
        
        m_Criminals.push_back(pedDriver);

        CleoFunctions::REMOVE_REFERENCES_TO_CAR(carHandle);
        CleoFunctions::SET_CAR_ENGINE_OPERATION(carHandle, true);

        Log::Level(LOG_LEVEL::LOG_BOTH) << "set to psycho" << std::endl;

        CleoFunctions::SET_CAR_TO_PSYCHO_DRIVER(carHandle);
    }); 
}

void Callouts::StartHouseInvasionCallout()
{
    auto house = Locations::GetRandomHouse();

    AproachCallout(house, 20.0f, [] (CVector calloutPosition) {

        auto playerActor = CleoFunctions::GET_PLAYER_ACTOR(0);

        std::vector<Ped*> criminals;

        auto criminalSkin = GetRandomSkin(SkinGenre::SKIN_MALE, SkinGang::GANG_NONE);

        int criminalAtDoor = CleoFunctions::CREATE_ACTOR_PEDTYPE(4, criminalSkin.modelId, calloutPosition.x, calloutPosition.y, calloutPosition.z);
        auto criminalPedAtDoor = Peds::TryCreatePed(criminalAtDoor);
        criminalPedAtDoor->AddBlip();
        criminals.push_back(criminalPedAtDoor);

        for(int i = 0; i < Mod::GetRandomNumber(0, 1); i++)
        {
            criminalSkin = GetRandomSkin(SkinGenre::SKIN_MALE, SkinGang::GANG_NONE);

            auto criminalPed = SpawnPedInRandomPedPathLocation(4, criminalSkin.modelId, calloutPosition, 5.0f);
            criminalPed->AddBlip();
            criminals.push_back(criminalPed);
        }

        for(auto criminal : criminals)
        {
            m_Criminals.push_back(criminal);

            CleoFunctions::FLEE_FROM_ACTOR(criminal->hPed, playerActor, 30.0f, -1);

            criminal->UpdateInventory();

            for(int i = 0; i < Mod::GetRandomNumber(1, 2); i++) criminal->inventory->AddItemToInventory(Item_Type::CELLPHONE);
            for(int i = 0; i < Mod::GetRandomNumber(1, 2); i++) criminal->inventory->AddItemToInventory(Item_Type::STOLEN_WALLET);
            for(int i = 0; i < Mod::GetRandomNumber(1, 2); i++) criminal->inventory->AddItemToInventory(Item_Type::STOLEN_WATCH);
        }
    }, []() {});
}

void Callouts::StartChaseCallout()
{
    AproachCalloutCarPath(100.0f, [] (CVector carPathNodePosition) {
        std::vector<int> vehicleIds;
        for(auto id : m_StolenVehicleIds) vehicleIds.push_back(id);
        for(auto id : m_StolenTruckIds) vehicleIds.push_back(id);

        auto vehicleModel = vehicleIds[Mod::GetRandomNumber(0, vehicleIds.size() -1)];
        
        Log::Level(LOG_LEVEL::LOG_BOTH) << "creating car " << vehicleModel << std::endl;

        auto carHandle = CleoFunctions::CREATE_CAR_AT(vehicleModel, carPathNodePosition.x, carPathNodePosition.y, carPathNodePosition.z);

        auto vehicle = Vehicles::TryCreateVehicle(carHandle);
        vehicle->isStolen = true;
        vehicle->AddBlip();

        Log::Level(LOG_LEVEL::LOG_BOTH) << "create driver" << std::endl;

        auto pedSkin = GetRandomSkin(SkinGenre::SKIN_MALE, SkinGang::GANG_NONE);

        auto hDriver = CleoFunctions::CREATE_ACTOR_PEDTYPE_IN_CAR_DRIVERSEAT(carHandle, 4, pedSkin.modelId);
        auto driver = Peds::TryCreatePed(hDriver);
        driver->AddBlip();

        vehicle->SetDriverAndPassengersOwners();
        
        //m_Criminals.push_back(driver);

        CleoFunctions::REMOVE_REFERENCES_TO_CAR(carHandle);
        CleoFunctions::SET_CAR_ENGINE_OPERATION(carHandle, true);

        Chase::MakeCarStartRunning(vehicle, driver);

        auto spawnBackupPosition = Mod::GetCarPositionWithOffset(carHandle, CVector(0, -5.0f, 0));

        Backup::SpawnBackupCar(&Backup::m_DataBackupVehicles[0], spawnBackupPosition);
    });
}

void Callouts::AproachCallout(CVector location, float aproachDistance, std::function<void(CVector)> onReachMarker, std::function<void()> onAbort)
{
    auto playerActor = CleoFunctions::GET_PLAYER_ACTOR(0);

    int marker = CleoFunctions::CreateMarker(location.x, location.y, location.z, 0, 3, 3);

    CleoFunctions::AddWaitForFunction([playerActor, location, aproachDistance] () {
        float playerX = 0.0f, playerY = 0.0f, playerZ = 0.0f;
        CleoFunctions::GET_CHAR_COORDINATES(playerActor, &playerX, &playerY, &playerZ);

        auto distance = DistanceBetweenPoints(CVector(playerX, playerY, playerZ), location);

        if(distance < aproachDistance) return true;

        if(m_AbortedCallout) return true;

        return false;
    }, [marker, location, playerActor, onReachMarker, onAbort] () {

        CleoFunctions::DISABLE_MARKER(marker);

        m_AproachingCallout = false;
        Log::Level(LOG_LEVEL::LOG_BOTH) << "m_AproachingCallout = " << m_AproachingCallout << std::endl;

        if(m_AbortedCallout)
        {
            m_AbortedCallout = false;

            onAbort();
        } else {
            onReachMarker(location);
        }
    }); 
}

void Callouts::AproachCalloutPedPath(float aproachDistance, std::function<void(CVector)> onReachMarker)
{
    auto playerActor = CleoFunctions::GET_PLAYER_ACTOR(0);

    float x = 0, y = 0, z = 0;
    CleoFunctions::STORE_COORDS_FROM_ACTOR_WITH_OFFSET(playerActor, -CALLOUT_DISTANCE, CALLOUT_DISTANCE, 0, &x, &y, &z);

    float nodeX = 0, nodeY = 0, nodeZ = 0;
    CleoFunctions::STORE_PED_PATH_COORDS_CLOSEST_TO(x, y, z, &nodeX, &nodeY, &nodeZ);
    CVector nodePosition = CVector(nodeX, nodeY, nodeZ);

    AproachCallout(nodePosition, aproachDistance, onReachMarker, []() {});
}

void Callouts::AproachCalloutCarPath(float aproachDistance, std::function<void(CVector)> onReachMarker)
{
    auto playerActor = CleoFunctions::GET_PLAYER_ACTOR(0);

    auto distX = Mod::GetRandomNumber(-CALLOUT_DISTANCE, CALLOUT_DISTANCE);

    float x = 0, y = 0, z = 0;
    CleoFunctions::STORE_COORDS_FROM_ACTOR_WITH_OFFSET(playerActor, (float)distX, CALLOUT_DISTANCE, 0, &x, &y, &z);

    float nodeX = 0, nodeY = 0, nodeZ = 0;
    CleoFunctions::GET_NEAREST_CAR_PATH_COORDS_FROM(x, y, z, 2, &nodeX, &nodeY, &nodeZ);
    CVector nodePosition = CVector(nodeX, nodeY, nodeZ);

    AproachCallout(nodePosition, aproachDistance, onReachMarker, []() {});
}

Ped* Callouts::SpawnPedInRandomPedPathLocation(int pedType, int modelId, CVector position, float radius)
{
    Log::Level(LOG_LEVEL::LOG_BOTH) << "SpawnPedInRandomPedPathLocation " << std::endl;

    Log::Level(LOG_LEVEL::LOG_BOTH) << "position.x = " << position.x << std::endl;
    Log::Level(LOG_LEVEL::LOG_BOTH) << "position.y = " << position.y << std::endl;
    Log::Level(LOG_LEVEL::LOG_BOTH) << "position.z = " << position.z << std::endl;

    CVector offset = CVector(
      radius/2 + (float)(Mod::GetRandomNumber(0, (int)radius)),
      radius/2 + (float)(Mod::GetRandomNumber(0, (int)radius)),
      0
    );

    Log::Level(LOG_LEVEL::LOG_BOTH) << "offset.x = " << offset.x << std::endl;
    Log::Level(LOG_LEVEL::LOG_BOTH) << "offset.y = " << offset.y << std::endl;
    Log::Level(LOG_LEVEL::LOG_BOTH) << "offset.z = " << offset.z << std::endl;

    CVector tryPosition = position + offset;

    float nodeX = 0, nodeY = 0, nodeZ = 0;
    CleoFunctions::STORE_PED_PATH_COORDS_CLOSEST_TO(tryPosition.x, tryPosition.y, tryPosition.z, &nodeX, &nodeY, &nodeZ);

    int hPed = CleoFunctions::CREATE_ACTOR_PEDTYPE(pedType, modelId, nodeX, nodeY, nodeZ);

    return Peds::TryCreatePed(hPed);
}


SkinData Callouts::GetRandomSkin(SkinGenre genre, SkinGang gang)
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

    auto skin = possibleSkins[Mod::GetRandomNumber(0, possibleSkins.size() - 1)];

    Log::Level(LOG_LEVEL::LOG_BOTH) << "Skin chosed: " << skin.modelId << std::endl;

    return skin;
}

Ped* Callouts::GetClosestCriminal(CVector fromPosition)
{
    Ped* closestCriminal = NULL;
    double closestDistance = INFINITY;

    for(auto criminal : m_Criminals)
    {
        auto criminalPosition = Mod::GetPedPosition(criminal->hPed);
        auto distance = DistanceBetweenPoints(fromPosition, criminalPosition);

        if(distance < closestDistance)
        {
            closestDistance = distance;
            closestCriminal = criminal;
        }
    }

    return closestCriminal;
}