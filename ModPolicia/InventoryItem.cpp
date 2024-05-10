#include "InventoryItem.h"

std::vector<InventoryItem> InventoryItems::m_Items = {
    {45, Item_Type::CELLPHONE, 0, 1, 1, false},
    {46, Item_Type::DOCUMENTS, 0, 1, 1, false},
    {47, Item_Type::CIGARETTE, 0, 1, 5, false},
    {48, Item_Type::WEED, 0, 1, 5, false}
};

void InventoryItems::Init()
{
    
}