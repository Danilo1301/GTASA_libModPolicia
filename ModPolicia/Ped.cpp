#include "Ped.h"

#include "Log.h"
#include "Mod.h"
#include "CleoFunctions.h"
#include "Vehicles.h"

extern void* (*GetPedFromRef)(int);

float Ped::CHANCE_PED_FORGETTING_DOCUMENTS_AT_HOME = 0.1f;
float Ped::CHANCE_PED_BEEING_DRUG_DEALER = 0.2f;
float Ped::CHANCE_PED_CONSUME_DRUGS = 0.4f;
float Ped::CHANCE_PED_HAVING_EXPIRED_DRIVER_LICENSE = 0.2f;
float Ped::CHANCE_PED_BEEING_WANTED = 0.1f;

Ped::Ped(int hPed)
{
    this->hPed = hPed;

    this->pPed = (CPed*)GetPedFromRef(hPed);

    this->birthDay = Mod::GetRandomNumber(1, 29);
    this->birthMonth = Mod::GetRandomNumber(1, 12);
    this->birthYear = Mod::GetRandomNumber(1964, 1990);

    this->rg[0] = Mod::GetRandomNumber(100, 999);
    this->rg[1] = Mod::GetRandomNumber(100, 999);

    this->cpf[0] = Mod::GetRandomNumber(100, 999);
    this->cpf[1] = Mod::GetRandomNumber(100, 999);

    int currentYear = 2024;
    
    this->cnhRegisterNum = Mod::GetRandomNumber(100000, 999999);
    this->cnhValidDay = Mod::GetRandomNumber(1, 29);
    this->cnhValidMonth = Mod::GetRandomNumber(1, 12);
    this->cnhValidYear = Mod::GetRandomNumber(currentYear, currentYear + 10);
    if(Mod::CalculateProbability(CHANCE_PED_HAVING_EXPIRED_DRIVER_LICENSE)) this->cnhValidYear = Mod::GetRandomNumber(currentYear - 5, currentYear);

    this->isWanted = Mod::CalculateProbability(CHANCE_PED_BEEING_WANTED);

    this->money = Mod::GetRandomNumber(0, 200);
}

Ped::~Ped()
{
    
}

void Ped::Update(int dt)
{
    if(!CleoFunctions::ACTOR_DEFINED(hPed)) return;

    //this code is useless because it didnt fix the issue, its better to check if helf <= 0
    /*
    if(CleoFunctions::ACTOR_DEAD(hPed))
    {
        shouldHandsup = false;
    }
    */
    auto pedHealth = CleoFunctions::ACTOR_HEALTH(hPed);
    if(pedHealth <= 0)
    {
        if(shouldHandsup)
        {
            shouldHandsup = false;
            CleoFunctions::REMOVE_REFERENCES_TO_ACTOR(hPed);        
            CleoFunctions::PERFORM_ANIMATION_AS_ACTOR(hPed, "hndshkfa_swt", "gangs", 200.0f, 0, 0, 0, 0, 10);
        }
    }


    justLeftTheCar = false;
    if(CleoFunctions::IS_CHAR_IN_ANY_CAR(hPed))
    {
        auto inCar = CleoFunctions::ACTOR_USED_CAR(hPed);
        if(inCar != previousCar)
        {
            //Log::Level(LOG_LEVEL::LOG_BOTH) << "Ped " << hPed << " just entered car " << inCar << std::endl;

            previousCar = inCar;
        }
    } else {
        if(previousCar > 0)
        {
            Log::Level(LOG_LEVEL::LOG_BOTH) << "Ped " << hPed << " just left car " << previousCar << std::endl;

            previousCar = 0;
            justLeftTheCar = true;
        }
    }

    if(shouldHandsup)
    {
        if(!CleoFunctions::ACTOR_PERFORMING_ANIMATION(hPed, "handsup"))
        {
            Log::Level(LOG_LEVEL::LOG_BOTH) << "Performing animation: handsup" << std::endl;
            CleoFunctions::PERFORM_ANIMATION_AS_ACTOR(hPed, "handsup", "PED", 4.0f, 0, 0, 0, 1, -1);
        }
    }

    if(driveAfterEnterCar)
    {
        if(CleoFunctions::IS_CHAR_IN_ANY_CAR(hPed))
        {
            driveAfterEnterCar = false;

            Log::Level(LOG_LEVEL::LOG_BOTH) << "Entered car. Now driving..." << std::endl;
            
            CleoFunctions::REMOVE_REFERENCES_TO_CAR(hVehicleOwned);
            CleoFunctions::SET_CAR_ENGINE_OPERATION(hVehicleOwned, true);
            CleoFunctions::SET_CAR_TRAFFIC_BEHAVIOUR(hVehicleOwned, 0);
            CleoFunctions::SET_CAR_TO_PSYCHO_DRIVER(hVehicleOwned);
            CleoFunctions::SET_CAR_MAX_SPEED(hVehicleOwned, 30.0f);
        }
    }
}

void Ped::UpdateInventory()
{
    if(inventory->created) return;

    inventory->created = true;

    //

    bool hasDocuments = !Mod::CalculateProbability(CHANCE_PED_FORGETTING_DOCUMENTS_AT_HOME);
    if(hasDocuments)
    {
        inventory->AddItemToInventory(Item_Type::DOCUMENTS);
    }

    //

    bool isDrugDealer = Mod::CalculateProbability(CHANCE_PED_BEEING_DRUG_DEALER);
    bool consumeDrugs = Mod::CalculateProbability(CHANCE_PED_CONSUME_DRUGS);

    if(isDrugDealer)
    {
        AddDrugs(true);
    } else {
        if(consumeDrugs)
        {
            AddDrugs(false);
        }
    }

    //

    bool hasCellphone = Mod::CalculateProbability(0.92);
    if(hasCellphone)
    {
        auto cellphone = inventory->AddItemToInventory(Item_Type::CELLPHONE);
    }

    if(Mod::CalculateProbability(0.1))
    {
        auto stolenCellphone = inventory->AddItemToInventory(Item_Type::CELLPHONE);
        stolenCellphone->isStolen = true;
    }
}

void Ped::UpdateBreathalyzer()
{
    if(updatedBreathalyzer) return;

    updatedBreathalyzer = true;

    auto vehicle = Vehicles::GetVehicleByHandle(hVehicleOwned);
    bool hasBeer = vehicle->inventory->HasItemOfType(Item_Type::BEER);
    
    if(hasBeer)
    {
        int maximunValue = 4;

        bool drankTooMuch = Mod::GetRandomNumber(0, 1) == 0;

        if(drankTooMuch) maximunValue = 50;

        breathalyzerValue = Mod::GetRandomNumber(0, maximunValue) * 0.01f;
    }
}

bool Ped::HasDocuments()
{
    UpdateInventory();

    return inventory->HasItemOfType(Item_Type::DOCUMENTS);
}

bool Ped::HasGuns()
{
    if(inventory->HasItemOfType(Item_Type::REVOLVER_38)) return true;
    if(inventory->HasItemOfType(Item_Type::PISTOL)) return true;
    return false;
}

int Ped::AddBlip()
{
    if(blip > 0) RemoveBlip();
    blip = CleoFunctions::ADD_BLIP_FOR_CHAR(hPed);
    return blip;
}

int Ped::AddBlip(int color)
{
    int blip = AddBlip();
    CleoFunctions::SET_MARKER_COLOR_TO(blip, color);
    return blip;
}

void Ped::RemoveBlip()
{
    if(blip <= 0) return;

    CleoFunctions::DISABLE_MARKER(blip);
    blip = 0;
}

void Ped::CopyFrom(Ped* fromPed)
{
    Log::Level(LOG_LEVEL::LOG_BOTH) << "Copy info from ped " << fromPed->hPed << " to ped " << hPed << std::endl;

    this->birthDay = fromPed->birthDay;
    this->birthMonth = fromPed->birthMonth;
    this->birthYear = fromPed->birthYear;

    this->rg[0] = fromPed->rg[0];
    this->rg[1] = fromPed->rg[1];

    this->cpf[0] = fromPed->cpf[0];
    this->cpf[1] = fromPed->cpf[1];

    this->cnhRegisterNum = fromPed->cnhRegisterNum;
    this->cnhValidDay = fromPed->cnhValidDay;
    this->cnhValidMonth = fromPed->cnhValidMonth;
    this->cnhValidYear = fromPed->cnhValidYear;

    this->isWanted = fromPed->isWanted;

    this->money = fromPed->money;

    this->inventory->CopyFrom(fromPed->inventory);
}

void Ped::AddDrugs(bool drugDealer)
{
    int drugRng = Mod::GetRandomNumber(0, 4);

    if(drugRng == 0)
    {
        auto drug = inventory->AddItemToInventory(Item_Type::WEED);
        if(drugDealer)
        {
            drug->amount = Mod::GetRandomNumber(10, 50);
        }
    }

    if(drugRng == 1)
    {
        auto drug = inventory->AddItemToInventory(Item_Type::CRACK);
        if(drugDealer)
        {
            drug->amount = Mod::GetRandomNumber(10, 50);
        }
    }

    if(drugRng == 2)
    {
        auto drug = inventory->AddItemToInventory(Item_Type::COCAINE);
        if(drugDealer)
        {
            drug->amount = Mod::GetRandomNumber(10, 50);
        }
    }

    if(drugRng == 3)
    {
        auto drug = inventory->AddItemToInventory(Item_Type::K9);
        if(drugDealer)
        {
            drug->amount = Mod::GetRandomNumber(10, 50);
        }
    }

    if(drugRng == 4)
    {
        auto drug = inventory->AddItemToInventory(Item_Type::METHAMPHETAMINE);
        if(drugDealer)
        {
            drug->amount = Mod::GetRandomNumber(10, 50);
        }
    }

    if(Mod::CalculateProbability(0.3))
    {
        inventory->AddItemToInventory(Item_Type::LANCA_PERFUME);
    }
}

bool Ped::PedHasWeaponId(int hPed, int weaponId)
{
    for(int i = 1; i <= 13; i++)
    {
        int weaponType = 0, weaponAmmo = 0, weaponModel = 0;

        CleoFunctions::GET_WEAPON_DATA_FROM_ACTOR(hPed, i, &weaponType, &weaponAmmo, &weaponModel);

        if(weaponId == weaponType) return true;

        //Log::Level(LOG_LEVEL::LOG_BOTH) << "slot " << i << ": " << weaponType << "|" << weaponAmmo << "|" <<weaponModel << std::endl;
    }
    return false;
}