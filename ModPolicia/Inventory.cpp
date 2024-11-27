#include "Inventory.h"

#include "Mod.h"
#include "Log.h"

bool Inventory::HasItemOfType(ItemType type)
{
    for(auto item : items)
    {
        if(item->itemData->type == type) return true;
    }
    return false;
}

std::vector<InventoryItem*> Inventory::GetItemsOfType(ItemType type)
{
    std::vector<InventoryItem*> foundItems;
    for(auto item : items)
    {
        if(item->itemData->type == type)
        {
            foundItems.push_back(item);
        }
    }
    return foundItems;
}

InventoryItem* Inventory::AddItemToInventory(std::string id)
{
    auto item = InventoryItems::CreateInventoryItem(id);

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
        Log::Level(LOG_LEVEL::LOG_BOTH) << "Item: " << item->itemData->name << " x" << item->amount << std::endl;
    }

    RemoveAllItemsFromInventory();

    for(auto item : fromInventory->items)
    {
        auto newItem = AddItemToInventory(item->itemData->id);
        newItem->amount = item->amount;
        newItem->isStolen = item->isStolen;
    }

    Log::Level(LOG_LEVEL::LOG_BOTH) << "After copying:" << std::endl;

    for(auto item : items)
    {
        Log::Level(LOG_LEVEL::LOG_BOTH) << "Item: " << item->itemData->name << " x" << item->amount << std::endl;
    }
}