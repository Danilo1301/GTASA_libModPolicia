#include "Chase.h"

#include "CleoFunctions.h"
#include "Pullover.h"
#include "Widgets.h"
#include "Log.h"
#include "Vehicles.h"
#include "Mod.h"
#include "Peds.h"
#include "Callouts.h"
#include "SoundSystem.h"

#include "windows/WindowBackup.h"

const int BARRIER_MODEL = 1459;
const int SPIKES_MODEL = 2899;

Ped* Chase::m_ChasingPed = NULL;
std::vector<Barrier*> Chase::m_Barriers;
std::vector<BarrierModel> Chase::m_BarrierModels = {
    {BARRIER_MODEL, 596, 280}, //roadblock
    {SPIKES_MODEL, 528, 288}, //spikes
};

void Chase::Update(int dt)
{
    UpdateChase(dt);
    UpdateBarriers(dt);
}

void Chase::UpdateChase(int dt)
{
    auto chasingPed = m_ChasingPed;
    Vehicle* vehicle = NULL;

    bool log = false;

    if(log) Log::Level(LOG_LEVEL::LOG_BOTH) << "c1" << std::endl;

    if(chasingPed)
    {
        //end chase if criminals is 0 or if actor is not defined
        if(Callouts::GetCriminals().size() == 0 || !CleoFunctions::ACTOR_DEFINED(chasingPed->hPed)) {
            EndChase();
            chasingPed = NULL;
        }
    }

    if(log) Log::Level(LOG_LEVEL::LOG_BOTH) << "c2" << std::endl;

    //make passengers leave after driver also leaves, so it fixes the bug
    if(chasingPed)
    {
        if(chasingPed->justLeftTheCar)
        {
            vehicle = Vehicles::GetVehicleByHandle(chasingPed->hVehicleOwned);
            vehicle->MakePedsExitCar();

            vehicle->RemoveBlip();
        }
    }
    
    if(log) Log::Level(LOG_LEVEL::LOG_BOTH) << "c3" << std::endl;

    for(auto criminal : Callouts::GetCriminals())
    {
        bool isCriminalInCar = CleoFunctions::IS_CHAR_IN_ANY_CAR(criminal->hPed);

        //Log::Level(LOG_LEVEL::LOG_UPDATE) << "Criminal " << criminal->hPed << " isInCar: " << (isCriminalInCar ? "T" : "F") << std::endl;

        //make criminal in car passenger shoot cops
        if(isCriminalInCar && criminal->willShootAtCops)
        {
            auto criminalCarHandle = CleoFunctions::ACTOR_USED_CAR(criminal->hPed);
            bool isDriver = criminal->hPed == CleoFunctions::GET_DRIVER_OF_CAR(criminalCarHandle);
            
            if(!isDriver)
            {
                auto criminalPosition = Mod::GetPedPosition(criminal->hPed);
                auto closestCop = Backup::FindClosestCop(criminalPosition, 30.0f, true);

                if(closestCop > 0)
                {
                    if(criminal->shootingAtPed != closestCop)
                    {
                        criminal->shootingAtPed = closestCop;

                        Log::Level(LOG_LEVEL::LOG_BOTH) << "Criminal passenger " << criminal->hPed << " is now shooting at cop: " << closestCop << std::endl;

                        CleoFunctions::ACTOR_DRIVEBY(criminal->hPed, closestCop, -1, 0, 0, 0, 100.0f, 4, true, 90);
                    }
                } else {
                    criminal->shootingAtPed = 0;
                }
            }
        }

        //reset
        if(criminal->justLeftTheCar)
        {
            Log::Level(LOG_LEVEL::LOG_BOTH) << "Reset criminal shootingAtPed" << std::endl;
            criminal->shootingAtPed = 0;
        }

        //if criminal just left the car and wont shoot cops
        if(criminal->justLeftTheCar && !criminal->willShootAtCops)
        {   
            int playerActor = CleoFunctions::GET_PLAYER_ACTOR(0);

            Log::Level(LOG_LEVEL::LOG_BOTH) << "Criminal passenger " << criminal->hPed << " is running away from player" << std::endl;

            CleoFunctions::FLEE_FROM_ACTOR(criminal->hPed, playerActor, 50.0f, -1);
        }

        //if criminal is on foot and will shot at cops
        if(!isCriminalInCar && criminal->willShootAtCops)
        {
            auto criminalPosition = Mod::GetPedPosition(criminal->hPed);
            auto closestCop = Backup::FindClosestCop(criminalPosition, 30.0f, true);

            if(closestCop > 0)
            {
                if(criminal->shootingAtPed != closestCop)
                {
                    criminal->shootingAtPed = closestCop;

                    Log::Level(LOG_LEVEL::LOG_BOTH) << "Criminal on foot " << criminal->hPed << " is now killing cop: " << closestCop << std::endl;

                    CleoFunctions::KILL_ACTOR(criminal->hPed, closestCop);
                }
            } else {
                criminal->shootingAtPed = 0;
            }
        }
    }

    if(log) Log::Level(LOG_LEVEL::LOG_BOTH) << "c4" << std::endl;
}

void Chase::UpdateBarriers(int dt)
{
    std::vector<Barrier*> barriersToRemove;

    for(auto barrier : m_Barriers)
    {
        if(barrier->object == 0) continue;

        //spikestrips
        if(barrier->objectId == SPIKES_MODEL)
        {
            auto vehicles = Vehicles::GetAllCarsInSphere(barrier->objectPosition, 4.0f);
            for(auto vehicle : vehicles)
            {
                DeflateCarTires(vehicle->hVehicle);

                if(Chase::m_ChasingPed)
                {
                    if(vehicle->hVehicle == Chase::m_ChasingPed->hVehicleOwned)
                    {
                        CleoFunctions::DESTROY_OBJECT(barrier->object);
                        barrier->object = 0;
                        break;
                    }
                }
            }
        }

        //roadblocks
        if(barrier->objectId == BARRIER_MODEL)
        {
            if(Chase::m_ChasingPed)
            {
                auto vehicle = Vehicles::GetVehicleByHandle(Chase::m_ChasingPed->hVehicleOwned);

                if(vehicle)
                {
                    auto vehiclePosition = Mod::GetCarPosition(vehicle->hVehicle);
                    auto distanceFromBarrier = DistanceBetweenPoints(vehiclePosition, barrier->objectPosition);

                    if(distanceFromBarrier < 10.0f)
                    {
                        if(vehicle->hasJustCrashed)
                        {
                            barrier->suspectCrashedInBarrier = true;

                            Log::Level(LOG_LEVEL::LOG_BOTH) << "Suspect's vehicle just hit the barrier!" << std::endl;

                            SoundSystem::PlayStreamFromAudiosFolderWithRandomVariation("roadblocks/ROADBLOCK_HIT_", false);
                        }
                    }

                    if(distanceFromBarrier < 10.0f)
                    {
                        barrier->suspectHasGottenClose = true;
                    }

                    if(distanceFromBarrier > 10.0f)
                    {
                        if(barrier->suspectHasGottenClose && !barrier->suspectCrashedInBarrier)
                        {
                            if(!barrier->suspectHasEvaded)
                            {
                                barrier->suspectHasEvaded = true;

                                Log::Level(LOG_LEVEL::LOG_BOTH) << "Suspect's vehicle just evaded the barrier!" << std::endl;

                                SoundSystem::PlayStreamFromAudiosFolderWithRandomVariation("roadblocks/ROADBLOCK_BREACHED_", false);
                            }
                        }
                    }
                }
            }
        }

        //delete if too far away
        auto playerActor = CleoFunctions::GET_PLAYER_ACTOR(0);
        auto playerPosition = Mod::GetPedPosition(playerActor);
        auto distance = DistanceBetweenPoints(barrier->objectPosition, playerPosition);
        if(distance > 200.0f)
        {
            barriersToRemove.push_back(barrier);
        }
    }

    for(auto barrier : barriersToRemove)
    {
        if(CleoFunctions::ACTOR_DEFINED(barrier->ped1)) CleoFunctions::DESTROY_ACTOR(barrier->ped1);
        if(CleoFunctions::ACTOR_DEFINED(barrier->ped2)) CleoFunctions::DESTROY_ACTOR(barrier->ped2);
        if(CleoFunctions::CAR_DEFINED(barrier->car)) CleoFunctions::DESTROY_CAR(barrier->car);
        if(barrier->object > 0) CleoFunctions::DESTROY_OBJECT(barrier->object);
        CleoFunctions::DISABLE_MARKER(barrier->marker);

        auto it = std::find(m_Barriers.begin(), m_Barriers.end(), barrier); 
        m_Barriers.erase(it);
        delete barrier;
    }
}

void Chase::MakeCarStartRunning(Vehicle* vehicle, Ped* ped)
{
    Log::Level(LOG_LEVEL::LOG_BOTH) << "MakeCarStartRunning" << std::endl;

    ped->willShootAtCops = Mod::CalculateProbability(0.3f);
    //ped->willShootAtCops = Mod::CalculateProbability(0.0f);
    if(vehicle->HasGuns() || ped->HasGuns()) ped->willShootAtCops = true;

    //CleoFunctions::SHOW_TEXT_3NUMBERS("MPFX1", ped->willShootAtCops ? 1 : 0, 0, 0, 3000, 1);

    if(ped->willShootAtCops)
    {
        CleoFunctions::GIVE_ACTOR_WEAPON(ped->hPed, 22, 10000);

        auto passengersHandle = vehicle->GetPassengers();
        for(auto passengerHandle : passengersHandle)
        {
            auto passenger = Peds::TryCreatePed(passengerHandle);
            passenger->willShootAtCops = ped->willShootAtCops;

            CleoFunctions::GIVE_ACTOR_WEAPON(passengerHandle, 22, 10000);
        }
    }

    /*
    00AE: set_car 3@ traffic_behaviour_to 2
    00AD: set_car 3@ max_speed_to 50.0             
    00A8: set_car 3@ to_psycho_driver
    */

    auto vehiclePosition = Mod::GetCarPosition(vehicle->hVehicle);
    auto vehicleAngle = CleoFunctions::GET_CAR_Z_ANGLE(vehicle->hVehicle);

    CleoFunctions::SET_CAR_TRAFFIC_BEHAVIOUR(vehicle->hVehicle, 2);
    CleoFunctions::SET_CAR_MAX_SPEED(vehicle->hVehicle, 30.0f);
    CleoFunctions::SET_CAR_TO_PSYCHO_DRIVER(vehicle->hVehicle);

    CleoFunctions::PUT_CAR_AT(vehicle->hVehicle, vehiclePosition.x, vehiclePosition.y, vehiclePosition.z);
    CleoFunctions::SET_CAR_Z_ANGLE(vehicle->hVehicle, vehicleAngle);

    m_ChasingPed = ped;

    //add criminals
    auto owners = vehicle->GetOwners();
    for(auto owner : owners)
    {
        auto pedOwner = Peds::TryCreatePed(owner);
        Callouts::AddPedToCriminalList(pedOwner);
    }
}

void Chase::EndChase()
{
    Log::Level(LOG_LEVEL::LOG_BOTH) << "end chase" << std::endl;

    Callouts::RemovePedFromCriminalList(m_ChasingPed);

    m_ChasingPed = NULL;
}

Barrier* Chase::AddBarrier(CVector position, int objectId, int carModelId, int pedModelId)
{
    Barrier* barrier = new Barrier();
    barrier->objectId = objectId;
    m_Barriers.push_back(barrier);

    auto playerActor = CleoFunctions::GET_PLAYER_ACTOR(0);

    float x = 0, y = 0, z = 0;
    CleoFunctions::GET_NEAREST_CAR_PATH_COORDS_FROM(position.x, position.y, position.z, 2, &x, &y, &z);

    //0395: clear_area 1 at 4@ 5@ 6@ radius 6.0

    auto car = barrier->car = CleoFunctions::CREATE_CAR_AT(carModelId, x, y, z);

    auto angle = CleoFunctions::ACTOR_Z_ANGLE(playerActor);

    CleoFunctions::SET_CAR_Z_ANGLE(car, angle);

    CleoFunctions::STORE_COORDS_FROM_CAR_WITH_OFFSET(car, 4.0, 0, 0, &x, &y, &z);

    CleoFunctions::PUT_CAR_AT(car, x, y, z);

    angle = CleoFunctions::ACTOR_Z_ANGLE(playerActor);
    angle += 65.0f;

    CleoFunctions::SET_CAR_Z_ANGLE(car, angle);

    //397: enable_car $CAR1 siren 1        
    //067F: set_car 2@ lights 2

    CleoFunctions::STORE_COORDS_FROM_CAR_WITH_OFFSET(car, 0, 4.0, 0, &x, &y, &z);

    auto ground = CleoFunctions::GROUND_Z_AT(x, y, z);

    auto object = barrier->object = CleoFunctions::CREATE_OBJECT(objectId, x, y, ground);
    barrier->objectPosition = CVector(x, y, ground);

    angle = CleoFunctions::ACTOR_Z_ANGLE(playerActor);

    if(objectId == SPIKES_MODEL) angle += 90.0;

    CleoFunctions::SET_OBJECT_Z_ANGLE(object, angle);
    
    CleoFunctions::STORE_COORDS_FROM_CAR_WITH_OFFSET(car, 2.7, 2.2, 0, &x, &y, &z);
    auto ped1 = barrier->ped1 = CleoFunctions::CREATE_ACTOR_PEDTYPE(4, pedModelId, x, y, z);

    CleoFunctions::STORE_COORDS_FROM_CAR_WITH_OFFSET(car, 2.7, -2.2, 0, &x, &y, &z);
    auto ped2 = barrier->ped2 = CleoFunctions::CREATE_ACTOR_PEDTYPE(4, pedModelId, x, y, z);

    auto marker = barrier->marker = CleoFunctions::CREATE_MARKER_AT(x, y, z, 2, 3);

    return barrier;
}

Barrier* Chase::AddBarrier(CVector position, BarrierModel* barrierModel)
{
    return AddBarrier(position, barrierModel->objectId, barrierModel->vehicleModelId, barrierModel->pedModelId);
}

void Chase::AddRoadBlocks(CVector position)
{
    AddBarrier(position, &m_BarrierModels[0]);
    
}

void Chase::AddSpikestrips(CVector position)
{
    AddBarrier(position, &m_BarrierModels[1]);
}

void Chase::DeflateCarTires(int hVehicle)
{
    //04FE: deflate_tire 0 on_car 0@
    CleoFunctions::DEFLATE_TIRE_ON_CAR(hVehicle, 0);
    CleoFunctions::DEFLATE_TIRE_ON_CAR(hVehicle, 1);
    CleoFunctions::DEFLATE_TIRE_ON_CAR(hVehicle, 2);
    CleoFunctions::DEFLATE_TIRE_ON_CAR(hVehicle, 3);
}