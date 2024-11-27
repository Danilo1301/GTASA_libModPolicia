#pragma once

#include "pch.h"

#include "InventoryItem.h"

class Inventory {
public:
    std::vector<InventoryItem*> items;

    bool created = false;

    bool HasItemOfType(ItemType type);
    std::vector<InventoryItem*> GetItemsOfType(ItemType type);
    InventoryItem* AddItemToInventory(std::string id);
    void RemoveItemFromInventory(InventoryItem* item);
    void RemoveAllItemsFromInventory();
    void CopyFrom(Inventory* fromInventory);
};