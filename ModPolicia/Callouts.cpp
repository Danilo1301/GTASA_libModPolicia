#include "Callouts.h"

#include "Mod.h"
#include "CleoFunctions.h"
#include "Widgets.h"
#include "Peds.h"
#include "Log.h"
#include "Pullover.h"
#include "Chase.h"

int Callouts::m_TimeBetweenCallouts = 50000;
int Callouts::m_TimeToCallout = 0;
std::vector<Callout> Callouts::m_Callouts = {
    {CALLOUT_TYPE::CALLOUT_ASSAULT, 81, 1.0f},
    {CALLOUT_TYPE::GANG_SHOTS_FIRED, 89, 0.3f}
};
int Callouts::m_CurrentCalloutIndex = -1;
int Callouts::m_ModulatingCalloutIndex = -1;
std::vector<Ped*> Callouts::m_Criminals;
bool Callouts::m_AproachingCallout = false;

void Callouts::Update(int dt)
{ 
    if(!IsModulatingCallout())
    {
        if(!Pullover::m_PullingPed && !Chase::m_ChasingPed)
        {
            if(m_CurrentCalloutIndex == -1)
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
            m_ModulatingCalloutIndex = -1;
        }
    }

    if(IsModulatingCallout())
    {
        if(m_CurrentCalloutIndex == -1)
        {
            if(Widgets::IsWidgetJustPressed(37)) //green button
            {

                m_CurrentCalloutIndex = m_ModulatingCalloutIndex;
                m_ModulatingCalloutIndex = -1;

                Log::file << "Accepting callout " << m_CurrentCalloutIndex << std::endl;

                CleoFunctions::SHOW_TEXT_3NUMBERS("MPFX82", 0, 0, 0, 3000, 1);
                    
                m_AproachingCallout = true;
                Log::file << "m_AproachingCallout = " << m_AproachingCallout << std::endl;

                if(m_CurrentCalloutIndex == 0) StartAssaultCallout();
                else if(m_CurrentCalloutIndex == 1) StartGangShotsFiredCallout();
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

        if(criminal->scorchStatus == SCORCH_STATUS::BEEING_SCORCHED || criminal->scorchStatus == SCORCH_STATUS::WAITING_FOR_CAR)
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
            m_CurrentCalloutIndex = -1;

            CleoFunctions::SHOW_TEXT_3NUMBERS("MPFX90", 0, 0, 0, 3000, 1);
        }
    }
}

int Callouts::GetRandomCallout()
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

    return randomCalloutIndex;
}

bool Callouts::IsModulatingCallout()
{
    return m_ModulatingCalloutIndex != -1;
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

    float x = 0, y = 0, z = 0;
    CleoFunctions::STORE_COORDS_FROM_ACTOR_WITH_OFFSET(playerActor, 0, 200, 0, &x, &y, &z);

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
    }, [marker, nodePosition] () {
        CleoFunctions::DISABLE_MARKER(marker);

        m_AproachingCallout = false;
        Log::file << "m_AproachingCallout = " << m_AproachingCallout << std::endl;

        int criminal = CleoFunctions::CREATE_ACTOR_PEDTYPE(20, 19, nodePosition.x, nodePosition.y, nodePosition.z);

        auto criminalPed = Peds::TryCreatePed(criminal);
        m_Criminals.push_back(criminalPed);

        criminalPed->AddBlip();

        CleoFunctions::SET_ACTOR_WEAPON_AND_AMMO(criminal, 4, 1);

        float findX = 0, findY = 0, findZ = 0;
        CleoFunctions::STORE_COORDS_FROM_ACTOR_WITH_OFFSET(criminal, -2.0f, -2.0f, 0.0f, &findX, &findY, &findZ);

        float pedX = 0, pedY = 0, pedZ = 0;
        CleoFunctions::STORE_PED_PATH_COORDS_CLOSEST_TO(findX, findY, findZ, &pedX, &pedY, &pedZ);

        int ped = CleoFunctions::CREATE_ACTOR_PEDTYPE(23, 193, pedX, pedY, pedZ);

        CleoFunctions::FLEE_FROM_ACTOR(ped, criminal, 1000.0f, -1);

        CleoFunctions::KILL_ACTOR(criminal, ped);

        //CleoFunctions::SET_ACTOR_WANTED_BY_POLICE(criminal, true); //this makes the criminal forget about the victim and chase the police nearby
    }); 

    /*
    009A: 6@ = create_actor_pedtype 20 model #DNFYLC at 3@ 4@ 5@  //criminal
    0187: 8@ = create_marker_above_actor 6@ 
    
    04C4: store_coords_to 0@ 1@ 2@ from_actor 6@ with_offset -8.0 -7.0 0.0
    02C0: store_to 3@ 4@ 5@ ped_path_coords_closest_to 0@ 1@ 2@
    
    009A: 7@ = create_actor_pedtype 23 model #VWFYST1 at 3@ 4@ 5@ //ped
    
    05DD: AS_actor 7@ flee_from_actor 6@ from_origin_radius 1000.0 timelimit -1
    
    05E2: AS_actor 6@ kill_actor 7@ 
    09B6: set_actor 6@ wanted_by_police 1 
    */
}

void Callouts::StartGangShotsFiredCallout()
{
    auto playerActor = CleoFunctions::GET_PLAYER_ACTOR(0);

    float x = 0, y = 0, z = 0;
    CleoFunctions::STORE_COORDS_FROM_ACTOR_WITH_OFFSET(playerActor, 0, 300, 0, &x, &y, &z);

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
    }, [marker, playerActor, nodePosition] () {
        CleoFunctions::DISABLE_MARKER(marker);

        m_AproachingCallout = false;
        Log::file << "m_AproachingCallout = " << m_AproachingCallout << std::endl;

        for(int i = 0; i < 5; i++)
        {
            auto criminal = SpawnPedInRandomPedPathLocation(23, 102, nodePosition, 10.0f);
            criminal->AddBlip();

            m_Criminals.push_back(criminal);

            if(i < 2)
                CleoFunctions::GIVE_ACTOR_WEAPON(criminal->hPed, 22, 10000);

            CleoFunctions::KILL_ACTOR(criminal->hPed, playerActor);
        }
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