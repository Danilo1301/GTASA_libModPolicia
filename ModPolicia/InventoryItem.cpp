#include "InventoryItem.h"

std::vector<InventoryItem> InventoryItems::m_Items = {
    {45, Item_Type::CELLPHONE, 0, 1, 1, false, true},
    {46, Item_Type::DOCUMENTS, 0, 1, 1, false, false},
    {47, Item_Type::CIGARETTE, 0, 1, 5, false, false},
    {48, Item_Type::WEED, 0, 1, 10, false, true},
    {60, Item_Type::BEER, 0, 1, 5, false, true},
    {61, Item_Type::REVOLVER_38, 0, 1, 2, false, true},
    {62, Item_Type::PISTOL, 0, 1, 1, false, true},
    {115, Item_Type::STOLEN_WATCH, 0, 1, 1, false, true},
    {116, Item_Type::STOLEN_WALLET, 0, 1, 1, false, true}
};

void InventoryItems::Init()
{
    
}