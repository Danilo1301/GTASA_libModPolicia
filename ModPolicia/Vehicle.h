#pragma once

#include "pch.h"

#include "SimpleGTA.h"

#include "Inventory.h"

class Vehicle {
public:
    static float CHANCE_VEHICLE_BEEING_STOLEN;
    static float CHANCE_VEHICLE_HAVING_GUNS;

    int hVehicle;
    CVehicle* pVehicle = NULL;

    int blip = 0;
    
    bool isStolen = false;

    Inventory* inventory = new Inventory();

    int hDriver = 0;

    CVector drivingTo = CVector(0, 0, 0);

    Vehicle(int hVehicle);
	~Vehicle();

    void UpdateInventory();

    bool HasIlegalStuff();

    int AddBlip();
    void RemoveBlip();
};