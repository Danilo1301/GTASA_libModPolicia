#include "Vehicle.h"

#include "Log.h"
#include "Mod.h"
#include "CleoFunctions.h"
#include "Widgets.h"
#include "Peds.h"
#include "SoundSystem.h"
#include "Debug.h"
#include "ModConfig.h"

#include "windows/WindowCarMenu.h"

extern void* (*GetVehicleFromRef)(int);
extern IMenuVSL* menuVSL;

float Vehicle::CHANCE_VEHICLE_BEEING_STOLEN = 0.1;

Vehicle::Vehicle(int hVehicle)
{
    this->hVehicle = hVehicle;

    this->pVehicle = (CVehicle*)GetVehicleFromRef(hVehicle);

    this->modelId = CleoFunctions::GET_CAR_MODEL(hVehicle);

    this->isStolen = Mod::CalculateProbability(CHANCE_VEHICLE_BEEING_STOLEN);

    this->trunk = new TrunkData(modelId, hVehicle);
}

Vehicle::~Vehicle()
{
    if(screenButton) screenButton->SetToBeRemoved();
}

void Vehicle::Update(int dt)
{
    Log::Level(LOG_LEVEL::LOG_DEEP_UPDATE) << "Vehicle " << hVehicle << ": Update" << std::endl;

    if(!CleoFunctions::CAR_DEFINED(hVehicle)) return;

    //detect if car crashes
    hasJustCrashed = false;
    auto speed = GetSpeed();
    if(prevSpeed == 0.0f) prevSpeed = speed;
    auto difference = std::abs(speed - prevSpeed);
    if(difference > 5.0f)
    {
        Log::Level(LOG_LEVEL::LOG_BOTH) << "Vehicle " << hVehicle << " has crashed! Speed went from " << prevSpeed << " to " << speed << std::endl;
        hasJustCrashed = true;
    }
    prevSpeed = speed;

    //

    if(freezeCarPosition)
    {
        auto position = GetCarPosition(hVehicle);

        CleoFunctions::PUT_CAR_AT(hVehicle, position.x, position.y, position.z);
    }

    //

    UpdateCarMenuWidget();

    if(!CleoFunctions::CAR_DEFINED(hVehicle)) return;

    UpdateLeaveScene(dt);
}

void Vehicle::UpdateLeaveScene(int dt)
{
    if(actionStatus == ACTION_STATUS::WAITING_FOR_PEDS_TO_ENTER_CAR_AND_LEAVE)
    {   
        auto isPedsOnCar = IsAllDriverAndPassengersInsideCar();
        
        if(isPedsOnCar)
        {
            timeOnCar += dt;
        } else {
            timeOnCar = 0;
        }

        Debug::AddLine(1, timeOnCar);

        if(isPedsOnCar && timeOnCar > 1000)
        {
            int amountOfPeds = GetDriverAndPassengers().size();

            if(amountOfPeds != 0)
            {
                actionStatus = ACTION_STATUS::LEAVING_SCENE;

                Log::Level(LOG_LEVEL::LOG_BOTH) << "Vehicle " << hVehicle << " leaving scene" << std::endl;

                float findX = 0, findY = 0, findZ = 0;
                CleoFunctions::STORE_COORDS_FROM_CAR_WITH_OFFSET(hVehicle, 0, 200, 0, &findX, &findY, &findZ);

                if(IsPoliceHelicopter())
                {
                    Log::Level(LOG_LEVEL::LOG_BOTH) << "heli " << hVehicle << " leaving scene" << std::endl;

                    CleoFunctions::HELI_FLY_TO(hVehicle, findX, findY, 200.0f, 200.0f, 200.0f);
                } else {
                    //CleoFunctions::CLEAR_ACTOR_TASK(hDriver); //doesnt do anything
                    //CleoFunctions::CAR_DRIVE_TO(hVehicle, findX, findY, findZ); //makes car drive backwards?
                    CleoFunctions::AS_ACTOR_DRIVE_CAR_TO(hDriverOwner, hVehicle, findX, findY, findZ, 15.0f, 0, 0, 2);
                }

                fromPos = GetCarPosition(hVehicle);
            } else {
                actionStatus = ACTION_STATUS::ACTION_NONE;

                Log::Level(LOG_LEVEL::LOG_BOTH) << hVehicle << " was going to leave scene but num of peds is 0, setting action to NONE" << std::endl;
            }
            
        }
    }

    if(actionStatus == ACTION_STATUS::WAITING_FOR_PEDS_TO_ENTER_CAR_AND_DO_NOTHING)
    {
        auto isPedsOnCar = IsAllDriverAndPassengersInsideCar();

        if(isPedsOnCar)
        {
            Log::Level(LOG_LEVEL::LOG_BOTH) << "peds entered car, setting action status to NONE" << std::endl;

            actionStatus = ACTION_STATUS::ACTION_NONE;
        }
    }

    if(actionStatus == ACTION_STATUS::WAITING_FOR_PEDS_TO_EXIT)
    {
        auto isPedsOutCar = IsAllDriverAndPassengersOutsideCar();

        if(isPedsOutCar)
        {
            Log::Level(LOG_LEVEL::LOG_BOTH) << "peds left car, setting action status to NONE" << std::endl;

            actionStatus = ACTION_STATUS::ACTION_NONE;
        }
    }

    if(actionStatus == ACTION_STATUS::LEAVING_SCENE)
    {
        auto distance = DistanceBetweenPoints(fromPos, GetCarPosition(hVehicle));

        if(distance > 120)
        {
            actionStatus = ACTION_STATUS::ACTION_NONE;

            Log::Level(LOG_LEVEL::LOG_BOTH) << "destroying vehicle " << hVehicle << " and passengers" << std::endl;

            if(IsActorAliveAndDefined(hDriverOwner)) CleoFunctions::DESTROY_ACTOR(hDriverOwner);

            for(auto passenger : hPassengersOwner)
            {
                if(IsActorAliveAndDefined(passenger)) CleoFunctions::DESTROY_ACTOR(passenger);
            }
                
            CleoFunctions::DESTROY_CAR(hVehicle);

            if(towingVehicle > 0)
            {
                Log::Level(LOG_LEVEL::LOG_BOTH) << "destroying towing vehicle" << std::endl;

                CleoFunctions::DESTROY_CAR(towingVehicle);
            }

            Log::Level(LOG_LEVEL::LOG_BOTH) << "destroyed" << std::endl;
        }
    }
}

void Vehicle::UpdateCarMenuWidget()
{
    // if(!IsPoliceCar()) return;
    
    // auto playerActor = CleoFunctions::GET_PLAYER_ACTOR(0);

    // if(CleoFunctions::IS_CHAR_IN_ANY_CAR(playerActor)) return;
    
    // auto distance = DistanceBetweenPoints(GetCarPosition(hVehicle), GetPedPosition(playerActor));

    // if(distance < 2.0f)
    // {
    //     if(!WindowCarMenu::m_Window)
    //     {
    //         if(Widgets::IsWidgetJustPressed(69))
    //         {
    //             WindowCarMenu::Create(this);
    //         }
    //     }
    // }
}

void Vehicle::Draw()
{
    auto position = GetCarPosition(hVehicle);

    auto playerActor = GetPlayerActor();
    //auto pedPosition = GetPedPosition(playerActor);

    auto distanceFromPed = DistanceFromPed(playerActor, position);

    auto isInAnyVehicle = CleoFunctions::IS_CHAR_IN_ANY_CAR(GetPlayerActor());

    auto showButton = distanceFromPed < 4.0f && !isInAnyVehicle;

    if(showButton)
    {
        if(!screenButton)
        {
            auto vehicle = this;

            std::string path = ModConfig::GetConfigFolder() + "/assets/button_car.png";
            screenButton = menuVSL->AddScreenButton(CVector2D(0, 0), path, CVector2D(60, 60));
            screenButton->m_Text = "Ver opcoes";
            screenButton->onClick = [vehicle]() {

                vehicle->UpdateInventoryAndOwners();

                WindowCarMenu::Create(vehicle);
            };
        }
    } else {
        if(screenButton)
        {
            screenButton->SetToBeRemoved();
            screenButton = NULL;
        }
    }
 
    auto topPosition = position + CVector(0, 0, 1.5f);

    auto buttonPos = menuVSL->ConvertWorldPositionToScreenPosition(topPosition);

    if(screenButton)
    {
        screenButton->m_Position = buttonPos;
    }
}

void Vehicle::UpdateInventory()
{
    if(inventory->created) return;

    inventory->created = true;

    if(Mod::CalculateProbability(0.30))
    {
        inventory->AddItemToInventory("weed");
    }

    for(auto itemData : InventoryItems::GetItemsDataOfType(ItemType::ALCOHOL_DRINK))
    {
        if(Mod::CalculateProbability(itemData->chance))
        {
            inventory->AddItemToInventory(itemData->id);
        }
    }

    for(auto itemData : InventoryItems::GetItemsDataOfType(ItemType::COMMON_ITEM))
    {
        if(itemData->canSpawnInVehicles)
        {
            if(Mod::CalculateProbability(itemData->chance))
            {
                inventory->AddItemToInventory(itemData->id);
            }
        }
    }

    if(Mod::CalculateProbability(0.25))
    {
        inventory->AddItemToInventory("revolver38");

        if(Mod::CalculateProbability(0.60))
        {
            inventory->AddItemToInventory("pistol");
        }

        if(Mod::CalculateProbability(0.60))
        {
            inventory->AddItemToInventory("rifle");
        }
    }

    if(Mod::CalculateProbability(0.2))
    {
        auto stolenCellphone = inventory->AddItemToInventory("cellphone");
        stolenCellphone->isStolen = true;
    }
}

void Vehicle::UpdateInventoryAndOwners()
{
    auto vehicle = this;

    vehicle->UpdateInventory();
    vehicle->SetDriverAndPassengersOwners();

    if(hDriverOwner > 0)
    {
        auto ped = Peds::TryCreatePed(hDriverOwner);
        ped->UpdateInventory();
    }

    auto passengersHandle = vehicle->GetPassengers();
    for(auto passengerHandle : passengersHandle)
    {
        auto passenger = Peds::TryCreatePed(passengerHandle);
        passenger->UpdateInventory();
    }
}

bool Vehicle::IsPoliceCar()
{
    if(modelId == 596) return true;
    if(modelId == 597) return true;
    if(modelId == 598) return true;
    if(modelId == 599) return true;
    if(modelId == 528) return true;
    if(modelId == 490) return true;
    if(modelId == 601) return true;
    return false;
}

bool Vehicle::IsPoliceHelicopter()
{
    if(modelId == 497) return true;
    return false;
}

bool Vehicle::HasIlegalStuff()
{
    if(HasGuns()) return true;
    
    if(inventory->HasItemOfType(ItemType::CELLPHONE))
    {
        for(auto cellphone : inventory->GetItemsOfType(ItemType::CELLPHONE))
        {
            if(cellphone->isStolen) return true;
        }
    }
    return false;
}

bool Vehicle::HasGuns()
{
    if(inventory->HasItemOfType(ItemType::ILEGAL_GUN)) return true;
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
    Log::Level(LOG_LEVEL::LOG_BOTH) << "MakePedsEnterVehicleAndLeaveScene " << hVehicle  << std::endl;

    actionStatus = ACTION_STATUS::WAITING_FOR_PEDS_TO_ENTER_CAR_AND_LEAVE;

    MakePedsEnterVehicle();
}

void Vehicle::MakePedsEnterVehicle()
{
    Log::Level(LOG_LEVEL::LOG_BOTH) << "MakePedsEnterVehicle " << hVehicle << std::endl;

    CheckDriverAndPassengersAreAlive();

    if(!CleoFunctions::CAR_DEFINED(hVehicle))
    {
        Log::Level(LOG_LEVEL::LOG_BOTH) << "Car is not defined" << std::endl;
        actionStatus = ACTION_STATUS::ACTION_NONE;
        return;
    }

    if(IsActorAliveAndDefined(hDriverOwner))
    {
        if(!CleoFunctions::IS_CHAR_IN_ANY_CAR(hDriverOwner))
        {
            Log::Level(LOG_LEVEL::LOG_BOTH) << "hDriverOwner is going to enter car" << std::endl;
            CleoFunctions::CLEAR_ACTOR_TASK(hDriverOwner);
            CleoFunctions::ENTER_CAR_AS_DRIVER_AS_ACTOR(hDriverOwner, hVehicle, 10000);
        }
    }

    int seatId = 0;
    for(auto passenger : hPassengersOwner)
    {
        if(IsActorAliveAndDefined(passenger))
        {
            if(!CleoFunctions::IS_CHAR_IN_ANY_CAR(passenger))
            {
                Log::Level(LOG_LEVEL::LOG_BOTH) << "passenger is going to enter car" << std::endl;
                CleoFunctions::CLEAR_ACTOR_TASK(passenger);
                CleoFunctions::ACTOR_ENTER_CAR_PASSENGER_SEAT(passenger, hVehicle, 10000, seatId);
            }
        }
        seatId++;
    }
}

void Vehicle::MakePedsExitCar()
{
    Log::Level(LOG_LEVEL::LOG_BOTH) << "MakePedsExitCar" << std::endl;

    auto driverHandle = GetDriver();
    if(driverHandle > 0)
    {
        CleoFunctions::AS_ACTOR_EXIT_CAR(driverHandle);
    }

    for(auto passengerHandle : GetPassengers())
    {
        CleoFunctions::AS_ACTOR_EXIT_CAR(passengerHandle);
    }
}

void Vehicle::CheckDriverAndPassengersAreAlive()
{
    //Log::Level(LOG_LEVEL::LOG_UPDATE) << "CheckDriverAndPassengersAreAlive" << std::endl;
    
    //check passengers first
    std::vector<int> passengersToRemove;

    for(auto passengerHandle : hPassengersOwner)
    {
        if(!IsActorAliveAndDefined(passengerHandle))
        {
            Log::Level(LOG_LEVEL::LOG_BOTH) << "The passenger owner " << passengerHandle << " died" << std::endl;
            passengersToRemove.push_back(passengerHandle);
        }
    }

    if(passengersToRemove.size() > 0)
    {
        Log::Level(LOG_LEVEL::LOG_BOTH) << passengersToRemove.size() << " passengers has died" << std::endl;
        Log::Level(LOG_LEVEL::LOG_BOTH) << "Before deleting:" << std::endl;
        for(auto passenger : hPassengersOwner) Log::Level(LOG_LEVEL::LOG_BOTH) << "Passenger: " << passenger << std::endl;
    }

    for(auto passenger : passengersToRemove)
    {
        auto it = std::find(hPassengersOwner.begin(), hPassengersOwner.end(), passenger);
        hPassengersOwner.erase(it);
    }

    if(passengersToRemove.size() > 0)
    {
        Log::Level(LOG_LEVEL::LOG_BOTH) << "After deleting:" << std::endl;
        for(auto passenger : hPassengersOwner) Log::Level(LOG_LEVEL::LOG_BOTH) << "Passenger: " << passenger << std::endl;
    }
    
    //check driver
    if(hDriverOwner > 0)
    {
        if(!IsActorAliveAndDefined(hDriverOwner))
        {
            Log::Level(LOG_LEVEL::LOG_BOTH) << "Driver " << hDriverOwner << " died" << std::endl;

            hDriverOwner = 0;

            ReplaceDriverByAnyPassenger();
        }
    }
}

void Vehicle::ReplaceDriverByAnyPassenger()
{
    Log::Level(LOG_LEVEL::LOG_BOTH) << "ReplaceDriverByAnyPassenger" << std::endl;

    bool changedDriver = false;

    Log::Level(LOG_LEVEL::LOG_BOTH) << "Before deleting:" << std::endl;
    Log::Level(LOG_LEVEL::LOG_BOTH) << "Driver: " << hDriverOwner << ", passengers: " << hPassengersOwner.size() << std::endl;
    for(auto passenger : hPassengersOwner) Log::Level(LOG_LEVEL::LOG_BOTH) << "Passenger: " << passenger << std::endl;

    for(auto passenger : hPassengersOwner)
    {
        if(IsActorAliveAndDefined(passenger))
        {
            Log::Level(LOG_LEVEL::LOG_BOTH) << "Passenger " << passenger << " is able to be driver" << std::endl;

            hDriverOwner = passenger;
            changedDriver = true;
            break;
        }
    }

    if(changedDriver)
    {
        auto it = std::find(hPassengersOwner.begin(), hPassengersOwner.end(), hDriverOwner);
        hPassengersOwner.erase(it);
    }

    Log::Level(LOG_LEVEL::LOG_BOTH) << "After deleting:" << std::endl;
    Log::Level(LOG_LEVEL::LOG_BOTH) << "Driver: " << hDriverOwner << ", passengers: " << hPassengersOwner.size() << std::endl;
    for(auto passenger : hPassengersOwner) Log::Level(LOG_LEVEL::LOG_BOTH) << "Passenger: " << passenger << std::endl;
}

void Vehicle::RemoveDriverAndPassengersBlip()
{
    Log::Level(LOG_LEVEL::LOG_BOTH) << "removing vehicle blip" << std::endl;
    RemoveBlip();

    Log::Level(LOG_LEVEL::LOG_BOTH) << "removing driver " << hDriverOwner << " blip" << std::endl;
    if(hDriverOwner > 0) Peds::TryCreatePed(hDriverOwner)->RemoveBlip();

    Log::Level(LOG_LEVEL::LOG_BOTH) << "removing passengers blip" << std::endl;
    for(auto passenger : hPassengersOwner) {
        Peds::TryCreatePed(passenger)->RemoveBlip();
    }

    Log::Level(LOG_LEVEL::LOG_BOTH) << "blips removed" << std::endl;
}

std::vector<int> Vehicle::GetPassengers()
{
    std::vector<int> passengers;

    auto maxPassengers = CleoFunctions::CAR_MAX_PASSENGERS(hVehicle);
    //Log::Level(LOG_LEVEL::LOG_BOTH) << "maxPassengers: " << maxPassengers << std::endl;
    for(int seatId = 0; seatId < maxPassengers; seatId++)
    {
        //Log::Level(LOG_LEVEL::LOG_BOTH) << "checking seat: " << seatId << std::endl;

        if(!CleoFunctions::CAR_PASSENGER_SEAT_FREE(hVehicle, seatId))
        {
            auto passengerHandle = CleoFunctions::GET_ACTOR_HANDLE_FROM_CAR_PASSENGER_SEAT(hVehicle, seatId);

            //Log::Level(LOG_LEVEL::LOG_BOTH) << "passengerHandle: " << passengerHandle << std::endl;

            passengers.push_back(passengerHandle);
        } else {
            //Log::Level(LOG_LEVEL::LOG_BOTH) << "seat " << seatId << " is free" << std::endl;
        }
    }

    return passengers;
}

int Vehicle::GetDriver()
{
    return CleoFunctions::GET_DRIVER_OF_CAR(hVehicle);
}

std::vector<int> Vehicle::GetDriverAndPassengers()
{
    std::vector<int> peds;

    auto driver = GetDriver();
    if(driver > 0) peds.push_back(driver);

    auto passengersHandle = GetPassengers();
    for(auto passengerHandle : passengersHandle)
    {
        peds.push_back(passengerHandle);
    }

    return peds;
}

std::vector<int> Vehicle::GetOwners()
{
    std::vector<int> owners;

    if(hDriverOwner > 0) owners.push_back(hDriverOwner);

    for(auto passengerHandle : hPassengersOwner)
    {
        owners.push_back(passengerHandle);
    }

    return owners;
}
void Vehicle::SetDriverAndPassengersOwners()
{
    Log::Level(LOG_LEVEL::LOG_BOTH) << "Vehicle: SetDriverAndPassengersOwners" << std::endl;

    hDriverOwner = GetDriver();

    Log::Level(LOG_LEVEL::LOG_BOTH) << "driver onwer: " << hDriverOwner << std::endl;

    //set vehicle owned for driver
    if(hDriverOwner > 0)
    {
        auto driverPed = Peds::TryCreatePed(hDriverOwner);
        driverPed->hVehicleOwned = hVehicle;
    }

    Log::Level(LOG_LEVEL::LOG_BOTH) << "getting passengers..." << std::endl;

    hPassengersOwner.clear();
    auto passengersHandle = GetPassengers();
    for(auto passengerHandle : passengersHandle)
    {
        hPassengersOwner.push_back(passengerHandle);
        Log::Level(LOG_LEVEL::LOG_BOTH) << "passenger onwer: " << passengerHandle << std::endl;
    }

    Log::Level(LOG_LEVEL::LOG_BOTH) << hPassengersOwner.size() << " passengers" << std::endl;
}

bool Vehicle::IsAllDriverAndPassengersInsideCar()
{
    CheckDriverAndPassengersAreAlive();

    int requiredPeds = 0;
    int pedsOnCar = 0;
    
    if(IsActorAliveAndDefined(hDriverOwner))
    {
        requiredPeds++;

        if(CleoFunctions::IS_CHAR_IN_ANY_CAR(hDriverOwner)) pedsOnCar++;
    }

    int seatId = 0;
    for(auto passenger : hPassengersOwner)
    {
        if(IsActorAliveAndDefined(passenger))
        {
            requiredPeds++;

            if(CleoFunctions::IS_CHAR_IN_ANY_CAR(passenger)) pedsOnCar++;
        }
        seatId++;
    }

    //Log::Level(LOG_LEVEL::LOG_BOTH) << "waiting: " << pedsOnCar << " / " << requiredPeds << std::endl;

    if(requiredPeds == 0)
    {
        Log::Level(LOG_LEVEL::LOG_BOTH) << "requiredPeds = 0, no peds alive, ignoring" << std::endl;
        return true;
    }

    if(requiredPeds == pedsOnCar)
    {
        return true;
    }
    
    return false;
}

bool Vehicle::IsAllDriverAndPassengersOutsideCar()
{
    int pedsOnCar = 0;

    if(IsActorAliveAndDefined(hDriverOwner))
    {
        if(CleoFunctions::IS_CHAR_IN_ANY_CAR(hDriverOwner)) pedsOnCar++;
    }

    for(auto passengerHandle : hPassengersOwner)
    {
        if(IsActorAliveAndDefined(passengerHandle))
        {
            if(CleoFunctions::IS_CHAR_IN_ANY_CAR(passengerHandle)) pedsOnCar++;
        }
    }

    return pedsOnCar == 0;
}

float Vehicle::GetSpeed()
{
    return CleoFunctions::CAR_SPEED(hVehicle);
}