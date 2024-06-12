#include "Scorch.h"

#include "Pullover.h"
#include "Vehicles.h"
#include "Log.h"
#include "Mod.h"
#include "CleoFunctions.h"
#include "SoundSystem.h"
#include "Locations.h"

std::vector<ScorchPedData*> Scorch::m_ScorchingPeds;

std::vector<Vehicle*> Scorch::m_TowTrucks;

void Scorch::Update(int dt)
{
    UpdateTowTrucks(dt);
    UpdateScorchingPeds(dt);
}

void Scorch::UpdateScorchingPeds(int dt)
{
    std::vector<ScorchPedData*> toRemove;

    for(auto scorchData : m_ScorchingPeds)
    {
        if(scorchData->followingPlayer)
        {
            if(!Mod::IsActorAliveAndDefined(scorchData->ped->hPed))
            {
                Log::Level(LOG_LEVEL::LOG_BOTH) << "Scorching ped became undefined or is now dead" << std::endl;

                DestroyScorchData(scorchData);
                toRemove.push_back(scorchData);
                continue;
            }

            auto dpLocation = scorchData->toDp;
            auto pedPos = Mod::GetPedPosition(scorchData->ped->hPed);
            auto distance = DistanceBetweenPoints(dpLocation.position, pedPos);

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
                Log::Level(LOG_LEVEL::LOG_BOTH) << "Ped is no longer alive, or is not defined, leaving" << std::endl;

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
                Log::Level(LOG_LEVEL::LOG_BOTH) << "waiting for ped to enter" << std::endl;

                vehicle->actionStatus = ACTION_STATUS::SCORCH_WAITING_FOR_PED_TO_ENTER;
                ped->shouldHandsup = false;

                CleoFunctions::REMOVE_REFERENCES_TO_ACTOR(ped->hPed);
                CleoFunctions::CLEAR_ACTOR_TASK(ped->hPed);

                CleoFunctions::ACTOR_ENTER_CAR_PASSENGER_SEAT(ped->hPed, vehicle->hVehicle, 10000, 1);

                Log::Level(LOG_LEVEL::LOG_BOTH) << "ped is entering" << std::endl;

                continue;
            }
            continue;
        }

        if(vehicle->actionStatus == ACTION_STATUS::SCORCH_WAITING_FOR_PED_TO_ENTER)
        {
            if(CleoFunctions::IS_CHAR_IN_ANY_CAR(ped->hPed))
            {
                Log::Level(LOG_LEVEL::LOG_BOTH) << "Ped is in the car, leaving" << std::endl;

                ped->RemoveBlip();
                vehicle->SetDriverAndPassengersOwners();
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

void Scorch::UpdateTowTrucks(int dt)
{
    std::vector<Vehicle*> toRemove;

    for(auto towTruck : m_TowTrucks)
    {
        if(towTruck->actionStatus == ACTION_STATUS::TOWING_GOING_TO_CAR)
        {
            auto towTruckPosition = Mod::GetCarPosition(towTruck->hVehicle);
            auto destination = towTruck->drivingTo;
            auto distance = DistanceBetweenPoints(towTruckPosition, destination);

            if(distance < 10)
            {
                CleoFunctions::PUT_TRAILER_ON_CAB(towTruck->towingVehicle, towTruck->hVehicle);

                towTruck->MakePedsEnterVehicleAndLeaveScene();

                CleoFunctions::FADE(100, 0);
                CleoFunctions::WAIT(100, []() {
                    CleoFunctions::FADE(1000, 1);
                });

                toRemove.push_back(towTruck);
            }
        }
    }

    for(auto item : toRemove)
    {
        auto it = std::find(m_TowTrucks.begin(), m_TowTrucks.end(), item);
        m_TowTrucks.erase(it);
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
    
    Log::Level(LOG_LEVEL::LOG_BOTH) << "Conduzir para a DP" << std::endl;

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

    Log::Level(LOG_LEVEL::LOG_BOTH) << "playerGroup = " << playerGroup << std::endl;

    auto playerActor = CleoFunctions::GET_PLAYER_ACTOR(0);

    /*
    CleoFunctions::PUT_ACTOR_IN_GROUP_AS_LEADER(playerActor, ped->hPed);
    CleoFunctions::PUT_ACTOR_IN_GROUP(ped->hPed, playerGroup);
    */
    CleoFunctions::PUT_ACTOR_IN_GROUP_AS_LEADER(ped->hPed, playerActor);
    CleoFunctions::PUT_ACTOR_IN_GROUP(playerGroup, ped->hPed);

    Log::Level(LOG_LEVEL::LOG_BOTH) << "create sphere" << std::endl;

    scorchData->toDp = Locations::GetClosestPoliceDepartment();
    CVector dpPosition = scorchData->toDp.position;

    scorchData->sphere = CleoFunctions::CREATE_SPHERE(dpPosition.x, dpPosition.y, dpPosition.z, 3.0);
    Log::Level(LOG_LEVEL::LOG_BOTH) << "sphere = " << scorchData->sphere << std::endl;

    scorchData->blip = CleoFunctions::CreateMarker(dpPosition.x, dpPosition.y, dpPosition.z, 0, 3, 3);
    Log::Level(LOG_LEVEL::LOG_BOTH) << "blip = " << scorchData->blip << std::endl;
}

void Scorch::CallVehicleToScorchPed(Ped* ped)
{
    Log::Level(LOG_LEVEL::LOG_BOTH) << "Call vehicle to scorch ped" << std::endl;

    SoundSystem::PlayHTAudio();
    SoundSystem::PlayStreamFromAudiosFolder("voices/REQUEST_CAR_TO_TRANSPORT_SUSPECT.wav", false);
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

    vehicle->SetDriverAndPassengersOwners();
    vehicle->actionStatus = ACTION_STATUS::SCORCH_GOING_TO_PED;

    float actorX = 0, actorY = 0, actorZ = 0;
    CleoFunctions::STORE_COORDS_FROM_ACTOR_WITH_OFFSET(ped->hPed, 0, 0, 0, &actorX, &actorY, &actorZ);

    float driveToX = 0, driveToY = 0, driveToZ = 0;
    CleoFunctions::GET_NEAREST_CAR_PATH_COORDS_FROM(actorX, actorY, actorZ, 2, &driveToX, &driveToY, &driveToZ);

    vehicle->drivingTo = CVector(driveToX, driveToY, driveToZ);

    Log::Level(LOG_LEVEL::LOG_BOTH) << "make car drive to coord" << std::endl;

    CleoFunctions::CAR_DRIVE_TO(car, driveToX, driveToY, driveToZ);
}

void Scorch::TeleportPedToPrision(Ped* ped)
{
    Log::Level(LOG_LEVEL::LOG_BOTH) << "TeleportPedToPrision" << std::endl;

    Pullover::m_PullingPed = NULL;
    Pullover::m_PullingVehicle = NULL;

    ped->arrested = true;

    Log::Level(LOG_LEVEL::LOG_BOTH) << "destroy actor" << std::endl;

    CleoFunctions::DESTROY_ACTOR(ped->hPed);

    Log::Level(LOG_LEVEL::LOG_BOTH) << "fade" << std::endl;

    CleoFunctions::FADE(1000, 0);
    CleoFunctions::WAIT(1000, []() {
        CleoFunctions::FADE(1000, 1);
    });
}

void Scorch::DestroyScorchData(ScorchPedData* data)
{
    data->ped->RemoveBlip();

    CleoFunctions::DESTROY_SPHERE(data->sphere);
    data->sphere = 0;

    CleoFunctions::DISABLE_MARKER(data->blip);
    data->blip = 0;
}

bool Scorch::IsPedBeeingScorched(int hPed)
{
    for(auto scorchData : m_ScorchingPeds)
    {
        if(scorchData->ped->hPed == hPed) return true;
    }
    return false;
}

void Scorch::CallTowTruckToVehicle(Vehicle* vehicle)
{
    Log::Level(LOG_LEVEL::LOG_BOTH) << "Call tow truck to vehicle" << std::endl;

    if(Pullover::m_PullingPed)
    {
        Pullover::m_PullingPed->hVehicleOwned = 0;
        Pullover::m_PullingPed = NULL;
        Pullover::m_PullingVehicle->hDriverOwner = 0;
        Pullover::m_PullingVehicle->hPassengersOwner.clear();
        Pullover::m_PullingVehicle = NULL;
    }

    auto playerActor = CleoFunctions::GET_PLAYER_ACTOR(0);

    float trySpawnCarX = 0, trySpawnCarY = 0, trySpawnCarZ = 0;
    CleoFunctions::STORE_COORDS_FROM_ACTOR_WITH_OFFSET(playerActor, 0, 100, 0, &trySpawnCarX, &trySpawnCarY, &trySpawnCarZ);

    float spawnCarX = 0, spawnCarY = 0, spawnCarZ = 0;
    CleoFunctions::GET_NEAREST_CAR_PATH_COORDS_FROM(trySpawnCarX, trySpawnCarY, trySpawnCarZ, 2, &spawnCarX, &spawnCarY, &spawnCarZ);
    
    int towtruck = CleoFunctions::CREATE_CAR_AT(525, spawnCarX, spawnCarY, spawnCarZ);
    
    auto towtruckVehicle = Vehicles::TryCreateVehicle(towtruck);
    towtruckVehicle->towingVehicle = vehicle->hVehicle;
    m_TowTrucks.push_back(towtruckVehicle);

    CleoFunctions::SET_CAR_ENGINE_OPERATION(towtruck, true);
    CleoFunctions::SET_CAR_MAX_SPEED(towtruck, 25.0f);
    CleoFunctions::SET_CAR_TRAFFIC_BEHAVIOUR(towtruck, 2);
    
    int blip = towtruckVehicle->AddBlip(2);

    int driver = CleoFunctions::CREATE_ACTOR_PEDTYPE_IN_CAR_DRIVERSEAT(towtruck, 23, 50);

    towtruckVehicle->SetDriverAndPassengersOwners();

    auto vehiclePosition = Mod::GetCarPosition(vehicle->hVehicle);

    float driveToX = 0, driveToY = 0, driveToZ = 0;
    CleoFunctions::GET_NEAREST_CAR_PATH_COORDS_FROM(vehiclePosition.x, vehiclePosition.y, vehiclePosition.z, 2, &driveToX, &driveToY, &driveToZ);

    towtruckVehicle->drivingTo = CVector(driveToX, driveToY, driveToZ);
    towtruckVehicle->actionStatus = ACTION_STATUS::TOWING_GOING_TO_CAR;

    Log::Level(LOG_LEVEL::LOG_BOTH) << "make tow truck drive to other vehicle" << std::endl;

    CleoFunctions::CAR_DRIVE_TO(towtruck, driveToX, driveToY, driveToZ);
}