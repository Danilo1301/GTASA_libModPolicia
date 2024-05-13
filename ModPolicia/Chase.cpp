#include "Chase.h"

#include "CleoFunctions.h"
#include "Pullover.h"
#include "Widgets.h"
#include "Log.h"
#include "Vehicles.h"
#include "Mod.h"

#include "windows/WindowBackup.h"

Ped* Chase::m_ChasingPed = NULL;
std::vector<Vehicle*> Chase::m_BackupVehicles;

void Chase::Update(int dt)
{
    UpdateChase(dt);
    UpdateBackup(dt);
}

void Chase::UpdateChase(int dt)
{
    if(!m_ChasingPed) return;

    auto ped = m_ChasingPed;
    auto vehicle = Vehicles::GetVehicleByHandle(ped->hVehicleOwned);

    //car despawned
    if(!CleoFunctions::CAR_DEFINED(vehicle->hVehicle))
    {
        EndChase();
        return;
    }

    //ped left car
    if(!CleoFunctions::IS_CHAR_IN_ANY_CAR(ped->hPed))
    {
        ped->RemoveBlip();
        if(vehicle) vehicle->RemoveBlip();

        CleoFunctions::REMOVE_REFERENCES_TO_ACTOR(ped->hPed);

        //ped->shouldHandsup = true;
        EndChase();
        return;
    }

    int playerActor = CleoFunctions::GET_PLAYER_ACTOR(0);

    auto distance = Pullover::GetDistanceBetweenPedAndCar(playerActor, vehicle->hVehicle);

    if(distance > Pullover::PULLOVER_MIN_DISTANCE_VEHICLE) return;
    
    if(CleoFunctions::IS_CHAR_IN_ANY_CAR(playerActor)) return;
      
    Pullover::m_PullingVehicle = vehicle;
    Pullover::m_PullingPed = ped;
    Pullover::AskPedToLeaveCar(ped);

    EndChase();
}

void Chase::UpdateBackup(int dt)
{
    if(WindowBackup::m_Window)
    {
        if(!m_ChasingPed)
        {
            WindowBackup::Remove();
        }
    }

    std::vector<Vehicle*> removeBackup;
    for(auto vehicle : m_BackupVehicles)
    {
        if(vehicle->chaseStatus == CHASE_STATUS::CHASING)
        {
            if(!m_ChasingPed)
            {
                Log::file << "making backup stop chase and move away" << std::endl;

                vehicle->chaseStatus = CHASE_STATUS::MOVING_AWAY_FROM_CHASE;

                float findX = 0, findY = 0, findZ = 0;
                CleoFunctions::STORE_COORDS_FROM_CAR_WITH_OFFSET(vehicle->hVehicle, 0, 200, 0, &findX, &findY, &findZ);

                CleoFunctions::CAR_DRIVE_TO(vehicle->hVehicle, findX, findY, findZ);

                vehicle->fromPos = Mod::GetCarPosition(vehicle->hVehicle);
            }
            continue;
        }

        if(vehicle->chaseStatus == CHASE_STATUS::MOVING_AWAY_FROM_CHASE)
        {
            auto distance = DistanceBetweenPoints(vehicle->fromPos, Mod::GetCarPosition(vehicle->hVehicle));

            if(distance > 80)
            {
                Log::file << "destroying backup" << std::endl;

                vehicle->chaseStatus = CHASE_STATUS::NOT_CHASING;

                CleoFunctions::DESTROY_ACTOR(vehicle->hDriver);
                CleoFunctions::DESTROY_CAR(vehicle->hVehicle);

                removeBackup.push_back(vehicle);
            }
        }
    }

    for(auto vehicle : removeBackup)
    {
        auto it = std::find(m_BackupVehicles.begin(), m_BackupVehicles.end(), vehicle);
        m_BackupVehicles.erase(it);
    }

    if(!m_ChasingPed) return;

    if(WindowBackup::m_Window) return;
    
    if(Widgets::IsWidgetJustPressed(37)) //green button
    {
        WindowBackup::Create();
    }
}

void Chase::CallBackup(int vehicleModelId, int pedModelId)
{
    Log::file << "call backup vehicleModelId: " << vehicleModelId << ", pedModelId: " << pedModelId << std::endl;

    /*
    04D3: get_nearest_car_path_coords_from 0@ 1@ 2@ type 2 store_to 3@ 4@ 5@

    00A5: 6@ = create_car #COPCARLA at 3@ 4@ 5@
    0129: 10@ = create_actor_pedtype 23 model #LAPD1 in_car 6@ driverseat
            
    0397: enable_car 6@ siren 1
    00AD: set_car 6@ max_speed_to 50.0
    0423: set_car 6@ improved_handling_to 1.5
    00AE: set_car 6@ traffic_behaviour_to 2  

    07F8: car 6@ follow_car 8@ radius 8.0
    */


    int playerActor = CleoFunctions::GET_PLAYER_ACTOR(0);

    float x = 0, y = 0, z = 0;
    CleoFunctions::STORE_COORDS_FROM_ACTOR_WITH_OFFSET(playerActor, 0, 200, 0, &x, &y, &z);

    float spawnX = 0, spawnY = 0, spawnZ = 0;
    CleoFunctions::GET_NEAREST_CAR_PATH_COORDS_FROM(x, y, z, 2, &spawnX, &spawnY, &spawnZ);

    Log::file << "spawnX = " << spawnX << std::endl;
    Log::file << "spawnY = " << spawnY << std::endl;
    Log::file << "spawnZ = " << spawnZ << std::endl;

    auto car = CleoFunctions::CREATE_CAR_AT(vehicleModelId, spawnX, spawnY, spawnZ);
    Log::file << "car = " << car << std::endl;

    auto vehicle = Vehicles::TryCreateVehicle(car);
    vehicle->chaseStatus = CHASE_STATUS::CHASING;

    m_BackupVehicles.push_back(vehicle);

    int blipCar = vehicle->AddBlip(2);
    Log::file << "blipCar = " << blipCar << std::endl;

    int ped = CleoFunctions::CREATE_ACTOR_PEDTYPE_IN_CAR_DRIVERSEAT(car, 23, pedModelId);

    vehicle->hDriver = ped;

    Log::file << "ped = " << ped << std::endl;

    CleoFunctions::ENABLE_CAR_SIREN(car, true);
    CleoFunctions::SET_CAR_MAX_SPEED(car, 50.0f);
    //0423: set_car 6@ improved_handling_to 1.5
    CleoFunctions::SET_CAR_TRAFFIC_BEHAVIOUR(car, 2);
    
    Log::file << "set car " << car << " follow car " << m_ChasingPed->hVehicleOwned << std::endl;

    CleoFunctions::CAR_FOLLOR_CAR(car, m_ChasingPed->hVehicleOwned, 8.0f);

    Log::file << "end call backup" << std::endl;
}

void Chase::CallHeliBackup()
{
    /*
    04C4: store_coords_to 0@ 1@ 2@ from_actor $PLAYER_ACTOR with_offset 0.0 0.0 50.0
    00A5: 3@ = create_car 497 at 0@ 1@ 2@
    0129: 7@ = create_actor_pedtype 23 model 280 in_car 3@ driverseat 
    0825: set_helicopter 3@ instant_rotor_start
    0918: set_car 3@ engine_operation 1 
    
    0186: 4@ = create_marker_above_car 3@

    02FF: show_text_3numbers GXT 'MPFX1' numbers 1 0 0 time 3000 flag 1

    04C4: store_coords_to 0@ 1@ 2@ from_actor $PLAYER_ACTOR with_offset 0.0 0.0 0.0

    073E: get_car_in_sphere 0@ 1@ 2@ radius 20.0 model -1 handle_as 5@

    if 056E: car 5@ defined
    then
        0186: 6@ = create_marker_above_car 5@
        
        0726: heli 3@ follow_actor -1 follow_car 5@ radius 15.0 
        03E5: show_text_box 'MPFX86' 
    end
    */
    int playerActor = CleoFunctions::GET_PLAYER_ACTOR(0);

    float spawnX = 0, spawnY = 0, spawnZ = 0;
    CleoFunctions::STORE_COORDS_FROM_ACTOR_WITH_OFFSET(playerActor, 100, 100, 100, &spawnX, &spawnY, &spawnZ);

    auto heli = CleoFunctions::CREATE_CAR_AT(497, spawnX, spawnY, spawnZ);
    int driver = CleoFunctions::CREATE_ACTOR_PEDTYPE_IN_CAR_DRIVERSEAT(heli, 23, 280);
    CleoFunctions::SET_HELICOPTER_INSTANT_ROTOR_START(heli);
    CleoFunctions::SET_CAR_ENGINE_OPERATION(heli, true);

    auto heliVehicle = Vehicles::TryCreateVehicle(heli);
    heliVehicle->AddBlip(2);
    heliVehicle->chaseStatus = CHASE_STATUS::CHASING;
    heliVehicle->hDriver = driver;

    m_BackupVehicles.push_back(heliVehicle);

    CleoFunctions::HELI_FOLLOW(heli, m_ChasingPed->hPed, -1, 50.0f);

    CleoFunctions::WAIT(3000, []() {
        CleoFunctions::SHOW_TEXT_BOX("MPFX86");
    });
}

void Chase::MakeCarStartRunning(Vehicle* vehicle, Ped* ped)
{
    /*
    00AE: set_car 3@ traffic_behaviour_to 2
    00AD: set_car 3@ max_speed_to 50.0             
    00A8: set_car 3@ to_psycho_driver
    */

   CleoFunctions::SET_CAR_TRAFFIC_BEHAVIOUR(vehicle->hVehicle, 2);
   CleoFunctions::SET_CAR_MAX_SPEED(vehicle->hVehicle, 30.0f);
   CleoFunctions::SET_CAR_TO_PSYCHO_DRIVER(vehicle->hVehicle);

   m_ChasingPed = ped;
}

void Chase::EndChase()
{
    Log::file << "end chase" << std::endl;

    m_ChasingPed = NULL;

    for(auto vehicle : m_BackupVehicles)
    {
        vehicle->RemoveBlip();
    }
    //m_BackupVehicles.clear();

    //Log::file << "cleared backup vehicles" << std::endl;
}