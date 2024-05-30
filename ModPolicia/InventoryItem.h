#pragma once

#include "pch.h"

enum Item_Type {
    CELLPHONE,
    DOCUMENTS,
    CIGARETTE,
    WEED,
    BEER,
    REVOLVER_38,
    PISTOL,
    STOLEN_WATCH,
    STOLEN_WALLET
};

/*
int itemNameGxtId
Item_Type type
int amount
int amountMin
int amountMax
bool isStolen
bool canBeAprehended
*/
struct InventoryItem {
    int itemNameGxtId;

    Item_Type type;

    int amount = 0;
    int amountMin = 1;
    int amountMax = 1;

    bool isStolen = false;
    bool canBeAprehended = false;
};

class InventoryItems {
public:
    static std::vector<InventoryItem> m_Items;

    static void Init();
};