#include "Chase.h"

#include "CleoFunctions.h"
#include "Pullover.h"
#include "Widgets.h"
#include "Log.h"
#include "Vehicles.h"
#include "Mod.h"

#include "windows/WindowBackup.h"

Ped* Chase::m_ChasingPed = NULL;

void Chase::Update(int dt)
{
    UpdateChase(dt);
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
}