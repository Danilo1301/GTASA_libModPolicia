#pragma once

#include "pch.h"

enum Item_Type {
    CELLPHONE,
    DOCUMENTS,
    CIGARETTE,
    WEED
};

struct InventoryItem {
    int itemNameGxtId;

    Item_Type type;

    int amount = 0;
    int amountMin = 1;
    int amountMax = 1;

    bool isSlotenCellphone = false;
};

class InventoryItems {
public:
    static std::vector<InventoryItem> m_Items;

    static void Init();
};