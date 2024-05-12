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

    Vehicle(int hVehicle);
	~Vehicle();

    void UpdateInventory();

    bool HasIlegalStuff();

    void AddBlip();
    void RemoveBlip();
};