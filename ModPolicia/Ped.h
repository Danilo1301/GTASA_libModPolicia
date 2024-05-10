#pragma once

#include "pch.h"

#include "SimpleGTA.h"

#include "InventoryItem.h"

class Ped {
public:
    static float CHANCE_FORGETTING_DOCUMENTS_AT_HOME;
    static float CHANCE_BEEING_DRUG_DEALER;
    static float CHANCE_CONSUME_DRUGS;

    int hPed;
    CPed* pPed = NULL;

    int birthDay;
    int birthMonth;
    int birthYear;

    int rg[2];

    int cpf[2];

    int cnhRegisterNum;
    int cnhValidDay;
    int cnhValidMonth;
    int cnhValidYear;

    std::vector<InventoryItem*> inventoryItems;
    bool updatedInventory = false;

    Ped(int hPed);
	~Ped();

    void UpdateInventory();
    bool HasItemOfType(Item_Type type);
    InventoryItem* AddItemToInventory(Item_Type type);
    void RemoveItemFromInventory(InventoryItem* item);
    void RemoveAllItemsFromInventory(Item_Type type);

    bool HasDocuments();
};