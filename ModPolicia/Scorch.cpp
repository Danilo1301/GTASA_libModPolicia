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
            continue;
        } 

        //not beeing scorched by player
        auto vehicle = scorchData->vehicle;
        auto ped = scorchData->ped;
        if(!Mod::IsActorAliveAndDefined(ped->hPed))
        {
            if(vehicle->actionStatus == ACTION_STATUS::SCORCH_GOING_TO_PED || vehicle->actionStatus == ACTION_STATUS::SCORCH_WAITING_FOR_PED_TO_ENTER)
            {
                Log::file << "Ped is no longer alive, or is not defined, leaving" << std::endl;

                ped->RemoveBlip();
                vehicle->MakePedsEnterVehicleAndLeaveScene();
                continue;
            }
        }
        if(vehicle->actionStatus == ACTION_STATUS::SCORCH_GOING_TO_PED)
        {
            auto carPos = Mod::GetCarPosition(vehicle->hVehicle);
            auto distance = DistanceBetweenPoints(carPos, vehicle->drivingTo);

            if(distance <= 8)
            {
                Log::file << "waiting for ped to enter" << std::endl;

                vehicle->actionStatus = ACTION_STATUS::SCORCH_WAITING_FOR_PED_TO_ENTER;
                ped->shouldHandsup = false;

                CleoFunctions::REMOVE_REFERENCES_TO_ACTOR(ped->hPed);

                CleoFunctions::ACTOR_ENTER_CAR_PASSENGER_SEAT(ped->hPed, vehicle->hVehicle, 10000, 1);

                Log::file << "ped is entering" << std::endl;

                continue;
            }
            continue;
        }
        if(vehicle->actionStatus == ACTION_STATUS::SCORCH_WAITING_FOR_PED_TO_ENTER)
        {
            if(CleoFunctions::IS_CHAR_IN_ANY_CAR(ped->hPed))
            {
                Log::file << "Ped is in the car, leaving" << std::endl;

                ped->RemoveBlip();
                vehicle->hPassengers.push_back(ped->hPed);
                vehicle->MakePedsEnterVehicleAndLeaveScene();
            }
            continue;
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

    CleoFunctions::SHOW_TEXT_3NUMBERS("MPFX85", 0, 0, 0, 2000, 1); //solicito viatura

    Pullover::m_PullingPed = NULL;
    Pullover::m_PullingVehicle = NULL;

    ScorchPedData* scorchData = new ScorchPedData();
    scorchData->ped = ped;
    m_ScorchingPeds.push_back(scorchData);

    if(ped->hVehicleOwned > 0) Vehicles::GetVehicleByHandle(ped->hVehicleOwned)->RemoveBlip();
    ped->hVehicleOwned = 0;

    auto playerActor = CleoFunctions::GET_PLAYER_ACTOR(0);

    float trySpawnCarX = 0, trySpawnCarY = 0, trySpawnCarZ = 0;
    CleoFunctions::STORE_COORDS_FROM_ACTOR_WITH_OFFSET(playerActor, 0, 100, 0, &trySpawnCarX, &trySpawnCarY, &trySpawnCarZ);

    float spawnCarX = 0, spawnCarY = 0, spawnCarZ = 0;
    CleoFunctions::GET_NEAREST_CAR_PATH_COORDS_FROM(trySpawnCarX, trySpawnCarY, trySpawnCarZ, 2, &spawnCarX, &spawnCarY, &spawnCarZ);
    
    int car = CleoFunctions::CREATE_CAR_AT(596, spawnCarX, spawnCarY, spawnCarZ);
    
    auto vehicle = Vehicles::TryCreateVehicle(car);
    
    scorchData->vehicle = vehicle;

    CleoFunctions::SET_CAR_ENGINE_OPERATION(car, true);
    CleoFunctions::ENABLE_CAR_SIREN(car, true);
    CleoFunctions::SET_CAR_DOOR_STATUS(car, 1);
    CleoFunctions::SET_CAR_MAX_SPEED(car, 25.0f);
    CleoFunctions::SET_CAR_TRAFFIC_BEHAVIOUR(car, 2);
  
    int blip = vehicle->AddBlip(2);

    int driver = CleoFunctions::CREATE_ACTOR_PEDTYPE_IN_CAR_DRIVERSEAT(car, 23, 280);

    vehicle->hDriver = driver;
    vehicle->actionStatus = ACTION_STATUS::SCORCH_GOING_TO_PED;

    float actorX = 0, actorY = 0, actorZ = 0;
    CleoFunctions::STORE_COORDS_FROM_ACTOR_WITH_OFFSET(ped->hPed, 0, 0, 0, &actorX, &actorY, &actorZ);

    float driveToX = 0, driveToY = 0, driveToZ = 0;
    CleoFunctions::GET_NEAREST_CAR_PATH_COORDS_FROM(actorX, actorY, actorZ, 2, &driveToX, &driveToY, &driveToZ);

    vehicle->drivingTo = CVector(driveToX, driveToY, driveToZ);

    Log::file << "make car drive to coord" << std::endl;

    CleoFunctions::CAR_DRIVE_TO(car, driveToX, driveToY, driveToZ);
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