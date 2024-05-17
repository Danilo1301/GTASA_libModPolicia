#pragma once

#include "pch.h"

#include "SimpleGTA.h"

#include "Inventory.h"

enum ACTION_STATUS {
    ACTION_NONE = 0,

    SCORCH_GOING_TO_PED,
    SCORCH_WAITING_FOR_PED_TO_ENTER,

    WAITING_FOR_PEDS_TO_ENTER_CAR,
    LEAVING_SCENE,

    CHASE_CHASING,

    CALLOUT_GOING_TO_CALLOUT,
    CALLOUT_ACTING_ON_CALLOUT
};

class Vehicle {
public:
    static float CHANCE_VEHICLE_BEEING_STOLEN;
    static float CHANCE_VEHICLE_HAVING_GUNS;

    int hVehicle;
    CVehicle* pVehicle = NULL;
    int modelId;

    int blip = 0;
    
    bool isStolen = false;

    Inventory* inventory = new Inventory();

    int hDriver = 0;
    std::vector<int> hPassengers;

    CVector fromPos = CVector(0, 0, 0);
    CVector drivingTo = CVector(0, 0, 0);

    ACTION_STATUS actionStatus = ACTION_STATUS::ACTION_NONE;

    Vehicle(int hVehicle);
	~Vehicle();

    void Update(int dt);
    void UpdateLeaveScene();
    void UpdateCarMenuWidget();

    void UpdateInventory();

    bool IsPoliceCar();
    
    bool HasIlegalStuff();

    int AddBlip();
    int AddBlip(int color);
    void RemoveBlip();

    void MakePedsEnterVehicleAndLeaveScene();

    void CheckPassengers();
    void ReplaceDriverByAnyPassenger();
};