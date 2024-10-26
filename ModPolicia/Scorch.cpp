#include "Scorch.h"

#include "Pullover.h"
#include "Vehicles.h"
#include "Log.h"
#include "Mod.h"
#include "CleoFunctions.h"
#include "SoundSystem.h"
#include "Locations.h"
#include "Callouts.h"
#include "Trunk.h"
#include "Input.h"
#include "menu/IMenuVSL.h"

extern IMenuVSL* menuVSL;

std::vector<ScorchPedData*> Scorch::m_ScorchingPeds;

std::vector<Vehicle*> Scorch::m_TowTrucks;

IWindow* Scorch::m_CarryWindow = NULL;

Ped* Scorch::m_CarryingPed = NULL;
int Scorch::m_CarryObject = 0;

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
        if(!IsActorAliveAndDefined(scorchData->ped->hPed))
        {
            Log::Level(LOG_LEVEL::LOG_BOTH) << "Scorching ped became undefined or is now dead" << std::endl;

            DestroyScorchData(scorchData);
            toRemove.push_back(scorchData);
            continue;
        }

        //not beeing scorched by player
        auto vehicle = scorchData->vehicle;
        auto ped = scorchData->ped;
        if(!IsActorAliveAndDefined(ped->hPed))
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
            auto carPos = GetCarPosition(vehicle->hVehicle);
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
            auto towTruckPosition = GetCarPosition(towTruck->hVehicle);
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

void Scorch::CallVehicleToScorchPed(Ped* ped)
{
    Log::Level(LOG_LEVEL::LOG_BOTH) << "Call vehicle to scorch ped" << std::endl;

    SoundSystem::PlayHTAudio();
    SoundSystem::PlayStreamFromAudiosFolderWithRandomVariation("voices/REQUEST_CAR_TO_TRANSPORT_SUSPECT_", false);

    menuVSL->ShowMessage(GetLanguageLine("voice_request_scorch_policecar"), 3000);

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

    Callouts::UpdateCriminals(0);

    Log::Level(LOG_LEVEL::LOG_BOTH) << "fade" << std::endl;

    CleoFunctions::FADE(1000, 0);
    CleoFunctions::WAIT(1000, []() {
        CleoFunctions::FADE(1000, 1);
    });
}

void Scorch::DestroyScorchData(ScorchPedData* data)
{
    data->ped->RemoveBlip();
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

    auto vehiclePosition = GetCarPosition(vehicle->hVehicle);

    float driveToX = 0, driveToY = 0, driveToZ = 0;
    CleoFunctions::GET_NEAREST_CAR_PATH_COORDS_FROM(vehiclePosition.x, vehiclePosition.y, vehiclePosition.z, 2, &driveToX, &driveToY, &driveToZ);

    towtruckVehicle->drivingTo = CVector(driveToX, driveToY, driveToZ);
    towtruckVehicle->actionStatus = ACTION_STATUS::TOWING_GOING_TO_CAR;

    Log::Level(LOG_LEVEL::LOG_BOTH) << "make tow truck drive to other vehicle" << std::endl;

    CleoFunctions::CAR_DRIVE_TO(towtruck, driveToX, driveToY, driveToZ);
}

void Scorch::CarryPed(Ped* ped)
{
    Pullover::m_PullingPed = NULL;
    Pullover::m_PullingVehicle = NULL;

    m_CarryingPed = ped;

    ped->shouldHandsup = false;
    
    auto objectSpawnPosition = GetPedPositionWithOffset(ped->hPed, CVector(0, 0, 0));

    auto object = m_CarryObject = CleoFunctions::CREATE_OBJECT(2328, objectSpawnPosition.x, objectSpawnPosition.y, objectSpawnPosition.z);
    CleoFunctions::SET_OBJECT_VISIBILITY(object, false);
    CleoFunctions::SET_OBJECT_COLLISION_DETECTION(object, false);

    auto playerActor = GetPlayerActor();
    auto objectOffset = CVector(0, 0, 0);

    CleoFunctions::ATTACH_OBJECT_TO_ACTOR(object, playerActor, objectOffset.x, objectOffset.y, objectOffset.z, 0, 0, 0);

    CleoFunctions::ENABLE_ACTOR_COLLISION_DETECTION(ped->hPed, false);

    auto angleLimit = 0.0f; //or 360.0f
    auto actorOffset = CVector(0, 1, 0);
    CleoFunctions::PUT_ACTOR_INTO_TURRET_ON_OBJECT(ped->hPed, object, actorOffset.x, actorOffset.y, actorOffset.z, 0, angleLimit, 0);
}

void Scorch::StopCarringPed()
{
    CleoFunctions::ENABLE_ACTOR_COLLISION_DETECTION(m_CarryingPed->hPed, true);

    m_CarryingPed = NULL;   
    
    CleoFunctions::DESTROY_OBJECT(Scorch::m_CarryObject);
    Scorch::m_CarryObject = 0;
}

void Scorch::ToggleCarryWindow(bool enabled)
{
    if(!enabled)
    {
        if(m_CarryWindow)
        {
            m_CarryWindow->SetToBeRemoved();
            m_CarryWindow = NULL;
        }
        return;
    }

    auto window = m_CarryWindow = menuVSL->AddWindow();
    window->m_Title = "";
    window->m_Position = CVector2D(0, 0);

    auto screenSize = Input::GetScreenSize();
    float w = 200.0f;
    float x = screenSize.x/2 - w/2;
    float y = 80;

    auto stopCarry_button = window->AddButton(GetLanguageLine("stop_carry"), CRGBA(60, 130, 180));
    stopCarry_button->m_FitInWindow = false;
    stopCarry_button->m_Position = CVector2D(x, y);
    stopCarry_button->m_Size = CVector2D(200, 50);
    stopCarry_button->onClick = []() {
        StopCarringPed();
        ToggleCarryWindow(false);
    };
}