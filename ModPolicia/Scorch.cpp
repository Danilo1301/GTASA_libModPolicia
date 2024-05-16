#include "Scorch.h"

#include "Pullover.h"
#include "Vehicles.h"
#include "Log.h"
#include "Mod.h"
#include "CleoFunctions.h"

std::vector<ScorchPedData*> Scorch::m_ScorchingPeds;

std::vector<CVector> Scorch::m_PoliceDepartmentPositions = {
    CVector(1536.1325, -1671.2093, 13.3828), //dp ls
    CVector(635.1006, -571.8163, 16.3359), //dp dillmore
    CVector(-1606.5406, 721.6789, 12.1554), //dp sf
    CVector(-211.8363, 978.0092, 19.3001), //dp fort carson
    CVector(2289.8784, 2425.8894, 10.8203) //dp lv
};

void Scorch::Update(int dt)
{
    std::vector<ScorchPedData*> toRemove;

    for(auto scorchData : m_ScorchingPeds)
    {
        if(scorchData->followingPlayer)
        {
            if(!Mod::IsActorAliveAndDefined(scorchData->ped->hPed))
            {
                Log::file << "Scorching ped became undefined or is now dead" << std::endl;

                DestroyScorchData(scorchData);
                toRemove.push_back(scorchData);
                continue;
            }

            auto dpPosition = m_PoliceDepartmentPositions[scorchData->toDpIndex];
            auto pedPos = Mod::GetPedPosition(scorchData->ped->hPed);
            auto distance = DistanceBetweenPoints(dpPosition, pedPos);

            if(distance < 3.0f)
            {
                /*
                01C2: remove_references_to_actor 4@ 
                009B: destroy_actor 4@ 
                03BD: destroy_sphere 7@
                */

                CleoFunctions::REMOVE_REFERENCES_TO_ACTOR(scorchData->ped->hPed);
                CleoFunctions::DESTROY_ACTOR(scorchData->ped->hPed);

                scorchData->ped->arrested = true;

                DestroyScorchData(scorchData);
                toRemove.push_back(scorchData);

                //m_PullingPed = NULL;
                //m_PullingVehicle = NULL;
            }
        }
    }

    for(auto item : toRemove)
    {
        auto it = std::find(m_ScorchingPeds.begin(), m_ScorchingPeds.end(), item);
        m_ScorchingPeds.erase(it);
    }
}

void Scorch::StartScorchingPed(Ped* ped)
{
    Pullover::m_PullingPed = NULL;
    Pullover::m_PullingVehicle = NULL;

    ScorchPedData* scorchData = new ScorchPedData();
    scorchData->ped = ped;
    scorchData->followingPlayer = true;
    m_ScorchingPeds.push_back(scorchData);

    if(ped->hVehicleOwned > 0)
    {
        Vehicles::GetVehicleByHandle(ped->hVehicleOwned)->RemoveBlip();
    }

    ped->hVehicleOwned = 0;
    
    Log::file << "Conduzir para a DP" << std::endl;

    /*
    01C2: remove_references_to_actor 4@

    07AF: 6@ = player $PLAYER_CHAR group 

    //062F: 6@ = create_group_type 0 //is this really encessary    
    0630: put_actor $PLAYER_ACTOR in_group 4@ as_leader 
    0631: put_actor 4@ in_group 6@
    */

    ped->shouldHandsup = false;

    CleoFunctions::REMOVE_REFERENCES_TO_ACTOR(ped->hPed);
        
    auto playerGroup = CleoFunctions::GET_PLAYER_GROUP(0);

    Log::file << "playerGroup = " << playerGroup << std::endl;

    auto playerActor = CleoFunctions::GET_PLAYER_ACTOR(0);

    /*
    CleoFunctions::PUT_ACTOR_IN_GROUP_AS_LEADER(playerActor, ped->hPed);
    CleoFunctions::PUT_ACTOR_IN_GROUP(ped->hPed, playerGroup);
    */
    CleoFunctions::PUT_ACTOR_IN_GROUP_AS_LEADER(ped->hPed, playerActor);
    CleoFunctions::PUT_ACTOR_IN_GROUP(playerGroup, ped->hPed);

    Log::file << "create sphere" << std::endl;

    scorchData->toDpIndex = GetClosestPoliceDepartment();
    CVector dpPosition = m_PoliceDepartmentPositions[scorchData->toDpIndex];

    scorchData->sphere = CleoFunctions::CREATE_SPHERE(dpPosition.x, dpPosition.y, dpPosition.z, 3.0);
    Log::file << "sphere = " << scorchData->sphere << std::endl;

    scorchData->blip = CleoFunctions::CreateMarker(dpPosition.x, dpPosition.y, dpPosition.z, 0, 3, 3);
    Log::file << "blip = " << scorchData->blip << std::endl;
}

void Scorch::CallVehicleToScorchPed(Ped* ped)
{
    Log::file << "Call vehicle to scorch ped" << std::endl;

}

void Scorch::TeleportPedToPrision(Ped* ped)
{
    Log::file << "TeleportPedToPrision" << std::endl;

    Pullover::m_PullingPed = NULL;
    Pullover::m_PullingVehicle = NULL;

    ped->arrested = true;

    Log::file << "destroy actor" << std::endl;

    CleoFunctions::DESTROY_ACTOR(ped->hPed);

    Log::file << "fade" << std::endl;

    CleoFunctions::FADE(500, 1);
}

void Scorch::DestroyScorchData(ScorchPedData* data)
{
    data->ped->RemoveBlip();

    CleoFunctions::DESTROY_SPHERE(data->sphere);
    data->sphere = 0;

    CleoFunctions::DISABLE_MARKER(data->blip);
    data->blip = 0;
}

int Scorch::GetClosestPoliceDepartment()
{
    int closest = -1;
    double closestDistance = INFINITY;

    for(size_t i = 0; i < m_PoliceDepartmentPositions.size(); i++)
    {
        auto playerActor = CleoFunctions::GET_PLAYER_ACTOR(0);
        float playerX = 0.0f, playerY = 0.0f, playerZ = 0.0f;
        CleoFunctions::GET_CHAR_COORDINATES(playerActor, &playerX, &playerY, &playerZ);

        auto dpPosition = m_PoliceDepartmentPositions[i];

        auto distance = DistanceBetweenPoints(dpPosition, CVector(playerX, playerY, playerZ));

        if(distance < closestDistance)
        {
            closestDistance = distance;
            closest = (int)i;
        }
    }

    return closest;
}

bool Scorch::IsPedBeeingScorched(int hPed)
{
    for(auto scorchData : m_ScorchingPeds)
    {
        if(scorchData->ped->hPed == hPed) return true;
    }
    return false;
}