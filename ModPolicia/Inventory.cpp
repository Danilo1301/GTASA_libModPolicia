#include "Inventory.h"

#include "Mod.h"
#include "Log.h"

bool Inventory::HasItemOfType(Item_Type type)
{
    for(auto item : items)
    {
        if(item->type == type) return true;
    }
    return false;
}

InventoryItem* Inventory::AddItemToInventory(Item_Type type)
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

    items.push_back(item);

    return item;
}

void Inventory::RemoveItemFromInventory(InventoryItem* item)
{
    auto it = std::find(items.begin(), items.end(), item);
    if (it == items.end()) return;
    items.erase(it);
    delete item;
}

void Inventory::RemoveAllItemsFromInventory(Item_Type type)
{
    Log::file << "Not implemented" << std::endl;
}