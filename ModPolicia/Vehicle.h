#pragma once

#include "pch.h"

#include "SimpleGTA.h"

#include "Inventory.h"

class Vehicle {
public:
    int hVehicle;
    CVehicle* pVehicle = NULL;

    int blip = 0;
    
    Inventory* inventory = new Inventory();

    Vehicle(int hVehicle);
	~Vehicle();

    void UpdateInventory();
};