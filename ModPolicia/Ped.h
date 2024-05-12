#pragma once

#include "pch.h"

#include "SimpleGTA.h"

#include "Inventory.h"
#include "Vehicle.h"

class Ped {
public:
    static float CHANCE_PED_FORGETTING_DOCUMENTS_AT_HOME;
    static float CHANCE_PED_BEEING_DRUG_DEALER;
    static float CHANCE_PED_CONSUME_DRUGS;
    static float CHANCE_PED_HAVING_EXPIRED_DRIVER_LICENSE;
    static float CHANCE_PED_BEEING_WANTED;

    int hPed;
    CPed* pPed = NULL;

    Vehicle* vehicleOwned = NULL;

    int blip = 0;

    int birthDay;
    int birthMonth;
    int birthYear;

    int rg[2];

    int cpf[2];

    int cnhRegisterNum;
    int cnhValidDay;
    int cnhValidMonth;
    int cnhValidYear;

    bool shouldHandsup = false;

    bool isWanted = false;

    bool beeingScorched = false;

    Inventory* inventory = new Inventory();

    Ped(int hPed);
	~Ped();

    void Update(int dt);

    void UpdateInventory();

    bool HasDocuments();

    void AddBlip();
    void RemoveBlip();
};