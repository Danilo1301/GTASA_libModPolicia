#include "Chase.h"

#include "CleoFunctions.h"
#include "Pullover.h"
#include "Widgets.h"
#include "Log.h"
#include "Vehicles.h"

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
    auto vehicle = ped->vehicleOwned;

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
        if(ped->vehicleOwned) ped->vehicleOwned->RemoveBlip();

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

    m_BackupVehicles.push_back(vehicle);

    //int blipCar = vehicle->AddBlip();
    //Log::file << "blipCar = " << blipCar << std::endl;

    //CleoFunctions::SET_MARKER_COLOR_TO(blipCar, 2);

    int ped = CleoFunctions::CREATE_ACTOR_PEDTYPE_IN_CAR_DRIVERSEAT(car, 23, pedModelId);

    Log::file << "ped = " << ped << std::endl;

    CleoFunctions::ENABLE_CAR_SIREN(car, true);
    CleoFunctions::SET_CAR_MAX_SPEED(car, 50.0f);
    //0423: set_car 6@ improved_handling_to 1.5
    CleoFunctions::SET_CAR_TRAFFIC_BEHAVIOUR(car, 2);

    Log::file << "set car " << car << " follow car " << m_ChasingPed->vehicleOwned->hVehicle << std::endl;

    CleoFunctions::CAR_FOLLOR_CAR(car, m_ChasingPed->vehicleOwned->hVehicle, 8.0f);

    Log::file << "end call backup" << std::endl;
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
    m_BackupVehicles.clear();

    Log::file << "cleared backup vehicles" << std::endl;
}