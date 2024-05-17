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

float Callouts::CALLOUT_DISTANCE = 50.0f;
int Callouts::m_TimeBetweenCallouts = 50000;
int Callouts::m_TimeToCallout = 0;
std::vector<Callout> Callouts::m_Callouts = {
    {CALLOUT_TYPE::CALLOUT_ASSAULT, 81, 0.0f},
    {CALLOUT_TYPE::GANG_SHOTS_FIRED, 89, 0.0f},
    {CALLOUT_TYPE::STOLEN_VEHICLE, 97, 1.0f},

};
CALLOUT_TYPE Callouts::m_CurrentCalloutIndex = CALLOUT_TYPE::CALLOUT_NONE;
CALLOUT_TYPE Callouts::m_ModulatingCalloutIndex = CALLOUT_TYPE::CALLOUT_NONE;
std::vector<Ped*> Callouts::m_Criminals;
bool Callouts::m_AproachingCallout = false;

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

void Callouts::Update(int dt)
{ 
    if(!IsModulatingCallout())
    {
        if(!Pullover::m_PullingPed && !Chase::m_ChasingPed)
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

                Log::file << "Modulating callout " << m_ModulatingCalloutIndex << std::endl;

                char buffer[256];
                sprintf(buffer, "MPFX%i", callout.gxtId);
                CleoFunctions::SHOW_TEXT_3NUMBERS(buffer, 0, 0, 0, 5000, 1);
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

                Log::file << "Accepting callout " << m_CurrentCalloutIndex << std::endl;

                CleoFunctions::SHOW_TEXT_3NUMBERS("MPFX82", 0, 0, 0, 3000, 1);
                    
                m_AproachingCallout = true;
                Log::file << "m_AproachingCallout = " << m_AproachingCallout << std::endl;

                if(m_CurrentCalloutIndex == CALLOUT_TYPE::CALLOUT_ASSAULT) StartAssaultCallout();
                else if(m_CurrentCalloutIndex == CALLOUT_TYPE::GANG_SHOTS_FIRED) StartGangShotsFiredCallout();
                else if(m_CurrentCalloutIndex == CALLOUT_TYPE::STOLEN_VEHICLE) StartStolenVehicleCallout();
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
            Log::file << "Criminal is not defined anymore" << std::endl;
            removeCriminals.push_back(criminal);
            continue;
        }

        if(CleoFunctions::ACTOR_DEAD(criminal->hPed))
        {
            Log::file << "Criminal is dead" << std::endl;
            criminal->RemoveBlip();
            removeCriminals.push_back(criminal);
            continue;
        }

        if(Scorch::IsPedBeeingScorched(criminal->hPed))
        {
            Log::file << "Criminal is beeing scorched" << std::endl;
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
            Log::file << "Criminal number is 0, clearing callout" << std::endl;
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
        Log::file << "callout " << calloutIndex << " chance " << chance << " total " << ids.size() << std::endl;
    }
    
    int randomCalloutIndex = ids[Mod::GetRandomNumber(0, ids.size() - 1)];

    ids.clear();

    return (CALLOUT_TYPE)randomCalloutIndex;
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

    AproachCallout([] (CVector pedPathNodePosition) {
        auto criminalSkin = GetRandomSkin(SkinGenre::SKIN_MALE, SkinGang::GANG_NONE);
        int criminal = CleoFunctions::CREATE_ACTOR_PEDTYPE(20, criminalSkin.modelId, pedPathNodePosition.x, pedPathNodePosition.y, pedPathNodePosition.z);

        auto criminalPed = Peds::TryCreatePed(criminal);
        m_Criminals.push_back(criminalPed);

        criminalPed->AddBlip();

        CleoFunctions::SET_ACTOR_WEAPON_AND_AMMO(criminal, 4, 1);

        float findX = 0, findY = 0, findZ = 0;
        CleoFunctions::STORE_COORDS_FROM_ACTOR_WITH_OFFSET(criminal, -2.0f, -2.0f, 0.0f, &findX, &findY, &findZ);

        float pedX = 0, pedY = 0, pedZ = 0;
        CleoFunctions::STORE_PED_PATH_COORDS_CLOSEST_TO(findX, findY, findZ, &pedX, &pedY, &pedZ);

        auto pedSkin = GetRandomSkin(SkinGenre::SKIN_FEMALE, SkinGang::GANG_NONE);
        int ped = CleoFunctions::CREATE_ACTOR_PEDTYPE(23, pedSkin.modelId, pedX, pedY, pedZ);

        CleoFunctions::FLEE_FROM_ACTOR(ped, criminal, 1000.0f, -1);

        CleoFunctions::KILL_ACTOR(criminal, ped);

        //CleoFunctions::SET_ACTOR_WANTED_BY_POLICE(criminal, true); //this makes the criminal forget about the victim and chase the police nearby
    });
}

void Callouts::StartGangShotsFiredCallout()
{
    AproachCallout([] (CVector pedPathNodePosition) {
        auto playerActor = CleoFunctions::GET_PLAYER_ACTOR(0);
        int gang = Mod::GetRandomNumber(1, 3);

        for(int i = 0; i < 5; i++)
        {
            auto criminalSkin = GetRandomSkin(SkinGenre::SKIN_MALE, (SkinGang)gang);
            auto criminal = SpawnPedInRandomPedPathLocation(20, criminalSkin.modelId, pedPathNodePosition, 10.0f);
            criminal->AddBlip();

            m_Criminals.push_back(criminal);

            if(i < 2)
                CleoFunctions::GIVE_ACTOR_WEAPON(criminal->hPed, 22, 10000);

            CleoFunctions::KILL_ACTOR(criminal->hPed, playerActor);
        }
    });
}

void Callouts::StartStolenVehicleCallout()
{
    AproachCallout([] (CVector calloutPosition) {

        int randomCar = CleoFunctions::GET_CAR_IN_SPHERE(calloutPosition.x, calloutPosition.y, calloutPosition.z, 200.0f, -1);

        if(randomCar > 0)
        {
            auto vehicle = Vehicles::TryCreateVehicle(randomCar);
            vehicle->isStolen = true;
            //vehicle->AddBlip();

            int driver = CleoFunctions::GET_DRIVER_OF_CAR(randomCar);

            if(driver > 0)
            {
                auto pedDriver = Peds::TryCreatePed(driver);
                pedDriver->AddBlip();
                
                m_Criminals.push_back(pedDriver);
            }
        }

        /*
        Log::file << "c1" << std::endl;

        float spawnX = 0, spawnY = 0, spawnZ = 0;
        CleoFunctions::GET_NEAREST_CAR_PATH_COORDS_FROM(calloutPosition.x, calloutPosition.y, calloutPosition.z, 2, &spawnX, &spawnY, &spawnZ);

        std::vector<int> vehicleModels = {445, 461, 479};
        int vehicleModel = vehicleModels[Mod::GetRandomNumber(0, vehicleModels.size() -1)];
        
        Log::file << "creating car " << vehicleModel << std::endl;

        int car = CleoFunctions::CREATE_CAR_AT(vehicleModel, spawnX, spawnY, spawnZ);

        CleoFunctions::REMOVE_REFERENCES_TO_CAR(car);

        auto vehicle = Vehicles::TryCreateVehicle(car);
        vehicle->isStolen = true;
        vehicle->AddBlip();

        auto pedSkin = GetRandomSkin(SkinGenre::SKIN_FEMALE, SkinGang::GANG_NONE);

        Log::file << "create driver" << std::endl;

        int driver = CleoFunctions::CREATE_ACTOR_PEDTYPE_IN_CAR_DRIVERSEAT(car, 20, pedSkin.modelId);

        CleoFunctions::SET_CAR_ENGINE_OPERATION(car, true);

        Log::file << "set to psycho" << std::endl;

        CleoFunctions::SET_CAR_TO_PSYCHO_DRIVER(car);
        */
    }); 
}

void Callouts::AproachCallout(std::function<void(CVector)> onReachMarker)
{
    auto playerActor = CleoFunctions::GET_PLAYER_ACTOR(0);

    float x = 0, y = 0, z = 0;
    CleoFunctions::STORE_COORDS_FROM_ACTOR_WITH_OFFSET(playerActor, 0, CALLOUT_DISTANCE, 0, &x, &y, &z);

    float nodeX = 0, nodeY = 0, nodeZ = 0;
    CleoFunctions::STORE_PED_PATH_COORDS_CLOSEST_TO(x, y, z, &nodeX, &nodeY, &nodeZ);
    CVector nodePosition = CVector(nodeX, nodeY, nodeZ);

    int marker = CleoFunctions::CreateMarker(nodeX, nodeY, nodeZ, 0, 3, 3);

    CleoFunctions::AddWaitForFunction([playerActor, nodePosition] () {
        float playerX = 0.0f, playerY = 0.0f, playerZ = 0.0f;
        CleoFunctions::GET_CHAR_COORDINATES(playerActor, &playerX, &playerY, &playerZ);

        auto distance = DistanceBetweenPoints(CVector(playerX, playerY, playerZ), nodePosition);

        if(distance < 100) return true;

        return false;
    }, [marker, playerActor, nodePosition, onReachMarker] () {
        CleoFunctions::DISABLE_MARKER(marker);

        m_AproachingCallout = false;
        Log::file << "m_AproachingCallout = " << m_AproachingCallout << std::endl;

        onReachMarker(nodePosition);
    }); 
}

Ped* Callouts::SpawnPedInRandomPedPathLocation(int pedType, int modelId, CVector position, float radius)
{
    Log::file << "SpawnPedInRandomPedPathLocation " << std::endl;

    Log::file << "position.x = " << position.x << std::endl;
    Log::file << "position.y = " << position.y << std::endl;
    Log::file << "position.z = " << position.z << std::endl;

    CVector offset = CVector(
      radius/2 + (float)(Mod::GetRandomNumber(0, (int)radius)),
      radius/2 + (float)(Mod::GetRandomNumber(0, (int)radius)),
      0
    );

    Log::file << "offset.x = " << offset.x << std::endl;
    Log::file << "offset.y = " << offset.y << std::endl;
    Log::file << "offset.z = " << offset.z << std::endl;

    CVector tryPosition = position + offset;

    float nodeX = 0, nodeY = 0, nodeZ = 0;
    CleoFunctions::STORE_PED_PATH_COORDS_CLOSEST_TO(tryPosition.x, tryPosition.y, tryPosition.z, &nodeX, &nodeY, &nodeZ);

    int hPed = CleoFunctions::CREATE_ACTOR_PEDTYPE(pedType, modelId, nodeX, nodeY, nodeZ);

    return Peds::TryCreatePed(hPed);
}


SkinData Callouts::GetRandomSkin(SkinGenre genre, SkinGang gang)
{
    std::vector<SkinData> possibleSkins;

    Log::file << "Get random skin genre " << genre << ", gang " << gang << std::endl;

    for(auto skin : m_Skins)
    {
        if(skin.gang != gang) continue;

        if(genre == SkinGenre::BOTH)
        {
            //ok
        } else {
            if(skin.genre != genre) continue;
        }

        Log::file << "Possible skin: " << skin.modelId << " genre " << skin.genre << ", gang " << skin.gang << std::endl;

        possibleSkins.push_back(skin);
    }

    auto skin = possibleSkins[Mod::GetRandomNumber(0, possibleSkins.size() - 1)];

    Log::file << "Skin chosed: " << skin.modelId << std::endl;

    return skin;
}