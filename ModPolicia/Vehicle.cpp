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