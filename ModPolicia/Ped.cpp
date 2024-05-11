#include "Ped.h"

#include "Log.h"
#include "Mod.h"

extern void* (*GetPedFromRef)(int);

float Ped::CHANCE_FORGETTING_DOCUMENTS_AT_HOME = 0.1f;
float Ped::CHANCE_BEEING_DRUG_DEALER = 0.3f;
float Ped::CHANCE_CONSUME_DRUGS = 0.3f;

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

    this->cnhRegisterNum = Mod::GetRandomNumber(100000, 999999);
    this->cnhValidDay = Mod::GetRandomNumber(1, 29);
    this->cnhValidMonth = Mod::GetRandomNumber(1, 12);
    this->cnhValidYear = Mod::GetRandomNumber(2024 - 5, 2024 + 10);
}

Ped::~Ped()
{
    
}

void Ped::UpdateInventory()
{
    if(inventory->created) return;

    inventory->created = true;

    bool hasDocuments = !Mod::CalculateProbability(CHANCE_FORGETTING_DOCUMENTS_AT_HOME);
    if(hasDocuments)
    {
        inventory->AddItemToInventory(Item_Type::DOCUMENTS);
    }

    bool hasDrugs = Mod::CalculateProbability(CHANCE_CONSUME_DRUGS);
    if(hasDrugs)
    {
        inventory->AddItemToInventory(Item_Type::WEED);
    }

    bool hasCellphone = Mod::CalculateProbability(0.95);
    if(hasCellphone)
    {
        auto cellphone = inventory->AddItemToInventory(Item_Type::CELLPHONE);
    }

    if(Mod::CalculateProbability(0.10))
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