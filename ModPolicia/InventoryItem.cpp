#include "InventoryItem.h"

/*
When adding new properties, add them to:
* Inventory::AddItemToInventory(Item_Type type)
* Inventory::CopyFrom(Inventory* fromInventory)
*/
std::vector<InventoryItem> InventoryItems::m_Items = {
    {"Celular", 45, Item_Type::CELLPHONE, 0, 1, 1, ITEM_MEASURE::MEASURE_X, false, true},
    {"Carteira", 46, Item_Type::DOCUMENTS, 0, 1, 1, ITEM_MEASURE::MEASURE_X, false, false},
    {"Cigarro", 47, Item_Type::CIGARETTE, 0, 1, 5, ITEM_MEASURE::MEASURE_X, false, false},
    {"Maconha", 48, Item_Type::WEED, 0, 1, 10, ITEM_MEASURE::MEASURE_G, false, true},
    {"Cerveja", 60, Item_Type::BEER, 0, 1, 5, ITEM_MEASURE::MEASURE_X, false, true},
    {"Revolver 38", 61, Item_Type::REVOLVER_38, 0, 1, 2, ITEM_MEASURE::MEASURE_X, false, true},
    {"Pistola", 62, Item_Type::PISTOL, 0, 1, 1, ITEM_MEASURE::MEASURE_X, false, true},
    {"Relogio roubado", 115, Item_Type::STOLEN_WATCH, 0, 1, 1, ITEM_MEASURE::MEASURE_X, false, true},
    {"Carteira roubada", 116, Item_Type::STOLEN_WALLET, 0, 1, 1, ITEM_MEASURE::MEASURE_X, false, true},
    {"Crack", 133, Item_Type::CRACK, 0, 1, 10, ITEM_MEASURE::MEASURE_G, false, true},
    {"Cocaina", 134, Item_Type::COCAINE, 0, 1, 10, ITEM_MEASURE::MEASURE_G, false, true},
    {"K9", 135, Item_Type::K9, 0, 1, 10, ITEM_MEASURE::MEASURE_G, false, true},
    {"Metanfetamina", 136, Item_Type::METHAMPHETAMINE, 0, 1, 10, ITEM_MEASURE::MEASURE_G, false, true},
    {"Lanca perfume", 137, Item_Type::LANCA_PERFUME, 0, 1, 1, ITEM_MEASURE::MEASURE_X, false, true},
};

std::map<ITEM_MEASURE, int> InventoryItems::m_MeasureGxtId = {
    {ITEM_MEASURE::MEASURE_X, 49},
    {ITEM_MEASURE::MEASURE_G, 132}
};

void InventoryItems::Init()
{
    
}

int InventoryItems::GetMeasureGxtId(ITEM_MEASURE measureType)
{
    return m_MeasureGxtId[measureType];
}

std::string InventoryItems::FormatItemAmount(InventoryItem* item)
{
    std::string amountStr = std::to_string(item->amount);

    if(item->measure == ITEM_MEASURE::MEASURE_G)
    {
        return amountStr + "g";
    }
    if(item->measure == ITEM_MEASURE::MEASURE_X)
    {
        return "x" + amountStr;
    }
    return amountStr;
}