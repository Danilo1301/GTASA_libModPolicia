#include "Vehicle.h"

#include "Log.h"
#include "Mod.h"
#include "CleoFunctions.h"

extern void* (*GetVehicleFromRef)(int);

Vehicle::Vehicle(int hVehicle)
{
    this->hVehicle = hVehicle;

    this->pVehicle = (CVehicle*)GetVehicleFromRef(hVehicle);

    this->isStolen = Mod::CalculateProbability(0.1);
}

Vehicle::~Vehicle()
{
    
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

    if(Mod::CalculateProbability(0.2))
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

void Vehicle::AddBlip()
{
    if(blip > 0) RemoveBlip();
    blip = CleoFunctions::ADD_BLIP_FOR_CHAR(hVehicle);
}

void Vehicle::RemoveBlip()
{
    if(blip <= 0) return;

    CleoFunctions::DISABLE_MARKER(blip);
    blip = 0;
}