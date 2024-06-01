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

std::vector<InventoryItem*> Inventory::GetItemsOfType(Item_Type type)
{
    std::vector<InventoryItem*> foundItems;
    for(auto item : items)
    {
        if(item->type == type)
        {
            foundItems.push_back(item);
        }
    }
    return foundItems;
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
        item->isStolen = itemInfo.isStolen;
        item->canBeAprehended = itemInfo.canBeAprehended;
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

void Inventory::RemoveAllItemsFromInventory()
{
    while (items.size() > 0)
    {
        RemoveItemFromInventory(items[0]);
    }
}

void Inventory::CopyFrom(Inventory* fromInventory)
{
    Log::Level(LOG_LEVEL::LOG_BOTH) << "Copy inventory" << std::endl;

    Log::Level(LOG_LEVEL::LOG_BOTH) << "Before copying:" << std::endl;

    for(auto item : items)
    {
        Log::Level(LOG_LEVEL::LOG_BOTH) << "Item: " << item->type << " x" << item->amount << std::endl;
    }

    RemoveAllItemsFromInventory();

    for(auto item : fromInventory->items)
    {
        auto newItem = AddItemToInventory(item->type);
        newItem->amount = item->amount;
        newItem->isStolen = item->isStolen;
        newItem->canBeAprehended = item->canBeAprehended;
    }

    Log::Level(LOG_LEVEL::LOG_BOTH) << "After copying:" << std::endl;

    for(auto item : items)
    {
        Log::Level(LOG_LEVEL::LOG_BOTH) << "Item: " << item->type << " x" << item->amount << std::endl;
    }
}