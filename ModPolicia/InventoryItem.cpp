#include "InventoryItem.h"

#include "Log.h"
#include "ModConfig.h"

#include "iniconfig/INIFile.h"

/*
When adding new properties, add them to:
* Inventory::AddItemToInventory(Item_Type type)
* Inventory::CopyFrom(Inventory* fromInventory)
*/

std::map<std::string, ItemData*> InventoryItems::m_ItemDatas;

void InventoryItems::Init()
{
    const bool CAN_CONFISCATE = true;
    const bool CANT_CONFISCATE = false;

    auto unknown = CreateItemData("unknown", "Desconhecido", ItemType::UNKNOWN, ItemMeasure::MEASURE_X, 1, 1, CANT_CONFISCATE);

    auto documments = CreateItemData("documments", "Documentos", ItemType::DOCUMMENTS, ItemMeasure::MEASURE_X, 1, 1, CANT_CONFISCATE);
    auto cellphone = CreateItemData("cellphone", "Celular", ItemType::CELLPHONE, ItemMeasure::MEASURE_X, 1, 1, CAN_CONFISCATE);

    // spawn randomly

    auto beer = CreateItemData("beer", "Cerveja", ItemType::ALCOHOL_DRINK, ItemMeasure::MEASURE_X, 1, 5, CAN_CONFISCATE);
    beer->chance = 0.50;

    //

    auto weed = CreateItemData("weed", "Maconha", ItemType::ILEGAL_DRUG, ItemMeasure::MEASURE_G, 1, 10, CAN_CONFISCATE);
    auto crack = CreateItemData("crack", "Crack", ItemType::ILEGAL_DRUG, ItemMeasure::MEASURE_G, 1, 10, CAN_CONFISCATE);
    auto cocaine = CreateItemData("cocaine", "Cocaina", ItemType::ILEGAL_DRUG, ItemMeasure::MEASURE_G, 1, 10, CAN_CONFISCATE);
    auto k9 = CreateItemData("k9", "K9", ItemType::ILEGAL_DRUG, ItemMeasure::MEASURE_G, 1, 10, CAN_CONFISCATE);
    auto metanfetamina = CreateItemData("metanfetamina", "Metanfetamina", ItemType::ILEGAL_DRUG, ItemMeasure::MEASURE_G, 1, 10, CAN_CONFISCATE);
    auto lanca_perfume = CreateItemData("lanca_perfume", "Lanca perfume", ItemType::ILEGAL_DRUG, ItemMeasure::MEASURE_X, 1, 1, CAN_CONFISCATE);
    auto pino_de_po = CreateItemData("pino_de_po", "Pino de po", ItemType::ILEGAL_DRUG, ItemMeasure::MEASURE_X, 1, 3, CAN_CONFISCATE);

    auto revolver38 = CreateItemData("revolver38", "Revolver 38", ItemType::ILEGAL_GUN, ItemMeasure::MEASURE_X, 1, 1, CAN_CONFISCATE);
    auto pistol = CreateItemData("pistol", "Pistola", ItemType::ILEGAL_GUN, ItemMeasure::MEASURE_X, 1, 1, CAN_CONFISCATE);
    auto rifle = CreateItemData("rifle", "Fuzil", ItemType::ILEGAL_GUN, ItemMeasure::MEASURE_X, 1, 1, CAN_CONFISCATE);
    auto handmade_gun = CreateItemData("handmade_gun", "Arma caseira", ItemType::ILEGAL_GUN, ItemMeasure::MEASURE_X, 1, 1, CAN_CONFISCATE);
    auto knife = CreateItemData("knife", "Faca", ItemType::ILEGAL_GUN, ItemMeasure::MEASURE_X, 1, 1, CAN_CONFISCATE);
    
    auto stolen_watch = CreateItemData("stolen_watch", "Relogio roubado", ItemType::STOLEN_ITEM, ItemMeasure::MEASURE_X, 1, 2, CAN_CONFISCATE);
    auto stolen_wallet = CreateItemData("stolen_wallet", "Carteira roubada", ItemType::STOLEN_ITEM, ItemMeasure::MEASURE_X, 1, 2, CAN_CONFISCATE);
}

void InventoryItems::LoadItems()
{
    Log::Level(LOG_LEVEL::LOG_BOTH) << "InventoryItems: LoadItems" << std::endl;

    auto path = ModConfig::GetConfigFolder() + "/items/common_items/";
    auto type = ItemType::COMMON_ITEM;

    if(!ModConfig::DirExists(path))
    {
        Log::Level(LOG_LEVEL::LOG_BOTH) << "WARNING: Path does not exist!" << std::endl;
        Log::Level(LOG_LEVEL::LOG_BOTH) << path << std::endl;
        return;
    }

    auto files = ModConfig::GetFilesName(path);
    for(auto itemFile : files)
    {
        Log::Level(LOG_LEVEL::LOG_BOTH) << "Loading item: " << itemFile << std::endl;

        auto filePath = path + "/" + itemFile;

        auto item = CreateItemData(filePath, "", type, ItemMeasure::MEASURE_X, 1, 1, true);

        INIFile iniFile;
        if (iniFile.Read(filePath))
        {   
            if(auto section = iniFile.GetFirstSection("Item"))
            {
                item->name = section->GetString("name");
                section->GetFloat("chance", &item->chance);
                section->GetBool("spawnOnPeds", &item->canSpawnInPeds);
                section->GetBool("spawnOnVehicles", &item->canSpawnInVehicles);
                section->GetBool("canConfiscate", &item->canConfiscate);
                section->GetInt("amountMin", &item->amountMin);
                section->GetInt("amountMax", &item->amountMax);

                std::string measureText = section->GetString("measure");
                if(measureText.compare("g") == 0)
                {
                    item->measure = ItemMeasure::MEASURE_G;
                } else {
                    item->measure = ItemMeasure::MEASURE_X;
                }
            }
        } else {
            Log::Level(LOG_LEVEL::LOG_BOTH) << "ModConfig: Error reading " << filePath << std::endl;
        }
    }
}

ItemData* InventoryItems::CreateItemData(std::string id, std::string name, ItemType type, ItemMeasure measure, int min, int max, bool canConfiscate)
{
    auto itemData = new ItemData();
    itemData->id = id;
    itemData->name = name;
    itemData->type = type;
    itemData->measure = measure;
    itemData->amountMin = min;
    itemData->amountMax = max;
    itemData->canConfiscate = canConfiscate;

    m_ItemDatas[id] = itemData;

    return itemData;
}

bool InventoryItems::HasItemData(std::string id)
{
    return m_ItemDatas.find(id) != m_ItemDatas.end();
}

std::vector<ItemData*> InventoryItems::GetItemsDataOfType(ItemType type)
{
    std::vector<ItemData*> items;

    for(auto pair : m_ItemDatas)
    {
        auto itemData = pair.second;

        if(itemData->type == type)
        {
            items.push_back(itemData);
        }
    }
    return items;
}

InventoryItem* InventoryItems::CreateInventoryItem(std::string id)
{
    if(!HasItemData(id))
    {
        Log::Level(LOG_LEVEL::LOG_BOTH) << "Item id " + id + " not found" << std::endl;

        return CreateInventoryItem("unknown");
    }

    ItemData* itemData = m_ItemDatas[id];
    
    auto item = new InventoryItem();
    item->itemData = itemData;
    item->amount = GetRandomNumber(itemData->amountMin, itemData->amountMax);

    return item;
}

std::string InventoryItems::FormatItemAmount(InventoryItem* item)
{
    std::string amountStr = std::to_string(item->amount);

    if(item->itemData->measure  == ItemMeasure::MEASURE_G)
    {
        return amountStr + "g";
    }
    if(item->itemData->measure == ItemMeasure::MEASURE_X)
    {
        return "x" + amountStr;
    }
    return amountStr;
}