#include "Ped.h"

#include "Log.h"
#include "Mod.h"
#include "CleoFunctions.h"

extern void* (*GetPedFromRef)(int);

float Ped::CHANCE_PED_FORGETTING_DOCUMENTS_AT_HOME = 0.1f;
float Ped::CHANCE_PED_BEEING_DRUG_DEALER = 0.3f;
float Ped::CHANCE_PED_CONSUME_DRUGS = 0.3f;
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
}

Ped::~Ped()
{
    
}

void Ped::Update(int dt)
{
    if(shouldHandsup)
    {
        if(!CleoFunctions::ACTOR_PERFORMING_ANIMATION(hPed, "handsup"))
        {
            Log::file << "Performing animation: handsup" << std::endl;
            CleoFunctions::PERFORM_ANIMATION_AS_ACTOR(hPed, "handsup", "PED", 4.0f, 0, 0, 0, 1, -1);
        }
    }
}

void Ped::UpdateInventory()
{
    if(inventory->created) return;

    inventory->created = true;

    bool hasDocuments = !Mod::CalculateProbability(CHANCE_PED_FORGETTING_DOCUMENTS_AT_HOME);
    if(hasDocuments)
    {
        inventory->AddItemToInventory(Item_Type::DOCUMENTS);
    }

    bool hasDrugs = Mod::CalculateProbability(CHANCE_PED_CONSUME_DRUGS);
    if(hasDrugs)
    {
        inventory->AddItemToInventory(Item_Type::WEED);
    }

    bool hasCellphone = Mod::CalculateProbability(0.92);
    if(hasCellphone)
    {
        auto cellphone = inventory->AddItemToInventory(Item_Type::CELLPHONE);
    }

    if(Mod::CalculateProbability(0.1))
    {
        auto stolenCellphone = inventory->AddItemToInventory(Item_Type::CELLPHONE);
        stolenCellphone->isSlotenCellphone = true;
    }
}

bool Ped::HasDocuments()
{
    UpdateInventory();

    return inventory->HasItemOfType(Item_Type::DOCUMENTS);
}

void Ped::AddBlip()
{
    if(blip > 0) RemoveBlip();
    blip = CleoFunctions::ADD_BLIP_FOR_CHAR(hPed);
}

void Ped::RemoveBlip()
{
    if(blip <= 0) return;

    CleoFunctions::DISABLE_MARKER(blip);
    blip = 0;
}