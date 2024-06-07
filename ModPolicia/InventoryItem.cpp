#include "InventoryItem.h"

/*
When adding new properties, add them to:
* Inventory::AddItemToInventory(Item_Type type)
* Inventory::CopyFrom(Inventory* fromInventory)
*/
std::vector<InventoryItem> InventoryItems::m_Items = {
    {45, Item_Type::CELLPHONE, 0, 1, 1, ITEM_MEASURE::MEASURE_X, false, true},
    {46, Item_Type::DOCUMENTS, 0, 1, 1, ITEM_MEASURE::MEASURE_X, false, false},
    {47, Item_Type::CIGARETTE, 0, 1, 5, ITEM_MEASURE::MEASURE_X, false, false},
    {48, Item_Type::WEED, 0, 1, 10, ITEM_MEASURE::MEASURE_G, false, true},
    {60, Item_Type::BEER, 0, 1, 5, ITEM_MEASURE::MEASURE_X, false, true},
    {61, Item_Type::REVOLVER_38, 0, 1, 2, ITEM_MEASURE::MEASURE_X, false, true},
    {62, Item_Type::PISTOL, 0, 1, 1, ITEM_MEASURE::MEASURE_X, false, true},
    {115, Item_Type::STOLEN_WATCH, 0, 1, 1, ITEM_MEASURE::MEASURE_X, false, true},
    {116, Item_Type::STOLEN_WALLET, 0, 1, 1, ITEM_MEASURE::MEASURE_X, false, true},
    {133, Item_Type::CRACK, 0, 1, 10, ITEM_MEASURE::MEASURE_G, false, true},
    {134, Item_Type::COCAINE, 0, 1, 10, ITEM_MEASURE::MEASURE_G, false, true},
    {135, Item_Type::K9, 0, 1, 10, ITEM_MEASURE::MEASURE_G, false, true},
    {136, Item_Type::METHAMPHETAMINE, 0, 1, 10, ITEM_MEASURE::MEASURE_G, false, true},
    {137, Item_Type::LANCA_PERFUME, 0, 1, 1, ITEM_MEASURE::MEASURE_X, false, true},
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