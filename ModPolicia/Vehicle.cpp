#include "Vehicle.h"

#include "Log.h"
#include "Mod.h"
#include "CleoFunctions.h"
#include "Widgets.h"

#include "windows/WindowCarMenu.h"

extern void* (*GetVehicleFromRef)(int);

float Vehicle::CHANCE_VEHICLE_BEEING_STOLEN = 0.1;
float Vehicle::CHANCE_VEHICLE_HAVING_GUNS = 0.2;

Vehicle::Vehicle(int hVehicle)
{
    this->hVehicle = hVehicle;

    this->pVehicle = (CVehicle*)GetVehicleFromRef(hVehicle);

    this->modelId = CleoFunctions::GET_CAR_MODEL(hVehicle);

    this->isStolen = Mod::CalculateProbability(CHANCE_VEHICLE_BEEING_STOLEN);

    Log::file << "Vehicle constructor " << hVehicle << ", model " << modelId << std::endl;
}

Vehicle::~Vehicle()
{
    
}


void Vehicle::Update(int dt)
{
    if(!CleoFunctions::CAR_DEFINED(hVehicle)) return;

    UpdateCarMenuWidget();

    if(!CleoFunctions::CAR_DEFINED(hVehicle)) return;

    UpdateLeaveScene();
}

void Vehicle::UpdateLeaveScene()
{
    if(actionStatus == ACTION_STATUS::WAITING_FOR_PEDS_TO_ENTER_CAR)
    {
        int requiredPeds = 0;
        int pedsOnCar = 0;
        
        if(Mod::IsActorAliveAndDefined(hDriver))
        {
            requiredPeds++;

            if(CleoFunctions::IS_CHAR_IN_ANY_CAR(hDriver)) pedsOnCar++;
        }

        int seatId = 0;
        for(auto passenger : hPassengers)
        {
            if(Mod::IsActorAliveAndDefined(passenger))
            {
                requiredPeds++;

                if(CleoFunctions::IS_CHAR_IN_ANY_CAR(passenger)) pedsOnCar++;
            }
            seatId++;
        }

        Log::file << "waiting: " << pedsOnCar << " / " << requiredPeds << std::endl;

        if(requiredPeds == pedsOnCar)
        {
            actionStatus = ACTION_STATUS::LEAVING_SCENE;

            Log::file << "leaving scene" << std::endl;

            float findX = 0, findY = 0, findZ = 0;
            CleoFunctions::STORE_COORDS_FROM_CAR_WITH_OFFSET(hVehicle, 0, 200, 0, &findX, &findY, &findZ);
            
            CleoFunctions::CAR_DRIVE_TO(hVehicle, findX, findY, findZ);

            fromPos = Mod::GetCarPosition(hVehicle);
        }
    }

    if(actionStatus == ACTION_STATUS::LEAVING_SCENE)
    {
        auto distance = DistanceBetweenPoints(fromPos, Mod::GetCarPosition(hVehicle));

        if(distance > 80)
        {
            actionStatus = ACTION_STATUS::ACTION_NONE;

            Log::file << "destroying vehicle and passengers" << std::endl;

            if(Mod::IsActorAliveAndDefined(hDriver)) CleoFunctions::DESTROY_ACTOR(hDriver);

            for(auto passenger : hPassengers)
            {
                if(Mod::IsActorAliveAndDefined(passenger)) CleoFunctions::DESTROY_ACTOR(passenger);
            }
                
            CleoFunctions::DESTROY_CAR(hVehicle);

            Log::file << "destroyed" << std::endl;
        }
    }
}

void Vehicle::UpdateCarMenuWidget()
{
    if(!IsPoliceCar()) return;
    
    auto playerActor = CleoFunctions::GET_PLAYER_ACTOR(0);

    if(CleoFunctions::IS_CHAR_IN_ANY_CAR(playerActor)) return;
    
    auto distance = DistanceBetweenPoints(Mod::GetCarPosition(hVehicle), Mod::GetPedPosition(playerActor));

    if(distance < 2.0f)
    {
        if(!WindowCarMenu::m_Window)
        {
            if(Widgets::IsWidgetJustPressed(69))
            {
                WindowCarMenu::Create();
            }
        }
    }
}

void Vehicle::UpdateInventory()
{
    if(inventory->created) return;

    inventory->created = true;

    if(Mod::CalculateProbability(0.3))
    {
        inventory->AddItemToInventory(Item_Type::WEED);
    }

    if(Mod::CalculateProbability(0.5))
    {
        inventory->AddItemToInventory(Item_Type::BEER);
    }

    if(Mod::CalculateProbability(CHANCE_VEHICLE_HAVING_GUNS))
    {
        inventory->AddItemToInventory(Item_Type::REVOLVER_38);

        if(Mod::CalculateProbability(0.5))
        {
            inventory->AddItemToInventory(Item_Type::PISTOL);
        }
    }

    if(Mod::CalculateProbability(0.1))
    {
        auto stolenCellphone = inventory->AddItemToInventory(Item_Type::CELLPHONE);
        stolenCellphone->isSlotenCellphone = true;
    }
}

bool Vehicle::IsPoliceCar()
{
    if(modelId == 596) return true;
    if(modelId == 597) return true;
    if(modelId == 598) return true;
    if(modelId == 599) return true;
    if(modelId == 601) return true;
    if(modelId == 528) return true;
    if(modelId == 490) return true;
    return false;
}

bool Vehicle::HasIlegalStuff()
{
    if(inventory->HasItemOfType(Item_Type::REVOLVER_38)) return true;
    if(inventory->HasItemOfType(Item_Type::PISTOL)) return true;
    if(inventory->HasItemOfType(Item_Type::CELLPHONE))
    {
        for(auto cellphone : inventory->GetItemsOfType(Item_Type::CELLPHONE))
        {
            if(cellphone->isSlotenCellphone) return true;
        }
    }
    return false;
}

int Vehicle::AddBlip()
{
    if(blip > 0) RemoveBlip();
    blip = CleoFunctions::ADD_BLIP_FOR_CAR(hVehicle);
    return blip;
}

int Vehicle::AddBlip(int color)
{
    int blip = AddBlip();
    CleoFunctions::SET_MARKER_COLOR_TO(blip, color);
    return blip;
}

void Vehicle::RemoveBlip()
{
    if(blip <= 0) return;

    CleoFunctions::DISABLE_MARKER(blip);
    blip = 0;
}

void Vehicle::MakePedsEnterVehicleAndLeaveScene()
{
    Log::file << "MakePedsEnterVehicleAndLeaveScene" << std::endl;

    actionStatus = ACTION_STATUS::WAITING_FOR_PEDS_TO_ENTER_CAR;

    CheckPassengers();
    
    if(!Mod::IsActorAliveAndDefined(hDriver)) ReplaceDriverByAnyPassenger();

    if(!CleoFunctions::CAR_DEFINED(hDriver))
    {
        Log::file << "Car is not defined" << std::endl;
        actionStatus = ACTION_STATUS::ACTION_NONE;
        return;
    }

    if(Mod::IsActorAliveAndDefined(hDriver))
    {
        if(!CleoFunctions::IS_CHAR_IN_ANY_CAR(hDriver))
        {
            CleoFunctions::ENTER_CAR_AS_DRIVER_AS_ACTOR(hDriver, hVehicle, 10000);
        }
    }

    int seatId = 0;
    for(auto passenger : hPassengers)
    {
        if(Mod::IsActorAliveAndDefined(passenger))
        {
            if(!CleoFunctions::IS_CHAR_IN_ANY_CAR(passenger))
            {
                CleoFunctions::ACTOR_ENTER_CAR_PASSENGER_SEAT(passenger, hVehicle, 10000, seatId);
            }
        }
        seatId++;
    }
}

void Vehicle::CheckPassengers()
{
    Log::file << "CheckPassengers" << std::endl;

    std::vector<int> passengersToRemove;

    Log::file << "Before deleting:" << std::endl;
    for(auto passenger : hPassengers) Log::file << "Passenger: " << passenger << std::endl;

    for(auto passenger : hPassengers)
    {
        if(!Mod::IsActorAliveAndDefined(passenger))
        {
            passengersToRemove.push_back(passenger);
        }
    }

    for(auto passenger : passengersToRemove)
    {
        auto it = std::find(hPassengers.begin(), hPassengers.end(), passenger);
        hPassengers.erase(it);
    }

    Log::file << "After deleting:" << std::endl;
    for(auto passenger : hPassengers) Log::file << "Passenger: " << passenger << std::endl;
}

void Vehicle::ReplaceDriverByAnyPassenger()
{
    Log::file << "ReplaceDriverByAnyPassenger" << std::endl;

    bool changedDriver = false;

    Log::file << "Before deleting:" << std::endl;
    Log::file << "Driver: " << hDriver << ", passengers: " << hPassengers.size() << std::endl;
    for(auto passenger : hPassengers) Log::file << "Passenger: " << passenger << std::endl;

    for(auto passenger : hPassengers)
    {
        if(Mod::IsActorAliveAndDefined(passenger))
        {
            Log::file << "Passenger " << passenger << " is able to be driver" << std::endl;

            hDriver = passenger;
            changedDriver = true;
            break;
        }
    }

    if(changedDriver)
    {
        auto it = std::find(hPassengers.begin(), hPassengers.end(), hDriver);
        hPassengers.erase(it);
    }

    Log::file << "After deleting:" << std::endl;
    Log::file << "Driver: " << hDriver << ", passengers: " << hPassengers.size() << std::endl;
    for(auto passenger : hPassengers) Log::file << "Passenger: " << passenger << std::endl;
}