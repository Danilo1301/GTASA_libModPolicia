#include "Ped.h"

#include "Log.h"
#include "Mod.h"
#include "CleoFunctions.h"
#include "Vehicles.h"

extern void* (*GetPedFromRef)(int);

float Ped::CHANCE_PED_FORGETTING_DOCUMENTS_AT_HOME = 0.1f;
float Ped::CHANCE_PED_BEEING_DRUG_DEALER = 0.1f;
float Ped::CHANCE_PED_CONSUME_DRUGS = 0.2f;
float Ped::CHANCE_PED_HAVING_EXPIRED_DRIVER_LICENSE = 0.2f;
float Ped::CHANCE_PED_BEEING_WANTED = 0.1f;
float Ped::CHANCE_PED_RESISTING_PULLOVER = 0.08f;

Ped::Ped(int hPed)
{
    this->hPed = hPed;

    this->pPed = (CPed*)GetPedFromRef(hPed);

    this->birthDay = GetRandomNumber(1, 29);
    this->birthMonth = GetRandomNumber(1, 12);
    this->birthYear = GetRandomNumber(1964, 1990);

    this->rg = Names::GenerateRandomRG();

    this->cpf = Names::GenerateRandomCPF();

    int currentYear = 2024;
    
    this->cnhRegisterNum = GetRandomNumber(100000, 999999);
    this->cnhValidDay = GetRandomNumber(1, 29);
    this->cnhValidMonth = GetRandomNumber(1, 12);
    this->cnhValidYear = GetRandomNumber(currentYear, currentYear + 10);
    if(Mod::CalculateProbability(CHANCE_PED_HAVING_EXPIRED_DRIVER_LICENSE)) this->cnhValidYear = GetRandomNumber(currentYear - 5, currentYear);

    this->isWanted = Mod::CalculateProbability(CHANCE_PED_BEEING_WANTED);

    this->money = GetRandomNumber(0, 200);

    //crimeCodes
    if(Mod::CalculateProbability(0.3))
    {
        std::vector<int> crimes = {157, 171, 147, 121};

        int numCrimes = 1;
        if(Mod::CalculateProbability(0.5)) numCrimes = 2;

        for(int i = 0; i < numCrimes; i++)
        {
            int rnd = GetRandomNumber(0, crimes.size() - 1);
            crimeCodes.push_back(crimes[rnd]);
            crimes.erase(crimes.begin() + rnd);
        }
    }

    this->willShootAtCops = Mod::CalculateProbability(CHANCE_PED_RESISTING_PULLOVER);

    this->isMale = CleoFunctions::ACTOR_MALE(hPed);
    this->name = Names::GetRandomName(isMale ? SkinGenre::SKIN_MALE : SkinGenre::SKIN_FEMALE);

    //test
    this->id = GetRandomNumber(0, 200);
    this->color = CRGBA(GetRandomNumber(0, 255), GetRandomNumber(0, 255), GetRandomNumber(0, 255));
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
            CleoFunctions::CLEAR_ACTOR_TASK(hPed);
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
        inventory->AddItemToInventory("documments");
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
        auto cellphone = inventory->AddItemToInventory("cellphone");
    }

    if(Mod::CalculateProbability(0.1))
    {
        auto stolenCellphone = inventory->AddItemToInventory("cellphone");
        stolenCellphone->isStolen = true;
    }

    // if(Mod::CalculateProbability(0.20))
    // {
    //     inventory->AddItemToInventory("revolver38");
    // } else if(Mod::CalculateProbability(0.20))
    // {
    //     inventory->AddItemToInventory("handmade_gun");
    // } else if(Mod::CalculateProbability(0.10))
    // {
    //     inventory->AddItemToInventory("knife");
    // }

    for(auto itemData : InventoryItems::GetItemsDataOfType(ItemType::COMMON_ITEM))
    {
        if(itemData->canSpawnInPeds)
        {
            if(Mod::CalculateProbability(itemData->chance))
            {
                inventory->AddItemToInventory(itemData->id);
            }
        }
    }

    for(auto itemData : InventoryItems::GetItemsDataOfType(ItemType::ILEGAL_GUN))
    {
        if(itemData->canSpawnInPeds)
        {
            if(Mod::CalculateProbability(itemData->chance))
            {
                inventory->AddItemToInventory(itemData->id);
            }
        }
    }
}

void Ped::UpdateBreathalyzer()
{
    if(updatedBreathalyzer) return;

    updatedBreathalyzer = true;

    auto vehicle = Vehicles::GetVehicleByHandle(hVehicleOwned);
    bool hasBeer = vehicle->inventory->HasItemOfType(ItemType::ALCOHOL_DRINK);
    
    if(hasBeer)
    {
        int maximunValue = 4;

        bool drankTooMuch = Mod::CalculateProbability(0.7);

        if(drankTooMuch) maximunValue = 50;

        breathalyzerValue = GetRandomNumber(0, maximunValue) * 0.01f;
    }
}

bool Ped::HasDocuments()
{
    UpdateInventory();

    return inventory->HasItemOfType(ItemType::DOCUMMENTS);
}

bool Ped::HasGuns()
{
    if(inventory->HasItemOfType(ItemType::ILEGAL_GUN)) return true;
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

    this->willShootAtCops = fromPed->willShootAtCops;
}

void Ped::AddDrugs(bool drugDealer)
{
    int drugRng = GetRandomNumber(0, 4);

    if(drugRng == 0)
    {
        auto drug = inventory->AddItemToInventory("weed");
        if(drugDealer)
        {
            drug->amount = GetRandomNumber(10, 50);
        }
    }

    if(drugRng == 1)
    {
        auto drug = inventory->AddItemToInventory("crack");
        if(drugDealer)
        {
            drug->amount = GetRandomNumber(10, 50);
        }
    }

    if(drugRng == 2)
    {
        auto drug = inventory->AddItemToInventory("cocaine");
        if(drugDealer)
        {
            drug->amount = GetRandomNumber(10, 50);
        }
    }

    if(drugRng == 3)
    {
        auto drug = inventory->AddItemToInventory("k9");
        if(drugDealer)
        {
            drug->amount = GetRandomNumber(10, 50);
        }
    }

    if(drugRng == 4)
    {
        auto drug = inventory->AddItemToInventory("metanfetamina");
        if(drugDealer)
        {
            drug->amount = GetRandomNumber(10, 50);
        }
    }

    if(Mod::CalculateProbability(0.3))
    {
        inventory->AddItemToInventory("lanca_perfume");
    }

    if(Mod::CalculateProbability(0.5))
    {
        inventory->AddItemToInventory("pino_de_po");
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

int Ped::GetDialogResponse(eDialogId id)
{
    if(dialogResponses.count(id)) return dialogResponses[id];
    return -1;
}