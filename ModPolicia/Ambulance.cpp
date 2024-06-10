#include "Ambulance.h"

#include "Log.h"
#include "CleoFunctions.h"
#include "Vehicles.h"
#include "Peds.h"
#include "Mod.h"

EmergencyVehicleSystem::EmergencyVehicleSystem(int vehicleModelId, int pedModelId, bool ressurectPed)
{
    m_VehicleModelId = vehicleModelId;
    m_PedModelId = pedModelId;
    m_RessurectPed = ressurectPed;
}

void EmergencyVehicleSystem::Update(int dt)
{
    std::vector<Vehicle*> vehiclesToRemove;
    std::vector<Ped*> pedsToRemove;

    //update vehicles
    for(auto vehicle : m_Vehicles)
    {
        if(vehicle->actionStatus == ACTION_STATUS::MEDIC_GOING_TO_LOCATION)
        {
            auto targetPosition = vehicle->drivingTo;
            auto distance = DistanceBetweenPoints(Mod::GetCarPosition(vehicle->hVehicle), targetPosition);

            if(distance < 20)
            {
                vehicle->actionStatus = ACTION_STATUS::ACTION_NONE;
                vehicle->RemoveBlip();

                Log::Level(LOG_LEVEL::LOG_BOTH) << "vehicle is in targetPosition, now its ACTION_NONE" << std::endl;
            }
        }
    }

    //update medics
    for(auto vehicle : m_Vehicles)
    {
        vehicle->CheckDriverAndPassengersAreAlive();

        std::vector<Ped*> medics;
        if(vehicle->hDriverOwner > 0)
        {
            medics.push_back(Peds::GetPedByHandle(vehicle->hDriverOwner));
        }

        for(auto passenger : vehicle->hPassengersOwner)
        {
            medics.push_back(Peds::GetPedByHandle(passenger));
        }

        if(vehicle->actionStatus == ACTION_STATUS::ACTION_NONE)
        {
            for(auto medic : medics)
            {
                if(medic->actionStatus == PED_ACTION_STATUS::PED_ACTION_NONE)
                {
                    Log::Level(LOG_LEVEL::LOG_BOTH) << "finding a dead body..." << std::endl;

                    auto body = Ambulance::GetDeadPedThatIsFree(medic);
                    
                    Log::Level(LOG_LEVEL::LOG_BOTH) << "body: " << body << std::endl;

                    if(body)
                    {
                        medic->actionStatus = PED_ACTION_STATUS::MEDIC_GOING_TO_BODY;

                        medic->goingToPed = body->hPed;

                        Log::Level(LOG_LEVEL::LOG_BOTH) << "medic running to dead body" << std::endl;

                        CleoFunctions::AS_ACTOR_RUN_TO_ACTOR(medic->hPed, body->hPed, 10000, 1.0f);
                    } else {
                        medic->actionStatus = PED_ACTION_STATUS::MEDIC_FOUND_NO_BODIES;

                        Log::Level(LOG_LEVEL::LOG_BOTH) << "medic found no bodies" << std::endl;
                    }
                }

                if(medic->actionStatus == PED_ACTION_STATUS::MEDIC_GOING_TO_BODY)
                {
                    if(!CleoFunctions::ACTOR_DEFINED(medic->goingToPed))
                    {
                        Log::Level(LOG_LEVEL::LOG_BOTH) << "body disapeared, set medic action to NONE" << std::endl;

                        medic->actionStatus = PED_ACTION_STATUS::PED_ACTION_NONE;

                        continue;
                    }

                    if(CleoFunctions::ACTOR_STOPPED(medic->hPed))
                    {
                        Log::Level(LOG_LEVEL::LOG_BOTH) << "medic forgor what he was doing, set action to NONE" << std::endl;

                        medic->actionStatus = PED_ACTION_STATUS::PED_ACTION_NONE;
                        medic->goingToPed = 0;

                        continue;
                    }

                    auto distance = DistanceBetweenPoints(Mod::GetPedPosition(medic->hPed), Mod::GetPedPosition(medic->goingToPed));

                    if(distance < 1.5f)
                    {
                        if(m_RessurectPed)
                        {
                            medic->actionStatus = PED_ACTION_STATUS::MEDIC_ON_BODY;

                            TakeBody(medic, true);
                        } else {
                            medic->actionStatus = PED_ACTION_STATUS::MEDIC_TAKING_PICTURES;

                            Log::Level(LOG_LEVEL::LOG_BOTH) << "medic taking picures" << std::endl;
                            
                            auto pedPosition = Mod::GetPedPosition(medic->goingToPed);

                            CleoFunctions::ROTATE_AND_SHOOT(medic->hPed, pedPosition.x, pedPosition.y, pedPosition.z, 6000);

                            CleoFunctions::WAIT(6000, [medic] () {
                                medic->actionStatus = PED_ACTION_STATUS::MEDIC_ON_BODY;

                                TakeBody(medic, false);
                            });
                        }   
                    }
                }
            }
        }

        if(vehicle->actionStatus == ACTION_STATUS::ACTION_NONE)
        {
            int medicsReady = 0;
            int totalMedics = 0;
            for(auto medic : medics)
            {
                if(Mod::IsActorAliveAndDefined(medic->hPed))
                {
                    totalMedics++;
                }

                if(medic->actionStatus == PED_ACTION_STATUS::MEDIC_FOUND_NO_BODIES)
                {
                    medicsReady++;
                }
            }

            if(totalMedics > 0)
            {
                if(medicsReady == totalMedics)
                {
                    vehicle->MakePedsEnterVehicleAndLeaveScene();

                    vehiclesToRemove.push_back(vehicle);
                }
            } else {
                //if all medics died
                vehiclesToRemove.push_back(vehicle);
            }
        }
    }

    //update peds
    for(auto medic : m_Peds)
    {
        if(!Mod::IsActorAliveAndDefined(medic->hPed))
        {
            pedsToRemove.push_back(medic);
        }
    }

    //remove all
    for(auto item : vehiclesToRemove)
    {
        auto it = std::find(m_Vehicles.begin(), m_Vehicles.end(), item);
        m_Vehicles.erase(it);
    }

    for(auto item : pedsToRemove)
    {
        auto it = std::find(m_Peds.begin(), m_Peds.end(), item);
        m_Peds.erase(it);
    }
}

void EmergencyVehicleSystem::TakeBody(Ped* medic, bool ressurect)
{
    Log::Level(LOG_LEVEL::LOG_BOTH) << "medic doing CPR" << std::endl;

    CleoFunctions::PERFORM_ANIMATION_AS_ACTOR(medic->hPed, "CPR", "MEDIC", 8.0f, 0, 0, 0, 0, -1);

    CleoFunctions::WAIT(6000, [medic, ressurect] () {
        Log::Level(LOG_LEVEL::LOG_BOTH) << "medic stopped CPR" << std::endl;

        medic->actionStatus = PED_ACTION_STATUS::PED_ACTION_NONE;
        
        if(CleoFunctions::ACTOR_DEFINED(medic->goingToPed))
        {
            if(ressurect)
            {
                Log::Level(LOG_LEVEL::LOG_BOTH) << "ressurect" << std::endl;

                auto pedBody = Peds::TryCreatePed(medic->goingToPed);
                auto pedPosition = Mod::GetPedPosition(medic->goingToPed);

                int modelId = CleoFunctions::GET_ACTOR_MODEL(medic->goingToPed);

                Log::Level(LOG_LEVEL::LOG_BOTH) << "create new ped" << std::endl;

                auto newPedHandle = CleoFunctions::CREATE_ACTOR_PEDTYPE(4, modelId, pedPosition.x, pedPosition.y, pedPosition.z);
                auto newPed = Peds::TryCreatePed(newPedHandle);

                Log::Level(LOG_LEVEL::LOG_BOTH) << "copy info between peds" << std::endl;

                newPed->UpdateInventory();
                newPed->CopyFrom(pedBody);

                CleoFunctions::REMOVE_REFERENCES_TO_ACTOR(newPedHandle);
            }

            Log::Level(LOG_LEVEL::LOG_BOTH) << "destroying body" << std::endl;

            CleoFunctions::DESTROY_ACTOR(medic->goingToPed);
        }

        medic->goingToPed = 0;
    });
}

std::vector<Ped*> EmergencyVehicleSystem::GetAllMedics()
{
    /*
    std::vector<Ped*> medics;
    
    for(auto vehicle : m_AmbulanceSystem->m_Vehicles)
    {
        if(Mod::IsActorAliveAndDefined(vehicle->hDriver))
        {
            medics.push_back(Peds::GetPedByHandle(vehicle->hDriver));
        }

        for(auto passenger : vehicle->hPassengers)
        {
            if(Mod::IsActorAliveAndDefined(passenger))
            {
                medics.push_back(Peds::GetPedByHandle(passenger));
            }
        }
    }

    return medics;
    */

   return m_Peds;
}

void EmergencyVehicleSystem::CallVehicle(CVector location)
{
    Log::Level(LOG_LEVEL::LOG_BOTH) << "Call vehicle at " << CVectorToString(location) << std::endl;

    float spawnX = 0, spawnY = 0, spawnZ = 0;
    CleoFunctions::GET_NEAREST_CAR_PATH_COORDS_FROM(location.x + 100.0f, location.y + 100.0f, location.z, 2, &spawnX, &spawnY, &spawnZ);

    Log::Level(LOG_LEVEL::LOG_BOTH) << "spawn vehicle at " << CVectorToString(CVector(spawnX, spawnY, spawnZ)) << std::endl;

    auto ambulance = CleoFunctions::CREATE_CAR_AT(m_VehicleModelId, spawnX, spawnY, spawnZ);
    Log::Level(LOG_LEVEL::LOG_BOTH) << "ambulance = " << ambulance << std::endl;

    auto vehicle = Vehicles::TryCreateVehicle(ambulance);
    vehicle->AddBlip(2);

    vehicle->actionStatus = ACTION_STATUS::MEDIC_GOING_TO_LOCATION;
    
    m_Vehicles.push_back(vehicle);

    //23 = special
    int type = 23;

    int driver = CleoFunctions::CREATE_ACTOR_PEDTYPE_IN_CAR_DRIVERSEAT(ambulance, type, m_PedModelId);
    auto pedDriver = Peds::TryCreatePed(driver);
    m_Peds.push_back(pedDriver);

    Log::Level(LOG_LEVEL::LOG_BOTH) << "driver = " << driver << std::endl;

    int passenger = CleoFunctions::CREATE_ACTOR_PEDTYPE_IN_CAR_PASSENGER_SEAT(ambulance, type, m_PedModelId, 0);
    auto pedPassenger = Peds::TryCreatePed(passenger);
    m_Peds.push_back(pedPassenger);

    vehicle->SetDriverAndPassengersOwners();
    
    if(!m_RessurectPed)
    {
        CleoFunctions::GIVE_ACTOR_WEAPON(driver, 43, 1000);
        CleoFunctions::GIVE_ACTOR_WEAPON(passenger, 43, 1000);
    }

    CleoFunctions::ENABLE_CAR_SIREN(ambulance, true);
    CleoFunctions::SET_CAR_MAX_SPEED(ambulance, 50.0f);
    //0423: set_car 6@ improved_handling_to 1.5
    CleoFunctions::SET_CAR_TRAFFIC_BEHAVIOUR(ambulance, 2);
    
    Log::Level(LOG_LEVEL::LOG_BOTH) << "make car drive to location" << std::endl;
    
    CleoFunctions::CAR_DRIVE_TO(ambulance, location.x, location.y, location.z);

    vehicle->drivingTo = location;
}




//--------------------------------------------------




EmergencyVehicleSystem* Ambulance::m_AmbulanceSystem = new EmergencyVehicleSystem(416, 274, true);
EmergencyVehicleSystem* Ambulance::m_IMLSystem = new EmergencyVehicleSystem(442, 70, false);

void Ambulance::Init()
{
    
}

void Ambulance::Update(int dt)
{
    m_AmbulanceSystem->Update(dt);
    m_IMLSystem->Update(dt);
}

void Ambulance::CallAmbulance(CVector location)
{
    m_AmbulanceSystem->CallVehicle(location);
}

void Ambulance::CallIML(CVector location)
{
    m_IMLSystem->CallVehicle(location);
}

Ped* Ambulance::GetDeadPedThatIsFree(Ped* medic)
{
    std::vector<Ped*> availablePeds;

    auto deadPeds = Peds::GetDeadPeds(Mod::GetPedPosition(medic->hPed), 50.0f);
    for(auto ped : deadPeds)
    {
        if(!IsAnyMedicGoingToPed(ped))
        {
            availablePeds.push_back(ped);
        }
    }

    if(availablePeds.size() == 0) return NULL;

    return availablePeds[0];
}

std::vector<Ped*> Ambulance::GetAllMedics()
{
    std::vector<Ped*> medics;
    
    for(auto medic : m_AmbulanceSystem->GetAllMedics()) medics.push_back(medic);
    for(auto medic : m_IMLSystem->GetAllMedics()) medics.push_back(medic);

    return medics;
}

bool Ambulance::IsAnyMedicGoingToPed(Ped* ped)
{
    auto medics = GetAllMedics();

    for(auto medic : medics)
    {
        if(medic->goingToPed == ped->hPed) return true;
    }

    return false;
}