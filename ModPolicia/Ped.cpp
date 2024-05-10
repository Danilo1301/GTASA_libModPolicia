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
    if(updatedInventory) return;

    updatedInventory = true;

    bool hasDocuments = !Mod::CalculateProbability(CHANCE_FORGETTING_DOCUMENTS_AT_HOME);
    if(hasDocuments)
    {
        AddItemToInventory(Item_Type::DOCUMENTS);
    }

    bool hasDrugs = Mod::CalculateProbability(CHANCE_CONSUME_DRUGS);
    if(hasDrugs)
    {
        AddItemToInventory(Item_Type::WEED);
    }

    bool hasCellphone = Mod::CalculateProbability(0.95);
    if(hasCellphone)
    {
        auto cellphone = AddItemToInventory(Item_Type::CELLPHONE);
    }

    if(Mod::CalculateProbability(0.10))
    {
        auto stolenCellphone = AddItemToInventory(Item_Type::CELLPHONE);
        stolenCellphone->isSlotenCellphone = true;
    }
}

bool Ped::HasItemOfType(Item_Type type)
{
    for(auto item : inventoryItems)
    {
        if(item->type == type) return true;
    }
    return false;
}

InventoryItem* Ped::AddItemToInventory(Item_Type type)
{
    auto item = new InventoryItem();

    for(auto itemInfo : InventoryItems::m_Items)
    {
        if(itemInfo.type != type) continue;

        item->itemNameGxtId = itemInfo.itemNameGxtId;
        item->type = itemInfo.type;
        item->amount = Mod::GetRandomNumber(itemInfo.amountMin, itemInfo.amountMax);
        item->amountMin = itemInfo.amountMin;
        item->amountMax = itemInfo.amountMax;
        item->isSlotenCellphone = itemInfo.isSlotenCellphone;

    }

    inventoryItems.push_back(item);

    return item;
}

void Ped::RemoveItemFromInventory(InventoryItem* item)
{
    auto it = std::find(inventoryItems.begin(), inventoryItems.end(), item);
    if (it == inventoryItems.end()) return;
    inventoryItems.erase(it);
    delete item;
}

void Ped::RemoveAllItemsFromInventory(Item_Type type)
{
    Log::file << "Not implemented" << std::endl;
}

bool Ped::HasDocuments()
{
    UpdateInventory();

    return HasItemOfType(Item_Type::DOCUMENTS);
}