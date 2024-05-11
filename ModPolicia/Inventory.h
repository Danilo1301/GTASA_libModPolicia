#pragma once

#include "pch.h"

#include "InventoryItem.h"

class Inventory {
public:
    std::vector<InventoryItem*> items;

    bool created = false;

    bool HasItemOfType(Item_Type type);
    InventoryItem* AddItemToInventory(Item_Type type);
    void RemoveItemFromInventory(InventoryItem* item);
    void RemoveAllItemsFromInventory(Item_Type type);
};