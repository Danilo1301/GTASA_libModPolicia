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
    STOLEN_WALLET,
    CRACK,
    COCAINE,
    K9,
    METHAMPHETAMINE,
    LANCA_PERFUME
};

enum ITEM_MEASURE {
    MEASURE_X,
    MEASURE_G
};

/*
int itemNameGxtId
Item_Type type
int amount
int amountMin
int amountMax
ITEM_MEASURE measure
bool isStolen
bool canBeAprehended
*/
struct InventoryItem {
    int itemNameGxtId;

    Item_Type type;

    int amount = 0;
    int amountMin = 1;
    int amountMax = 1;
    ITEM_MEASURE measure = ITEM_MEASURE::MEASURE_X;

    bool isStolen = false;
    bool canBeAprehended = false;
};

class InventoryItems {
public:
    static std::vector<InventoryItem> m_Items;
    static std::map<ITEM_MEASURE, int> m_MeasureGxtId;

    static void Init();

    static int GetMeasureGxtId(ITEM_MEASURE measureType);
};