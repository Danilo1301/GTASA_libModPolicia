#pragma once

#include "pch.h"

enum ItemType
{
    UNKNOWN,
    DOCUMMENTS,
    CELLPHONE,
    COMMON_ITEM,
    STOLEN_ITEM,
    ILEGAL_GUN,
    ILEGAL_DRUG,
    ALCOHOL_DRINK
};


enum ItemMeasure {
    MEASURE_X,
    MEASURE_G
};

struct ItemData {
    std::string id;
    std::string name;

    ItemType type;

    int amountMin = 1;
    int amountMax = 1;
    ItemMeasure measure = ItemMeasure::MEASURE_X;

    bool canConfiscate = false;
    bool canSpawnInPeds = false;
    bool canSpawnInVehicles = false;

    float chance = 1.00;
};

struct InventoryItem {
    ItemData* itemData;

    int amount = 0;
    bool isStolen = false;
};

class InventoryItems {
public:
    static std::map<std::string, ItemData*> m_ItemDatas;

    static void Init();

    static void LoadItemsFromFolder(std::string folder, ItemType type);
    static void LoadItems();
    
    static ItemData* CreateItemData(std::string id, std::string name, ItemType type, ItemMeasure measure, int min, int max, bool canAprehend);
    static bool HasItemData(std::string id);
    static std::vector<ItemData*> GetItemsDataOfType(ItemType type);

    static InventoryItem* CreateInventoryItem(std::string id);

    static std::string FormatItemAmount(InventoryItem* item);
};