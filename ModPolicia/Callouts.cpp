#include "Callouts.h"

#include "Mod.h"
#include "CleoFunctions.h"
#include "Widgets.h"
#include "Peds.h"
#include "Log.h"
#include "Pullover.h"
#include "Chase.h"

int Callouts::m_TimeBetweenCallouts = 30000;
int Callouts::m_TimeToCallout = 0;
std::vector<Callout> Callouts::m_Callouts = {
    {CALLOUT_TYPE::CALLOUT_ASSAULT, 81}
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

                m_ModulatingCalloutIndex = Mod::GetRandomNumber(0, m_Callouts.size() - 1);

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

                StartAssaultCallout();
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

        if(criminal->beeingScorched)
        {
            Log::file << "Criminal is beeing arrested" << std::endl;
            removeCriminals.push_back(criminal);
            continue;
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
        }
    }
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