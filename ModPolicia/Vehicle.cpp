#include "Vehicle.h"

#include "Log.h"
#include "Mod.h"

extern void* (*GetVehicleFromRef)(int);

Vehicle::Vehicle(int hVehicle)
{
    this->hVehicle = hVehicle;

    this->pVehicle = (CVehicle*)GetVehicleFromRef(hVehicle);
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

    if(Mod::CalculateProbability(0.1))
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