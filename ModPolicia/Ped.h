#pragma once

#include "pch.h"

#include "SimpleGTA.h"

#include "Inventory.h"

enum SCORCH_STATUS {
    NONE = 0,
    WAITING_FOR_CAR,
    ENTERING_CAR,
    BEEING_SCORCHED,
    BEEING_DRIVEN_TO_FAR_AWAY
};

class Ped {
public:
    static float CHANCE_PED_FORGETTING_DOCUMENTS_AT_HOME;
    static float CHANCE_PED_BEEING_DRUG_DEALER;
    static float CHANCE_PED_CONSUME_DRUGS;
    static float CHANCE_PED_HAVING_EXPIRED_DRIVER_LICENSE;
    static float CHANCE_PED_BEEING_WANTED;

    int hPed;
    CPed* pPed = NULL;

    int hVehicleOwned = 0;

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

    SCORCH_STATUS scorchStatus = SCORCH_STATUS::NONE;

    bool arrested = false;

    int waitingForScorchVehicleHandle = 0;

    Inventory* inventory = new Inventory();

    Ped(int hPed);
	~Ped();

    void Update(int dt);

    void UpdateInventory();

    bool HasDocuments();

    void AddBlip();
    void RemoveBlip();
};