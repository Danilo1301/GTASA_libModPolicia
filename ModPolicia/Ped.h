#pragma once

#include "pch.h"

#include "SimpleGTA.h"

#include "Inventory.h"

enum PED_ACTION_STATUS {
    PED_ACTION_NONE = 0,

    MEDIC_GOING_TO_BODY,
    MEDIC_FOUND_NO_BODIES,
    MEDIC_ON_BODY,
    MEDIC_TAKING_PICTURES
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
    
    PED_ACTION_STATUS actionStatus = PED_ACTION_STATUS::PED_ACTION_NONE;

    int hVehicleOwned = 0;

    int goingToPed = 0;

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

    bool arrested = false;

    int waitingForScorchVehicleHandle = 0;

    bool driveAfterEnterCar = false;

    int money = 0;

    Inventory* inventory = new Inventory();

    Ped(int hPed);
	~Ped();

    void Update(int dt);

    void UpdateInventory();

    bool HasDocuments();

    int AddBlip();
    int AddBlip(int color);
    void RemoveBlip();

    void CopyFrom(Ped* fromPed);
};