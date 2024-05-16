#pragma once

#include "pch.h"

#include "SimpleGTA.h"

#include "Inventory.h"

enum CHASE_STATUS {
    NOT_CHASING = 0,
    CHASING,
    MOVING_AWAY_FROM_CHASE
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

    CVector fromPos = CVector(0, 0, 0);
    CVector drivingTo = CVector(0, 0, 0);

    CHASE_STATUS chaseStatus = CHASE_STATUS::NOT_CHASING;

    Vehicle(int hVehicle);
	~Vehicle();

    void Update(int dt);
    void UpdateCarMenuWidget();

    void UpdateInventory();

    bool IsPoliceCar();
    
    bool HasIlegalStuff();

    int AddBlip();
    int AddBlip(int color);
    void RemoveBlip();
};